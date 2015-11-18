/*
Copyright (c) 2015, Michael Droettboom
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/

#include "outline.h"
#include "doc/outline.h"

#include "bbox.h"
#include "constants.h"

#include FT_BBOX_H
#include FT_OUTLINE_H


#define MAKE_OUTLINE_GETTER(name, convert_func, member) \
    MAKE_GETTER(Py_Outline, name, convert_func, member)
#define DEF_OUTLINE_GETTER(name) DEF_GETTER(name, doc_Outline_ ## name)
#define OUTLINE_METHOD(name) DEF_METHOD(name, Outline)
#define OUTLINE_METHOD_NOARGS(name) DEF_METHOD_NOARGS(name, Outline)


ftpy_ConstantType Py_FT_ORIENTATION_ConstantType;
ftpy_BitflagType Py_FT_OUTLINE_BitflagType;
ftpy_ConstantType Py_FT_CODES_ConstantType;


/****************************************************************************
 Helper functions
*/

static inline void conic_to_cubic(
    FT_Pos xi0, FT_Pos yi0,
    FT_Pos xi1, FT_Pos yi1,
    FT_Pos xi2, FT_Pos yi2,
    FT_Pos *xo0, FT_Pos *yo0,
    FT_Pos *xo1, FT_Pos *yo1,
    FT_Pos *xo2, FT_Pos *yo2)
{
    *xo0 = (2 * xi1 + xi0) / 3;
    *yo0 = (2 * yi1 + yi0) / 3;
    *xo1 = (xi1 + 2 * xi2) / 3;
    *yo1 = (yi1 + 2 * yi2) / 3;
    *xo2 = xi2;
    *yo2 = yi2;
}

/****************************************************************************
 Outline decompose helper functions
*/


typedef struct {
    PyObject *callback;
    int has_conic_to;
    FT_Pos last_x;
    FT_Pos last_y;
} DecomposeData;


