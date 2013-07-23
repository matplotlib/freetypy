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

#include "bitmap.h"
#include "doc/bitmap.h"

#include "constants.h"
#include "pyutil.h"

#include FT_BITMAP_H

#define MAKE_BITMAP_GETTER(name, convert_func, member)  \
    MAKE_GETTER(Py_Bitmap, name, convert_func, member);
#define BITMAP_METHOD(name) DEF_METHOD(name, Bitmap)
#define BITMAP_METHOD_NOARGS(name) DEF_METHOD_NOARGS(name, Bitmap)

/* TODO: Provide a way to get at the bitmap data without Numpy */

static ftpy_ConstantType Py_FT_PIXEL_MODE_ConstantType;


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Bitmap *x;
    Py_ssize_t shape[3];
    Py_ssize_t strides[3];
} Py_Bitmap;


static PyTypeObject Py_Bitmap_Type;


static void
Py_Bitmap_dealloc(Py_Bitmap* self)
{
    FT_Bitmap_Done(get_ft_library(), self->x);
    free(self->x);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


PyObject *
Py_Bitmap_cnew(FT_Bitmap *bitmap)
{
    Py_Bitmap *self;
    FT_Bitmap *copy;

    copy = PyMem_Malloc(sizeof(FT_Bitmap));
    if (copy == NULL) {
        return NULL;
    }
    memset(copy, 0, sizeof(FT_Bitmap));
    FT_Bitmap_New(copy);

    if (ftpy_exc(
            FT_Bitmap_Copy(get_ft_library(),
                           bitmap, copy))) {
        FT_Bitmap_Done(get_ft_library(), copy);
        free(copy);
        return NULL;
    }

    self = (Py_Bitmap *)(&Py_Bitmap_Type)->tp_alloc(&Py_Bitmap_Type, 0);
    self->base.owner = NULL;
    self->x = copy;
    return (PyObject *)self;
}


static PyObject *
Py_Bitmap_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Bitmap *self;
    self = (Py_Bitmap *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_Bitmap_init()
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Bitmap objects may not be instantiated directly. "
        "Use Face.glyph.bitmap instead.");

    return -1;
}


/****************************************************************************
 Getters
*/


MAKE_BITMAP_GETTER(rows, PyLong_FromLong, self->x->rows);
MAKE_BITMAP_GETTER(width, PyLong_FromLong, self->x->width);
MAKE_BITMAP_GETTER(pitch, PyLong_FromLong, self->x->pitch);
MAKE_BITMAP_GETTER(num_grays, PyLong_FromLong, self->x->num_grays);


static PyObject *pixel_mode_get(Py_Bitmap *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_FT_PIXEL_MODE_ConstantType, self->x->pixel_mode);
}


static PyObject *buffer_get(Py_Bitmap *self, PyObject *closure)
{
    return PyBytes_FromStringAndSize(
        (char *)self->x->buffer, self->x->rows * self->x->pitch);
}


static PyGetSetDef Py_Bitmap_getset[] = {
    DEF_GETTER(rows, doc_Bitmap_rows),
    DEF_GETTER(width, doc_Bitmap_width),
    DEF_GETTER(pitch, doc_Bitmap_pitch),
    DEF_GETTER(num_grays, doc_Bitmap_num_grays),
    DEF_GETTER(pixel_mode, doc_Bitmap_pixel_mode),
    DEF_GETTER(buffer, doc_Bitmap_buffer),
    {NULL}
};


/****************************************************************************
 Methods
*/


static PyObject*
Py_Bitmap_convert(Py_Bitmap* self, PyObject* args, PyObject* kwds) {
    int alignment = 1;
    FT_Bitmap *converted;
    Py_Bitmap *result;

    const char* keywords[] = {"alignment", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "|i:convert", (char **)keywords,
            &alignment)) {
        return NULL;
    }

    converted = PyMem_Malloc(sizeof(FT_Bitmap));
    if (converted == NULL) {
        return NULL;
    }
    memset(converted, 0, sizeof(FT_Bitmap));
    FT_Bitmap_New(converted);

    if (ftpy_exc(
            FT_Bitmap_Convert(get_ft_library(), self->x, converted,
                              alignment))) {
        return NULL;
    }

    result = (Py_Bitmap *)(&Py_Bitmap_Type)->tp_alloc(&Py_Bitmap_Type, 0);
    result->x = converted;
    return (PyObject *)result;
};


static PyObject*
Py_Bitmap_to_list(Py_Bitmap* self) {
    return ftpy_PyBuffer_ToList((PyObject *)self);
};


