/*
Copyright (c) 2013, Michael Droettboom
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



/****************************************************************************
 Outline decompose helper functions
*/


typedef struct {
    PyObject *callback;
    int has_conic_to;
    double last_x;
    double last_y;
} DecomposeData;


static int
Py_Outline_moveto_func(const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double x = to->x;
    double y = to->y;

    if (PyObject_CallMethod(obj, "move_to", "((dd))", x, y) == NULL) {
        return 0x6;
    }

    data->last_x = x;
    data->last_y = y;

    return 0;
}

static int
Py_Outline_lineto_func(const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double x = to->x;
    double y = to->y;

    if (PyObject_CallMethod(obj, "line_to", "((dd))", x, y) == NULL) {
        return 0x6;
    }

    data->last_x = x;
    data->last_y = y;

    return 0;
}

static int
Py_Outline_conicto_func(const FT_Vector *control, const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double xc = control->x;
    double yc = control->y;
    double x = to->x;
    double y = to->y;
    double xc0, yc0, xc1, yc1;

    if (data->has_conic_to) {
        if (PyObject_CallMethod(obj, "conic_to", "((dd)(dd))", xc, yc, x, y) == NULL) {
            return 0x6;
        }
    } else {
        xc0 = (2.0 * xc + data->last_x) / 3.0;
        yc0 = (2.0 * yc + data->last_y) / 3.0;
        xc1 = (x + 2.0 * xc) / 3.0;
        yc1 = (y + 2.0 * yc) / 3.0;
        if (PyObject_CallMethod(obj, "cubic_to", "((dd)(dd)(dd))",
                xc0, yc0, xc1, yc1, x, y) == NULL) {
            return 0x6;
        }
    }

    data->last_x = x;
    data->last_y = y;

    return 0;
}

static int
Py_Outline_cubicto_func(
    const FT_Vector *control1, const FT_Vector *control2,
    const FT_Vector *to, void *user)
{
    DecomposeData *data = (DecomposeData *)user;
    PyObject *obj = data->callback;
    double xc1 = control1->x;
    double yc1 = control1->y;
    double xc2 = control2->x;
    double yc2 = control2->y;
    double x = to->x;
    double y = to->y;

    if (PyObject_CallMethod(obj, "cubic_to", "((dd)(dd)(dd))",
            xc1, yc1, xc2, yc2, x, y) == NULL) {
        return 0x6;
    }

    data->last_x = x;
    data->last_y = y;

    return 0;
}


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Outline x;
} Py_Outline;


static PyTypeObject Py_Outline_Type;
static PyTypeObject Py_Outline_Points_Buffer_Type;
static PyTypeObject Py_Outline_Tags_Buffer_Type;
static PyTypeObject Py_Outline_Contours_Buffer_Type;


static PyObject *Py_Outline_Points_Buffer_cnew(PyObject *owner);
static PyObject *Py_Outline_Tags_Buffer_cnew(PyObject *owner);
static PyObject *Py_Outline_Contours_Buffer_cnew(PyObject *owner);


static void
Py_Outline_dealloc(Py_Outline* self)
{
    FT_Outline_Done(get_ft_library(), &self->x);
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

    if (ftpy_exc(
            FT_Outline_New(get_ft_library(),
                           outline->n_points,
                           outline->n_contours,
                           &self->x))) {
        Py_DECREF(self);
        return NULL;
    }

    if (ftpy_exc(
            FT_Outline_Copy(outline, &self->x))) {
        FT_Outline_Done(get_ft_library(), &self->x);
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
    return (PyObject *)self;
}


static int
Py_Outline_init(Py_Outline *self, PyObject *args, PyObject *kwds)
{
    /* TODO: Is there any usefulness to wrapping FT_Outline_New? */
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
    /* TODO: delta is in some sort of fixed value, not sure what */

    DecomposeData data;
    PyObject *obj;
    FT_Outline_Funcs funcs = {
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
    data.last_x = 0.0;
    data.last_y = 0.0;

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
Py_Outline_embolden_xy(Py_Outline* self, PyObject* args, PyObject* kwds) {
    double xstrength;
    double ystrength;

    const char* keywords[] = {"xstrength", "ystrength", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "d:embolden", (char **)keywords,
                &xstrength, &ystrength)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Outline_EmboldenXY(&self->x,
                TO_F26DOT6(xstrength), TO_F26DOT6(ystrength)))) {
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
Py_Outline_translate(Py_Outline* self, PyObject* args, PyObject* kwds)
{
    long xOffset;
    long yOffset;

    /* TODO: What is the scale of these arguments? */

    const char* keywords[] = {"xOffset", "yOffset", NULL};

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
    OUTLINE_METHOD(embolden_xy),
    OUTLINE_METHOD_NOARGS(get_bbox),
    OUTLINE_METHOD_NOARGS(get_cbox),
    OUTLINE_METHOD_NOARGS(get_orientation),
    OUTLINE_METHOD_NOARGS(reverse),
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

    if (define_constant_namespace(
            m, &Py_FT_OUTLINE_Type, &Py_FT_OUTLINE_BitflagType,
            "freetypy.OUTLINE",
            doc_OUTLINE, FT_OUTLINE_constants) ||

        define_constant_namespace(
            m, &Py_FT_ORIENTATION_Type, &Py_FT_ORIENTATION_ConstantType,
            "freetypy.ORIENTATION",
            doc_ORIENTATION, FT_ORIENTATION_constants)) {
        return -1;
    }

    return 0;
}
