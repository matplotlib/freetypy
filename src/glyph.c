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

#include "glyph.h"
#include "doc/glyph.h"

#include "bbox.h"
#include "bitmap.h"
#include "constants.h"
#include "glyph_metrics.h"
#include "outline.h"
#include "subglyphs.h"
#include "vector.h"


#define MAKE_GLYPH_GETTER(name, convert_func, member)   \
    MAKE_GETTER(Py_Glyph, name, convert_func, member)
#define DEF_GLYPH_GETTER(name) DEF_GETTER(name, doc_Glyph_ ## name)
#define GLYPH_METHOD(name) DEF_METHOD(name, Glyph)


/****************************************************************************
 Object basics
*/

static ftpy_ConstantType Py_FT_GLYPH_BBOX_ConstantType;
static ftpy_ConstantType Py_FT_GLYPH_FORMAT_ConstantType;
static ftpy_ConstantType Py_FT_RENDER_MODE_ConstantType;


static PyTypeObject Py_Glyph_Type;


static void
Py_Glyph_dealloc(Py_Glyph* self)
{
    Py_TYPE(self)->tp_clear((PyObject *)self);
    free(self->x);
    FT_Done_Glyph(self->glyph);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


PyObject *
Py_Glyph_cnew(FT_GlyphSlot glyph_slot, PyObject *owner, int load_flags)
{
    Py_Glyph *self;
    FT_GlyphSlot glyph_slot_copy;
    FT_Glyph glyph;

    if (ftpy_exc(FT_Get_Glyph(glyph_slot, &glyph))) {
        return NULL;
    }

    self = (Py_Glyph *)(&Py_Glyph_Type)->tp_alloc(&Py_Glyph_Type, 0);
    if (self == NULL) {
        return NULL;
    }

    glyph_slot_copy = PyMem_Malloc(sizeof(FT_GlyphSlotRec));
    if (glyph_slot_copy == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    memcpy(glyph_slot_copy, glyph_slot, sizeof(FT_GlyphSlotRec));
    self->x = glyph_slot_copy;
    self->glyph = glyph;
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static PyObject *
Py_Glyph_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Glyph *self;

    self = (Py_Glyph *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_Glyph_init(Py_Glyph *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Glyph objects may not be instantiated directly. "
        "Use Face.load_glyph and Face.load_char instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


static PyObject *face_get(Py_Glyph *self, PyObject *closure)
{
    Py_INCREF(self->base.owner);
    return self->base.owner;
}


static PyObject *metrics_get(Py_Glyph *self, PyObject *closure)
{
    return Py_Glyph_Metrics_cnew(&self->x->metrics, (PyObject *)self, self->load_flags);
}


static PyObject *linear_hori_advance_get(Py_Glyph *self, PyObject *closure)
{
    if (self->load_flags & FT_LOAD_LINEAR_DESIGN) {
        return PyInt_FromLong(self->x->linearHoriAdvance);
    } else {
        return ftpy_PyFloat_FromFT_FIXED(self->x->linearHoriAdvance);
    }
}


static PyObject *linear_vert_advance_get(Py_Glyph *self, PyObject *closure)
{
    if (self->load_flags & FT_LOAD_LINEAR_DESIGN) {
        return PyInt_FromLong(self->x->linearVertAdvance);
    } else {
        return ftpy_PyFloat_FromFT_FIXED(self->x->linearVertAdvance);
    }
}


MAKE_GLYPH_GETTER(advance, ftpy_Py_Vector_FromVector, &self->x->advance);


static PyObject *format_get(Py_Glyph *self, PyObject *closure)
{
    return Py_Constant_cnew(&Py_FT_GLYPH_FORMAT_ConstantType, self->x->format);
}


static PyObject *bitmap_get(Py_Glyph *self, PyObject *closure)
{
    if (self->glyph->format == FT_GLYPH_FORMAT_BITMAP) {
        return Py_Bitmap_cnew(
            &((FT_BitmapGlyph)self->glyph)->bitmap);
    } else {
        PyErr_SetString(PyExc_AttributeError, "Glyph has no bitmap");
        return NULL;
    }
}


MAKE_GLYPH_GETTER(bitmap_left, PyLong_FromLong, self->x->bitmap_left);
MAKE_GLYPH_GETTER(bitmap_top, PyLong_FromLong, self->x->bitmap_top);


static PyObject *outline_get(Py_Glyph *self, PyObject *closure)
{
    if (self->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
        return Py_Outline_cnew(&((FT_OutlineGlyph)self->glyph)->outline);
    } else {
        PyErr_SetString(PyExc_AttributeError, "Glyph has no outline");
        return NULL;
    }
}


MAKE_GLYPH_GETTER(subglyphs, Py_SubGlyphs_cnew, self);
MAKE_GLYPH_GETTER(lsb_delta, PyLong_FromLong, self->x->lsb_delta);
MAKE_GLYPH_GETTER(rsb_delta, PyLong_FromLong, self->x->rsb_delta);


static PyGetSetDef Py_Glyph_getset[] = {
    DEF_GLYPH_GETTER(face),
    DEF_GLYPH_GETTER(metrics),
    DEF_GLYPH_GETTER(linear_hori_advance),
    DEF_GLYPH_GETTER(linear_vert_advance),
    DEF_GLYPH_GETTER(advance),
    DEF_GLYPH_GETTER(format),
    DEF_GLYPH_GETTER(bitmap),
    DEF_GLYPH_GETTER(bitmap_left),
    DEF_GLYPH_GETTER(bitmap_top),
    DEF_GLYPH_GETTER(outline),
    DEF_GLYPH_GETTER(subglyphs),
    DEF_GLYPH_GETTER(lsb_delta),
    DEF_GLYPH_GETTER(rsb_delta),
    {NULL}
};


/****************************************************************************
 Methods
*/


static PyObject*
Py_Glyph_get_cbox(Py_Glyph* self, PyObject* args, PyObject* kwds) {
    unsigned int bbox_mode = FT_GLYPH_BBOX_UNSCALED;
    FT_BBox bbox;
    double scale = 1.0;

    const char* keywords[] = {"bbox_mode", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "|I:get_cbox", (char **)keywords,
            &bbox_mode)) {
        return NULL;
    }

    FT_Glyph_Get_CBox(self->glyph, bbox_mode, &bbox);

    if (bbox_mode == FT_GLYPH_BBOX_SUBPIXELS ||
        bbox_mode == FT_GLYPH_BBOX_GRIDFIT) {
        scale = 1.0 / 64.0;
    }

    return Py_BBox_cnew(&bbox, scale);
}


static PyObject*
Py_Glyph_render(Py_Glyph* self, PyObject* args, PyObject* kwds) {
    int render_mode = FT_RENDER_MODE_NORMAL;
    double x = 0;
    double y = 0;
    FT_Vector origin;

    const char* keywords[] = {"render_mode", "origin", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "|i(dd):render", (char **)keywords,
            &render_mode, &x, &y)) {
        return NULL;
    }

    origin.x = TO_F26DOT6(x);
    origin.y = TO_F26DOT6(y);

    if (ftpy_exc(
            FT_Glyph_To_Bitmap(&self->glyph, render_mode, &origin, 1))) {
        return NULL;
    }

    return Py_Bitmap_cnew(
        &((FT_BitmapGlyph)self->glyph)->bitmap);
}


static PyMethodDef Py_Glyph_methods[] = {
    GLYPH_METHOD(get_cbox),
    GLYPH_METHOD(render),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_Glyph_Type;


static PyTypeObject Py_FT_GLYPH_BBOX_Type;
#define GLYPH_BBOX_CONST(name) DEF_CONST(name, FT_GLYPH_BBOX)
static constant_def FT_GLYPH_BBOX_constants[] = {
    GLYPH_BBOX_CONST(UNSCALED),
    GLYPH_BBOX_CONST(SUBPIXELS),
    GLYPH_BBOX_CONST(GRIDFIT),
    GLYPH_BBOX_CONST(TRUNCATE),
    GLYPH_BBOX_CONST(PIXELS),
    {NULL}
};


static PyTypeObject Py_FT_GLYPH_FORMAT_Type;
#define GLYPH_FORMAT_CONST(name) DEF_CONST(name, FT_GLYPH_FORMAT)
static constant_def FT_GLYPH_FORMAT_constants[] = {
    GLYPH_FORMAT_CONST(NONE),
    GLYPH_FORMAT_CONST(COMPOSITE),
    GLYPH_FORMAT_CONST(BITMAP),
    GLYPH_FORMAT_CONST(OUTLINE),
    GLYPH_FORMAT_CONST(PLOTTER),
    {NULL}
};


static PyTypeObject Py_FT_RENDER_MODE_Type;
#define RENDER_MODE_CONST(name) DEF_CONST(name, FT_RENDER_MODE)
static constant_def FT_RENDER_MODE_constants[] = {
    RENDER_MODE_CONST(NORMAL),
    RENDER_MODE_CONST(LIGHT),
    RENDER_MODE_CONST(MONO),
    RENDER_MODE_CONST(LCD),
    RENDER_MODE_CONST(LCD_V),
    {NULL}
};


int setup_Glyph(PyObject *m)
{
    memset(&Py_Glyph_Type, 0, sizeof(PyTypeObject));
    Py_Glyph_Type = (PyTypeObject) {
        .tp_name = "freetypy.Glyph",
        .tp_basicsize = sizeof(Py_Glyph),
        .tp_dealloc = (destructor)Py_Glyph_dealloc,
        .tp_doc = doc_Glyph__init__,
        .tp_methods = Py_Glyph_methods,
        .tp_getset = Py_Glyph_getset,
        .tp_init = (initproc)Py_Glyph_init,
        .tp_new = Py_Glyph_new
    };

    ftpy_setup_type(m, &Py_Glyph_Type);

    if (define_constant_namespace(
            m, &Py_FT_GLYPH_BBOX_Type, &Py_FT_GLYPH_BBOX_ConstantType,
            "freetypy.GLYPH_BBOX",
            doc_GLYPH_BBOX, FT_GLYPH_BBOX_constants) ||

        define_constant_namespace(
            m, &Py_FT_GLYPH_FORMAT_Type, &Py_FT_GLYPH_FORMAT_ConstantType,
            "freetypy.GLYPH_FORMAT",
            doc_GLYPH_FORMAT, FT_GLYPH_FORMAT_constants) ||

        define_constant_namespace(
            m, &Py_FT_RENDER_MODE_Type, &Py_FT_RENDER_MODE_ConstantType,
            "freetypy.RENDER_MODE",
            doc_RENDER_MODE, FT_RENDER_MODE_constants)) {
        return -1;
    }

    return 0;
}