static PyMethodDef Py_Bitmap_methods[] = {
    BITMAP_METHOD(convert),
    BITMAP_METHOD_NOARGS(to_list),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Buffer interface
*/


static int Py_Bitmap_get_buffer(Py_Bitmap *self, Py_buffer *view, int flags)
{
    /*
      This is decremented automatically by the Python runtime on
      destruction of the memoryview.
    */
    switch(self->x->pixel_mode) {
    case FT_PIXEL_MODE_GRAY:
        view->ndim = 2;
        self->shape[0] = self->x->rows;
        self->shape[1] = self->x->width;
        self->strides[0] = self->x->pitch;
        self->strides[1] = 1;
        break;

    case FT_PIXEL_MODE_LCD:
        view->ndim = 3;
        self->shape[0] = self->x->rows;
        self->shape[1] = self->x->width / 3;
        self->shape[2] = 3;
        self->strides[0] = self->x->pitch;
        self->strides[1] = 3;
        self->strides[2] = 1;
        break;

    case FT_PIXEL_MODE_LCD_V:
        view->ndim = 3;
        self->shape[0] = self->x->rows / 3;
        self->shape[1] = self->x->width;
        self->shape[2] = 3;
        self->strides[0] = self->x->pitch * 3;
        self->strides[1] = 1;
        self->strides[2] = self->x->pitch;
        break;

    case FT_PIXEL_MODE_MONO:
        view->ndim = 2;
        self->shape[0] = self->x->rows;
        self->shape[1] = (self->x->width >> 3) + 1;
        self->strides[0] = self->x->pitch;
        self->strides[1] = 1;
        break;

    default:
        /*
          We don't support these formats, because, according to the
          freetype docs: "We haven't found a single font using this
          format"
        */
        PyErr_SetString(
            PyExc_NotImplementedError,
            "GRAY2 and GRAY4 pixel modes are not supported");
        return -1;
    }

    Py_INCREF(self);
    view->obj = (PyObject *)self;
    view->buf = self->x->buffer;
    view->readonly = 1;
    view->itemsize = 1;
    view->format = "B";
    view->len = self->x->rows * self->x->pitch;
    view->shape = self->shape;
    view->strides = self->strides;
    view->suboffsets = NULL;
    view->internal = NULL;

    return 0;
}


static PyBufferProcs Py_Bitmap_buffer_procs;


/****************************************************************************
 Setup
*/


static PyTypeObject Py_Bitmap_Type;


static PyTypeObject Py_FT_PIXEL_MODE_Type;
#define PIXEL_MODE_CONST(name) DEF_CONST(name, FT_PIXEL_MODE)
static constant_def FT_PIXEL_MODE_constants[] = {
    PIXEL_MODE_CONST(MONO),
    PIXEL_MODE_CONST(GRAY),
    PIXEL_MODE_CONST(GRAY2),
    PIXEL_MODE_CONST(GRAY4),
    PIXEL_MODE_CONST(LCD),
    PIXEL_MODE_CONST(LCD_V),
    {NULL}
};


int setup_Bitmap(PyObject *m)
{
    memset(&Py_Bitmap_buffer_procs, 0, sizeof(PyBufferProcs));
    Py_Bitmap_buffer_procs.bf_getbuffer = (getbufferproc)Py_Bitmap_get_buffer;

    memset(&Py_Bitmap_Type, 0, sizeof(PyTypeObject));
    Py_Bitmap_Type = (PyTypeObject) {
        .tp_name = "freetypy.Bitmap",
        .tp_basicsize = sizeof(Py_Bitmap),
        .tp_dealloc = (destructor)Py_Bitmap_dealloc,
        .tp_as_buffer = &Py_Bitmap_buffer_procs,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE
        #if !PY3K
        | Py_TPFLAGS_HAVE_NEWBUFFER
        #endif
        ,
        .tp_doc = doc_Bitmap__init__,
        .tp_methods = Py_Bitmap_methods,
        .tp_getset = Py_Bitmap_getset,
        .tp_init = (initproc)Py_Bitmap_init,
        .tp_new = Py_Bitmap_new
    };

    ftpy_setup_type(m, &Py_Bitmap_Type);

    if (define_constant_namespace(
            m, &Py_FT_PIXEL_MODE_Type, &Py_FT_PIXEL_MODE_ConstantType,
            "freetypy.PIXEL_MODE",
            doc_PIXEL_MODE, FT_PIXEL_MODE_constants)) {
        return -1;
    }

    return 0;
}
