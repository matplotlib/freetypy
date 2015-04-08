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

#include "layout.h"
#include "doc/layout.h"

#include "bbox.h"
#include "face.h"


#define DEF_LAYOUT_GETTER(name) DEF_GETTER(name, doc_Layout_ ## name)



/****************************************************************************
 Object basics
*/


static PyTypeObject Py_Layout_Points_Buffer_Type;
static PyTypeObject Py_Layout_Glyph_Indices_Buffer_Type;


static PyObject *Py_Layout_Points_Buffer_cnew(PyObject *owner);
static PyObject *Py_Layout_Glyph_Indices_Buffer_cnew(PyObject *owner);


static void
Py_Layout_dealloc(Py_Layout* self)
{
    free(self->x.glyph_indices);
    free(self->x.xys);
    Py_TYPE(self)->tp_clear((PyObject*)self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject *
Py_Layout_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Layout *self;

    self = (Py_Layout *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->base.owner = NULL;
    self->x.xys = NULL;
    self->x.glyph_indices = NULL;
    return (PyObject *)self;
}


static int
Py_Layout_init(Py_Layout *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"face", "text", "load_flags", NULL};
    PyObject *face_obj = NULL;
    Py_Face *face = NULL;
    PyObject *text_obj;
    int load_flags = FT_LOAD_DEFAULT;
    PyObject *text = NULL;
    PyObject *decoded_text = NULL;
    char *decoded_text_buf;
    Py_ssize_t decoded_text_size;
    int result = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!O|i:Layout.__init__", kwlist,
                                     &Py_Face_Type, &face_obj,
                                     &text_obj,
                                     &load_flags)) {
        goto exit;
    }

    face = (Py_Face *)face_obj;

    if (face->x->charmap == NULL ||
        face->x->charmap->encoding != FT_ENCODING_UNICODE) {
        PyErr_SetString(
            PyExc_ValueError, "The layout only supports Unicode character map");
        goto exit;
    }

    text = PyUnicode_FromObject(text_obj);
    if (text == NULL) {
        goto exit;
    }

    decoded_text = PyUnicode_AsUTF32String(text);
    if (decoded_text == NULL) {
        goto exit;
    }

    if (PyBytes_AsStringAndSize(decoded_text, &decoded_text_buf, &decoded_text_size)) {
        goto exit;
    }

    decoded_text_buf += 4;
    decoded_text_size = (decoded_text_size - 4) >> 2;

    if (ftpy_exc(ftpy_calculate_simple_layout(
        face->x, load_flags,
        (uint32_t *)decoded_text_buf, decoded_text_size, &self->x))) {
        goto exit;
    }

    Py_INCREF(face_obj);
    self->base.owner = face_obj;

    result = 0;

 exit:

    Py_XDECREF(text);
    Py_XDECREF(decoded_text);

    return result;
}


/****************************************************************************
 Getters
*/


static PyObject *ink_bbox_get(Py_Layout *self, PyObject *closure)
{
    return Py_BBox_cnew(&self->x.ink_bbox, 1.0 / (double)(1 << 6));
}

static PyObject *layout_bbox_get(Py_Layout *self, PyObject *closure)
{
    return Py_BBox_cnew(&self->x.layout_bbox, 1.0 / (double)(1 << 6));
}


static PyObject *glyph_indices_get(Py_Layout *self, PyObject *closure)
{
    return Py_Layout_Glyph_Indices_Buffer_cnew((PyObject *)self);
}


static PyObject *points_get(Py_Layout *self, PyObject *closure)
{
    return Py_Layout_Points_Buffer_cnew((PyObject *)self);
}


static PyGetSetDef Py_Layout_getset[] = {
    DEF_LAYOUT_GETTER(ink_bbox),
    DEF_LAYOUT_GETTER(layout_bbox),
    DEF_LAYOUT_GETTER(glyph_indices),
    DEF_LAYOUT_GETTER(points),
    {NULL}
};



/****************************************************************************
 Ancillary buffers
*/


static PyObject *
Py_Layout_Points_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Layout_Points_Buffer_Type)->tp_alloc(
        &Py_Layout_Points_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Layout_Points_Buffer_get_buffer(
    ftpy_Buffer *self, Py_buffer *view, int flags)
{
    ftpy_Layout *layout = &((Py_Layout *)self->base.owner)->x;
    size_t itemsize = sizeof(double);

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = layout->xys;
    view->readonly = 1;
    view->itemsize = itemsize;
    view->format = "d";
    view->len = layout->size * 2 * itemsize;
    view->internal = NULL;
    view->ndim = 2;
    view->shape = self->shape;
    self->shape[0] = layout->size;
    self->shape[1] = 2;
    view->strides = self->strides;
    self->strides[0] = itemsize * 2;
    self->strides[1] = itemsize;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Layout_Points_Buffer_procs;


static PyObject *
Py_Layout_Glyph_Indices_Buffer_cnew(PyObject *owner)
{
    ftpy_Buffer *self;
    self = (ftpy_Buffer *)(&Py_Layout_Glyph_Indices_Buffer_Type)->tp_alloc(
        &Py_Layout_Glyph_Indices_Buffer_Type, 0);
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static int Py_Layout_Glyph_Indices_Buffer_get_buffer(
    ftpy_Buffer *self, Py_buffer *view, int flags)
{
    ftpy_Layout *layout = &((Py_Layout *)self->base.owner)->x;
    size_t itemsize = sizeof(FT_ULong);

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = layout->glyph_indices;
    view->readonly = 1;
    view->itemsize = itemsize;
    view->format = "L";
    view->len = layout->size * itemsize;
    view->internal = NULL;
    view->ndim = 1;
    view->shape = self->shape;
    self->shape[0] = layout->size;
    view->strides = self->strides;
    self->strides[0] = itemsize;
    view->suboffsets = NULL;

    return 0;
}


static PyBufferProcs Py_Layout_Glyph_Indices_Buffer_procs;


/****************************************************************************
 Setup
*/


static PyTypeObject Py_Layout_Type;


int setup_Layout(PyObject *m)
{
    memset(&Py_Layout_Type, 0, sizeof(PyTypeObject));
    Py_Layout_Type = (PyTypeObject) {
        .tp_name = "freetypy.Layout",
        .tp_basicsize = sizeof(Py_Layout),
        .tp_dealloc = (destructor)Py_Layout_dealloc,
        .tp_doc = doc_Layout__init__,
        .tp_getset = Py_Layout_getset,
        // .tp_methods = Py_Layout_methods,
        .tp_init = (initproc)Py_Layout_init,
        .tp_new = Py_Layout_new
    };

    ftpy_setup_type(m, &Py_Layout_Type);

    if (ftpy_setup_buffer_type(
            &Py_Layout_Points_Buffer_Type,
            "freetypy.Layout.PointsBuffer",
            doc_Layout_points,
            &Py_Layout_Points_Buffer_procs,
            (getbufferproc)Py_Layout_Points_Buffer_get_buffer)) {
        return -1;
    }

    if (ftpy_setup_buffer_type(
            &Py_Layout_Glyph_Indices_Buffer_Type,
            "freetypy.Layout.Glyph_Indices_Buffer",
            doc_Layout_glyph_indices,
            &Py_Layout_Glyph_Indices_Buffer_procs,
            (getbufferproc)Py_Layout_Glyph_Indices_Buffer_get_buffer)) {
        return -1;
    }

    return 0;
}