static int
Py_Outline_moveto_func(const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double x = FROM_F26DOT6(to->x);
    double y = FROM_F26DOT6(to->y);

    if (PyObject_CallMethod(obj, "move_to", "((dd))", x, y) == NULL) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_lineto_func(const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double x = FROM_F26DOT6(to->x);
    double y = FROM_F26DOT6(to->y);

    if (PyObject_CallMethod(obj, "line_to", "((dd))", x, y) == NULL) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_conicto_func(const FT_Vector *control, const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double xc = FROM_F26DOT6(control->x);
    double yc = FROM_F26DOT6(control->y);
    double x = FROM_F26DOT6(to->x);
    double y = FROM_F26DOT6(to->y);
    FT_Pos xc0, yc0, xc1, yc1, xc2, yc2;

    if (data->has_conic_to) {
        if (PyObject_CallMethod(obj, "conic_to", "((dd)(dd))", xc, yc, x, y) == NULL) {
            return 0x6;
        }
    } else {
        conic_to_cubic(data->last_x, data->last_y, xc, yc, x, y,
                       &xc0, &yc0, &xc1, &yc1, &xc2, &yc2);
        if (PyObject_CallMethod(
                obj, "cubic_to", "((dd)(dd)(dd))",
                (double)xc0, (double)yc0,
                (double)xc1, (double)yc1,
                (double)xc2, (double)yc2) == NULL) {
            return 0x6;
        }
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_cubicto_func(
    const FT_Vector *control1, const FT_Vector *control2,
    const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double xc1 = FROM_F26DOT6(control1->x);
    double yc1 = FROM_F26DOT6(control1->y);
    double xc2 = FROM_F26DOT6(control2->x);
    double yc2 = FROM_F26DOT6(control2->y);
    double x = FROM_F26DOT6(to->x);
    double y = FROM_F26DOT6(to->y);

    if (PyObject_CallMethod(obj, "cubic_to", "((dd)(dd)(dd))",
            xc1, yc1, xc2, yc2, x, y) == NULL) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}


#define BUFFER_CHUNK_SIZE (1 << 16)


typedef struct {
    char *move_command;
    char *line_command;
    char *cubic_command;
    char *conic_command;
    int relative;
    int prefix;
    double last_x;
    double last_y;
    char *buffer;
    size_t buffer_size;
    size_t cursor;
} DecomposeToStringData;


static int
to_string_expand_buffer(
    DecomposeToStringData *data, size_t len)
{
    size_t new_len = len + 2 + data->cursor;

    if (new_len > data->buffer_size) {
        data->buffer_size = (((new_len / BUFFER_CHUNK_SIZE) + 1) *
                             BUFFER_CHUNK_SIZE);
        data->buffer = PyMem_Realloc(data->buffer, data->buffer_size);
        if (data->buffer == NULL) {
            return -1;
        }
    }

    return 0;
}


static int
append_command_string(
    DecomposeToStringData *data, FT_Pos *points, size_t npoints,
    char *command)
{
    size_t i;
    size_t len;
    char *buf;

    if (data->prefix) {
        len = strlen(command);
        if (to_string_expand_buffer(data, len)) {
            return -1;
        }
        strncpy(data->buffer + data->cursor, command,
                data->buffer_size - data->cursor);
        data->cursor += len;
    }

    for (i = 0; i < npoints; ++i) {
        buf = PyOS_double_to_string(FROM_F26DOT6(points[i]), 'r', 0, 0, NULL);

        len = strnlen(buf, 64);
        if (to_string_expand_buffer(data, len)) {
            return -1;
        }

        strncpy(data->buffer + data->cursor, buf,
                data->buffer_size - data->cursor);
        data->cursor += len;

        PyMem_Free(buf);

        if (i < npoints - 1) {
            data->buffer[data->cursor++] = ' ';
        }
    }

    if (!data->prefix) {
        len = strlen(command);
        if (to_string_expand_buffer(data, len)) {
            return -1;
        }
        strncpy(data->buffer + data->cursor, command,
                data->buffer_size - data->cursor);
        data->cursor += len;
    }

    return 0;
}


static int
Py_Outline_to_string_moveto_func(const FT_Vector *to, void *user)
{
    DecomposeToStringData *data = (DecomposeToStringData *)user;
    FT_Pos p[2];

    p[0] = to->x;
    p[1] = to->y;

    if (append_command_string(data, p, 2, data->move_command)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_to_string_lineto_func(const FT_Vector *to, void *user)
{
    DecomposeToStringData *data = (DecomposeToStringData *)user;
    FT_Pos p[2];

    p[0] = to->x;
    p[1] = to->y;

    if (append_command_string(data, p, 2, data->line_command)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_to_string_conicto_func(const FT_Vector *control, const FT_Vector *to, void *user)
{
    DecomposeToStringData *data = (DecomposeToStringData *)user;
    FT_Pos p[6];

    if (data->conic_command) {
        p[0] = control->x;
        p[1] = control->y;
        p[2] = to->x;
        p[3] = to->y;

        if (append_command_string(data, p, 4, data->conic_command)) {
            return 0x6;
        }
    } else {
        conic_to_cubic(
            data->last_x, data->last_y, control->x, control->y, to->x, to->y,
            &p[0], &p[1], &p[2], &p[3], &p[4], &p[5]);
        if (append_command_string(data, p, 6, data->cubic_command)) {
            return 0x6;
        }
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_to_string_cubicto_func(
    const FT_Vector *control1, const FT_Vector *control2,
    const FT_Vector *to, void *user)
{
    DecomposeToStringData *data = (DecomposeToStringData *)user;
    FT_Pos p[6];

    p[0] = control1->x;
    p[1] = control1->y;
    p[2] = control2->x;
    p[3] = control2->y;
    p[4] = to->x;
    p[5] = to->y;

    if (append_command_string(data, p, 6, data->cubic_command)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}


typedef struct {
    double last_x;
    double last_y;
    double *points;
    char *codes;
    size_t buffer_size;
    size_t cursor;
} DecomposeToPointsAndCodesData;


static int
to_points_and_codes_expand_buffer(
    DecomposeToPointsAndCodesData *data, size_t len)
{
    size_t new_len = len + data->cursor;

    if (new_len > data->buffer_size) {
        data->buffer_size = (((new_len / BUFFER_CHUNK_SIZE) + 1) *
                             BUFFER_CHUNK_SIZE);
        data->points = PyMem_Realloc(
            data->points, data->buffer_size * sizeof(double) * 2);
        if (data->points == NULL) {
            return -1;
        }
        data->codes = PyMem_Realloc(
            data->codes, data->buffer_size);
        if (data->codes == NULL) {
            return -1;
        }
    }

    return 0;
}


static int
append_points_and_codes(
    DecomposeToPointsAndCodesData *data, const FT_Vector *points,
    size_t npoints, char code)
{
    size_t i;

    if (to_points_and_codes_expand_buffer(data, npoints)) {
        return -1;
    }

    for (i = 0; i < npoints; ++i, ++data->cursor) {
        data->points[data->cursor * 2] = FROM_F26DOT6(points[i].x);
        data->points[data->cursor * 2 + 1] = FROM_F26DOT6(points[i].y);
        data->codes[data->cursor] = code;
    }

    return 0;
}


static int
Py_Outline_to_points_and_codes_moveto_func(const FT_Vector *to, void *user)
{
    DecomposeToPointsAndCodesData *data = (DecomposeToPointsAndCodesData *)user;

    if (append_points_and_codes(data, to, 1, CODE_MOVETO)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_to_points_and_codes_lineto_func(const FT_Vector *to, void *user)
{
    DecomposeToPointsAndCodesData *data = (DecomposeToPointsAndCodesData *)user;

    if (append_points_and_codes(data, to, 1, CODE_LINETO)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_to_points_and_codes_conicto_func(const FT_Vector *control, const FT_Vector *to, void *user)
{
    DecomposeToPointsAndCodesData *data = (DecomposeToPointsAndCodesData *)user;
    FT_Vector v[2];

    v[0] = *control;
    v[1] = *to;

    if (append_points_and_codes(data, v, 2, CODE_CONIC)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}

static int
Py_Outline_to_points_and_codes_cubicto_func(
    const FT_Vector *control1, const FT_Vector *control2,
    const FT_Vector *to, void *user)
{
    DecomposeToPointsAndCodesData *data = (DecomposeToPointsAndCodesData *)user;
    FT_Vector v[3];

    v[0] = *control1;
    v[1] = *control2;
    v[2] = *to;

    if (append_points_and_codes(data, v, 3, CODE_CUBIC)) {
        return 0x6;
    }

    data->last_x = to->x;
    data->last_y = to->y;

    return 0;
}


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Outline x;
    int inited;
    double *points;
    char *codes;
    size_t n_points;
} Py_Outline;


static PyTypeObject Py_Outline_Type;
static PyTypeObject Py_Outline_Points_Buffer_Type;
static PyTypeObject Py_Outline_Tags_Buffer_Type;
static PyTypeObject Py_Outline_Contours_Buffer_Type;
static PyTypeObject Py_Outline_Decomposed_Points_Buffer_Type;
static PyTypeObject Py_Outline_Codes_Buffer_Type;


static PyObject *Py_Outline_Points_Buffer_cnew(PyObject *owner);
static PyObject *Py_Outline_Tags_Buffer_cnew(PyObject *owner);
static PyObject *Py_Outline_Contours_Buffer_cnew(PyObject *owner);
static PyObject *Py_Outline_Decomposed_Points_Buffer_cnew(PyObject *owner);
static PyObject *Py_Outline_Codes_Buffer_cnew(PyObject *owner);


static void
Py_Outline_dealloc(Py_Outline* self)
{
    if (self->inited) {
        FT_Outline_Done(get_ft_library(), &self->x);
    }
    PyMem_Free(self->points);
    PyMem_Free(self->codes);
    Py_TYPE(self)->tp_clear((PyObject*)self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


PyObject *
Py_Outline_cnew(FT_Outline *outline)
{
    Py_Outline *self;

    self = (Py_Outline *)(&Py_Outline_Type)->tp_alloc(&Py_Outline_Type, 0);
    if (self == NULL) {
        return NULL;
    }

    self->points = NULL;
    self->codes = NULL;
    self->inited = 0;

    if (ftpy_exc(
            FT_Outline_New(get_ft_library(),
                           outline->n_points,
                           outline->n_contours,
                           &self->x))) {
        Py_DECREF(self);
        return NULL;
    }

    self->inited = 1;

    if (ftpy_exc(
            FT_Outline_Copy(outline, &self->x))) {
        Py_DECREF(self);
        return NULL;
    }

    self->base.owner = NULL;
    return (PyObject *)self;
}


static PyObject *
Py_Outline_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Outline *self;
    self = (Py_Outline *)type->tp_alloc(type, 0);
    self->inited = 0;
    self->points = 0;
    self->codes = 0;
    return (PyObject *)self;
}


static int
Py_Outline_init(Py_Outline *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Outline objects may not be instantiated directly. "
        "Use Face.glyph.outline instead.");

    return -1;
}


/****************************************************************************
 Getters
*/


MAKE_OUTLINE_GETTER(n_contours, PyLong_FromLong, self->x.n_contours)
MAKE_OUTLINE_GETTER(n_points, PyLong_FromLong, self->x.n_points)
MAKE_OUTLINE_GETTER(points, Py_Outline_Points_Buffer_cnew, (PyObject *)self)
MAKE_OUTLINE_GETTER(tags, Py_Outline_Tags_Buffer_cnew, (PyObject *)self)
MAKE_OUTLINE_GETTER(contours, Py_Outline_Contours_Buffer_cnew, (PyObject *)self)


static PyObject *flags_get(Py_Outline *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_FT_OUTLINE_BitflagType, self->x.flags);
}


static PyGetSetDef Py_Outline_getset[] = {
    DEF_OUTLINE_GETTER(n_contours),
    DEF_OUTLINE_GETTER(n_points),
    DEF_OUTLINE_GETTER(points),
    DEF_OUTLINE_GETTER(tags),
    DEF_OUTLINE_GETTER(contours),
    DEF_OUTLINE_GETTER(flags),
    {NULL}
};


/****************************************************************************
 Methods
*/

static PyObject*
Py_Outline_check(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    if (ftpy_exc(
            FT_Outline_Check(&self->x))) {
        return NULL;
    }

    Py_RETURN_NONE;
};


static PyObject*
Py_Outline_decompose(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    /* TODO: Also implement this as an iterator */

    DecomposeData data;
    PyObject *obj;
    const FT_Outline_Funcs funcs = {
        .move_to = Py_Outline_moveto_func,
        .line_to = Py_Outline_lineto_func,
        .conic_to = Py_Outline_conicto_func,
        .cubic_to = Py_Outline_cubicto_func,

        .shift = 0,
        .delta = 0
    };
    int error;

    const char* keywords[] = {"obj", "shift", "delta", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "O|ii:decompose", (char **)keywords,
            &obj, &funcs.shift, &funcs.delta)) {
        return NULL;
    }

    if (!PyObject_HasAttrString(obj, "move_to")) {
        PyErr_SetString(PyExc_AttributeError, "obj has no move_to method");
        return NULL;
    }
    if (!PyObject_HasAttrString(obj, "line_to")) {
        PyErr_SetString(PyExc_AttributeError, "obj has no line_to method");
        return NULL;
    }
    if (!PyObject_HasAttrString(obj, "cubic_to")) {
        PyErr_SetString(PyExc_AttributeError, "obj has no cubic_to method");
        return NULL;
    }
    data.has_conic_to = PyObject_HasAttrString(obj, "conic_to");
    data.callback = obj;
    data.last_x = 0;
    data.last_y = 0;

    error = FT_Outline_Decompose(&self->x, &funcs, &data);
    if (PyErr_Occurred()) {
        return NULL;
    } else if (ftpy_exc(error)) {
        return NULL;
    }

    Py_RETURN_NONE;
};


static PyObject*
Py_Outline_embolden(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    double strength;

    const char* keywords[] = {"strength", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "d:embolden", (char **)keywords,
            &strength)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Outline_Embolden(&self->x, TO_F26DOT6(strength)))) {
        return NULL;
    }

    Py_RETURN_NONE;
};


static PyObject*
Py_Outline_get_bbox(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    FT_BBox bbox;

    if (ftpy_exc(
            FT_Outline_Get_BBox(&self->x, &bbox))) {
        return NULL;
    }

    return Py_BBox_cnew(&bbox, 1.0);
}


static PyObject*
Py_Outline_get_cbox(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    FT_BBox bbox;

    FT_Outline_Get_CBox(&self->x, &bbox);

    return Py_BBox_cnew(&bbox, 1.0);
}


static PyObject*
Py_Outline_get_orientation(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    int orientation;

    orientation = FT_Outline_Get_Orientation(&self->x);

    return Py_Constant_cnew(&Py_FT_ORIENTATION_ConstantType, orientation);
}


static PyObject*
Py_Outline_reverse(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    FT_Outline_Reverse(&self->x);

    Py_RETURN_NONE;
};

static PyObject*
Py_Outline_transform(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    double xx, xy, yx, yy;
    FT_Matrix matrix;

    const char* keywords[] = {"xOffset", "yOffset", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "((dd)(dd)):transform", (char **)keywords,
            &xx, &xy, &yx, &yy)) {
        return NULL;
    }

    matrix.xx = TO_FT_FIXED(xx);
    matrix.xy = TO_FT_FIXED(xy);
    matrix.yx = TO_FT_FIXED(yx);
    matrix.yy = TO_FT_FIXED(yy);

    FT_Outline_Transform(&self->x, &matrix);

    Py_RETURN_NONE;
};


static PyObject*
Py_Outline_to_points_and_codes(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    PyObject *result = NULL;
    PyObject *points = NULL;
    PyObject *codes = NULL;
    DecomposeToPointsAndCodesData data;
    const FT_Outline_Funcs funcs = {
        .move_to = Py_Outline_to_points_and_codes_moveto_func,
        .line_to = Py_Outline_to_points_and_codes_lineto_func,
        .conic_to = Py_Outline_to_points_and_codes_conicto_func,
        .cubic_to = Py_Outline_to_points_and_codes_cubicto_func,

        .shift = 0,
        .delta = 0
    };
    int error;

    if (!self->points || !self->codes) {
        memset(&data, 0, sizeof(DecomposeToPointsAndCodesData));

        error = FT_Outline_Decompose(&self->x, &funcs, &data);
        if (PyErr_Occurred()) {
            PyMem_Free(data.points);
            PyMem_Free(data.codes);
            goto exit;
        } else if (ftpy_exc(error)) {
            PyMem_Free(data.points);
            PyMem_Free(data.codes);
            goto exit;
        }

        self->points = data.points;
        self->codes = data.codes;
        self->n_points = data.cursor;
    }

    points = Py_Outline_Decomposed_Points_Buffer_cnew((PyObject *)self);
    if (points == NULL) {
        goto exit;
    }

    codes = Py_Outline_Codes_Buffer_cnew((PyObject *)self);
    if (codes == NULL) {
        Py_DECREF(points);
        goto exit;
    }

    result = Py_BuildValue("(OO)", points, codes);
    if (result == NULL) {
        Py_DECREF(points);
        Py_DECREF(codes);
        goto exit;
    }

 exit:

    return result;
}


static PyObject*
Py_Outline_to_string(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    PyObject *result = NULL;
    DecomposeToStringData data;
    const FT_Outline_Funcs funcs = {
        .move_to = Py_Outline_to_string_moveto_func,
        .line_to = Py_Outline_to_string_lineto_func,
        .conic_to = Py_Outline_to_string_conicto_func,
        .cubic_to = Py_Outline_to_string_cubicto_func,

        .shift = 0,
        .delta = 0
    };
    int error;

    const char* keywords[] = {
        "move_command", "line_command", "cubic_command", "conic_command",
        "prefix", NULL};

    data.prefix = 0;
    data.conic_command = NULL;

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "sss|si:to_string", (char **)keywords,
            &data.move_command, &data.line_command,
            &data.cubic_command, &data.conic_command,
            &data.prefix)) {
        return NULL;
    }

    data.buffer = PyMem_Malloc(BUFFER_CHUNK_SIZE);
    if (data.buffer == NULL) {
        return NULL;
    }
    data.buffer[0] = 0;
    data.buffer_size = BUFFER_CHUNK_SIZE;
    data.cursor = 0;
    data.last_x = 0;
    data.last_y = 0;

    error = FT_Outline_Decompose(&self->x, &funcs, &data);
    if (PyErr_Occurred()) {
        goto exit;
    } else if (ftpy_exc(error)) {
        goto exit;
    }

    result = PyBytes_FromStringAndSize(data.buffer, data.cursor);
    if (result == NULL) {
        goto exit;
    }

 exit:
    PyMem_Free(data.buffer);

    return result;
}


static PyObject*
Py_Outline_translate(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    long xOffset;
    long yOffset;

    /* TODO: What is the scale of these arguments? */

    const char* keywords[] = {"x_offset", "y_offset", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "ll:translate", (char **)keywords,
            &xOffset, &yOffset)) {
        return NULL;
    }

    FT_Outline_Translate(&self->x, xOffset, yOffset);

    Py_RETURN_NONE;
};


/* TODO: FT_Outline_Get_Bitmap -- though that doesn't seem very useful */
/* TODO: FT_Outline_Render -- though that doesn't seem very useful */

static PyMethodDef Py_Outline_methods[] = {
    OUTLINE_METHOD_NOARGS(check),
    OUTLINE_METHOD(decompose),
    OUTLINE_METHOD(embolden),
    OUTLINE_METHOD_NOARGS(get_bbox),
    OUTLINE_METHOD_NOARGS(get_cbox),
    OUTLINE_METHOD_NOARGS(get_orientation),
    OUTLINE_METHOD_NOARGS(reverse),
    OUTLINE_METHOD(to_string),
    OUTLINE_METHOD_NOARGS(to_points_and_codes),
    OUTLINE_METHOD(transform),
    OUTLINE_METHOD(translate),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Ancillary buffers
*/


static PyObject *
Py_Outline_Points_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Outline_Points_Buffer_Type)->tp_alloc(
        &Py_Outline_Points_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Outline_Points_Buffer_get_buffer(ftpy_Buffer *self, Py_buffer *view, int flags)
{
    FT_Outline *outline = &((Py_Outline *)self->base.owner)->x;
    size_t itemsize = sizeof(FT_Pos);

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = outline->points;
    view->readonly = 1;
    view->itemsize = itemsize;
    if (itemsize == 4) {
        view->format = "i";
    } else {
        view->format = "q";
    }
    view->len = outline->n_points * 2 * itemsize;
    view->internal = NULL;
    view->ndim = 2;
    view->shape = self->shape;
    self->shape[0] = outline->n_points;
    self->shape[1] = 2;
    view->strides = self->strides;
    self->strides[0] = itemsize * 2;
    self->strides[1] = itemsize;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Outline_Points_Buffer_procs;


static PyObject *
Py_Outline_Tags_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Outline_Tags_Buffer_Type)->tp_alloc(
        &Py_Outline_Tags_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Outline_Tags_Buffer_get_buffer(ftpy_Buffer *self, Py_buffer *view, int flags)
{
    FT_Outline *outline = &((Py_Outline *)self->base.owner)->x;

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = outline->tags;
    view->readonly = 1;
    view->itemsize = 1;
    view->format = "B";
    view->len = outline->n_points;
    view->internal = NULL;
    view->ndim = 1;
    view->shape = self->shape;
    self->shape[0] = outline->n_points;
    view->strides = self->strides;
    self->strides[0] = 1;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Outline_Tags_Buffer_procs;


static PyObject *
Py_Outline_Contours_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Outline_Contours_Buffer_Type)->tp_alloc(
        &Py_Outline_Contours_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Outline_Contours_Buffer_get_buffer(
    ftpy_Buffer *self, Py_buffer *view, int flags)
{
    FT_Outline *outline = &((Py_Outline *)self->base.owner)->x;

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = outline->contours;
    view->readonly = 1;
    view->itemsize = 2;
    view->format = "H";
    view->len = outline->n_contours * 2;
    view->internal = NULL;
    view->ndim = 1;
    view->shape = self->shape;
    self->shape[0] = outline->n_contours;
    view->strides = self->strides;
    self->strides[0] = 2;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Outline_Contours_Buffer_procs;


static PyObject *
Py_Outline_Decomposed_Points_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Outline_Decomposed_Points_Buffer_Type)->tp_alloc(
        &Py_Outline_Decomposed_Points_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Outline_Decomposed_Points_Buffer_get_buffer(ftpy_Buffer *self, Py_buffer *view, int flags)
{
    Py_Outline *outline = (Py_Outline *)self->base.owner;
    size_t itemsize = sizeof(double);

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = outline->points;
    view->readonly = 1;
    view->itemsize = itemsize;
    view->format = "d";
    view->len = outline->n_points * 2 * itemsize;
    view->internal = NULL;
    view->ndim = 2;
    view->shape = self->shape;
    self->shape[0] = outline->n_points;
    self->shape[1] = 2;
    view->strides = self->strides;
    self->strides[0] = itemsize * 2;
    self->strides[1] = itemsize;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Outline_Decomposed_Points_Buffer_procs;


static PyObject *
Py_Outline_Codes_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Outline_Codes_Buffer_Type)->tp_alloc(
        &Py_Outline_Codes_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Outline_Codes_Buffer_get_buffer(ftpy_Buffer *self, Py_buffer *view, int flags)
{
    Py_Outline *outline = (Py_Outline *)self->base.owner;
    size_t itemsize = sizeof(char);

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = outline->codes;
    view->readonly = 1;
    view->itemsize = itemsize;
    view->format = "B";
    view->len = outline->n_points * itemsize;
    view->internal = NULL;
    view->ndim = 1;
    view->shape = self->shape;
    self->shape[0] = outline->n_points;
    view->strides = self->strides;
    self->strides[0] = itemsize;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Outline_Codes_Buffer_procs;


/****************************************************************************
 Setup
*/


static PyTypeObject Py_FT_OUTLINE_Type;
#define OUTLINE_CONST(name) DEF_CONST(name, FT_OUTLINE)
static constant_def FT_OUTLINE_constants[] = {
    OUTLINE_CONST(NONE),
    OUTLINE_CONST(OWNER),
    OUTLINE_CONST(EVEN_ODD_FILL),
    OUTLINE_CONST(REVERSE_FILL),
    OUTLINE_CONST(IGNORE_DROPOUTS),
    OUTLINE_CONST(SMART_DROPOUTS),
    OUTLINE_CONST(INCLUDE_STUBS),
    OUTLINE_CONST(HIGH_PRECISION),
    OUTLINE_CONST(SINGLE_PASS),
    {NULL}
};


static PyTypeObject Py_FT_ORIENTATION_Type;
#define ORIENTATION_CONST(name) DEF_CONST(name, FT_ORIENTATION)
static constant_def FT_ORIENTATION_constants[] = {
    ORIENTATION_CONST(TRUETYPE),
    ORIENTATION_CONST(POSTSCRIPT),
    ORIENTATION_CONST(FILL_RIGHT),
    ORIENTATION_CONST(FILL_LEFT),
    ORIENTATION_CONST(NONE),
    {NULL}
};


static PyTypeObject Py_FT_CODES_Type;
#define CODES_CONST(name) DEF_CONST(name, CODE)
static constant_def FT_CODES_constants[] = {
    CODES_CONST(STOP),
    CODES_CONST(MOVETO),
    CODES_CONST(LINETO),
    CODES_CONST(CONIC),
    CODES_CONST(CUBIC),
    {NULL}
};


int setup_Outline(PyObject *m)
{
    memset(&Py_Outline_Type, 0, sizeof(PyTypeObject));
    Py_Outline_Type = (PyTypeObject) {
        .tp_name = "freetypy.Outline",
        .tp_basicsize = sizeof(Py_Outline),
        .tp_dealloc = (destructor)Py_Outline_dealloc,
        .tp_doc = doc_Outline__init__,
        .tp_methods = Py_Outline_methods,
        .tp_getset = Py_Outline_getset,
        .tp_init = (initproc)Py_Outline_init,
        .tp_new = Py_Outline_new
    };

    ftpy_setup_type(m, &Py_Outline_Type);

    if (ftpy_setup_buffer_type(
            &Py_Outline_Points_Buffer_Type,
            "freetypy.Outline.PointsBuffer",
            doc_Outline_points,
            &Py_Outline_Points_Buffer_procs,
            (getbufferproc)Py_Outline_Points_Buffer_get_buffer)) {
        return -1;
    }

    if (ftpy_setup_buffer_type(
            &Py_Outline_Tags_Buffer_Type,
            "freetypy.Outline.TagsBuffer",
            doc_Outline_tags,
            &Py_Outline_Tags_Buffer_procs,
            (getbufferproc)Py_Outline_Tags_Buffer_get_buffer)) {
        return -1;
    }

    if (ftpy_setup_buffer_type(
            &Py_Outline_Contours_Buffer_Type,
            "freetypy.Outline.ContoursBuffer",
            doc_Outline_contours,
            &Py_Outline_Contours_Buffer_procs,
            (getbufferproc)Py_Outline_Contours_Buffer_get_buffer)) {
        return -1;
    }

    if (ftpy_setup_buffer_type(
            &Py_Outline_Decomposed_Points_Buffer_Type,
            "freetypy.Outline.DecomposedPointsBuffer",
            doc_Outline_decomposed_points,
            &Py_Outline_Decomposed_Points_Buffer_procs,
            (getbufferproc)Py_Outline_Decomposed_Points_Buffer_get_buffer)) {
        return -1;
    }

    if (ftpy_setup_buffer_type(
            &Py_Outline_Codes_Buffer_Type,
            "freetypy.Outline.CodesBuffer",
            doc_Outline_codes,
            &Py_Outline_Codes_Buffer_procs,
            (getbufferproc)Py_Outline_Codes_Buffer_get_buffer)) {
        return -1;
    }

    if (define_constant_namespace(
            m, &Py_FT_OUTLINE_Type, &Py_FT_OUTLINE_BitflagType,
            "freetypy.OUTLINE",
            doc_OUTLINE, FT_OUTLINE_constants) ||

        define_constant_namespace(
            m, &Py_FT_ORIENTATION_Type, &Py_FT_ORIENTATION_ConstantType,
            "freetypy.ORIENTATION",
            doc_ORIENTATION, FT_ORIENTATION_constants) ||

        define_constant_namespace(
            m, &Py_FT_CODES_Type, &Py_FT_CODES_ConstantType,
            "freetypy.CODES",
            doc_CODES, FT_CODES_constants)) {
        return -1;
    }

    return 0;
}
