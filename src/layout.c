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

#include "layout.h"
#include "doc/layout.h"

#include "bbox.h"
#include "face.h"


#define DEF_LAYOUT_GETTER(name) DEF_GETTER(name, doc_Layout_ ## name)



/****************************************************************************
 Object basics
*/


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

static PyObject *layout_get(Py_Layout *self, PyObject *closure)
{
    PyObject *result;
    PyObject *subresult;
    ftpy_Layout *layout;
    size_t i;

    layout = &self->x;

    result = PyList_New(layout->size);

    if (result == NULL) {
        return NULL;
    }

    for (i = 0; i < layout->size; ++i) {
        subresult = Py_BuildValue(
                "(Ok(dd))",
                self->base.owner,
                layout->glyph_indices[i],
                layout->xys[i].x,
                layout->xys[i].y);
        if (subresult == NULL) {
            Py_DECREF(result);
            return NULL;
        }
        if (PyList_SetItem(result, i, subresult)) {
            Py_DECREF(subresult);
            Py_DECREF(result);
            return NULL;
        }
    }

    return result;
}

static PyGetSetDef Py_Layout_getset[] = {
    DEF_LAYOUT_GETTER(ink_bbox),
    DEF_LAYOUT_GETTER(layout_bbox),
    DEF_LAYOUT_GETTER(layout),
    {NULL}
};


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

    return 0;
}
