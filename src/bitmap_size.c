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

#include "bitmap_size.h"
#include "doc/bitmap_size.h"


#define MAKE_BITMAP_SIZE_GETTER(name, convert_func, member)     \
    MAKE_GETTER(Py_Bitmap_Size, name, convert_func, member)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Bitmap_Size *x;
} Py_Bitmap_Size;


static PyTypeObject Py_Bitmap_Size_Type;


PyObject *
Py_Bitmap_Size_cnew(FT_Bitmap_Size *Bitmap_Size, PyObject *owner)
{
    Py_Bitmap_Size *self;
    self = (Py_Bitmap_Size *)(&Py_Bitmap_Size_Type)->tp_alloc(&Py_Bitmap_Size_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->x = Bitmap_Size;
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static PyObject *
Py_Bitmap_Size_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Bitmap_Size *self;

    self = (Py_Bitmap_Size *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_Bitmap_Size_init(Py_Bitmap_Size *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Bitmap_Size objects may not be instantiated directly. "
        "Use Face.available_sizes instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


MAKE_BITMAP_SIZE_GETTER(height, PyLong_FromLong, self->x->height);
MAKE_BITMAP_SIZE_GETTER(width, PyLong_FromLong, self->x->width);
MAKE_BITMAP_SIZE_GETTER(size, ftpy_PyFloat_FromF26DOT6, self->x->size);
MAKE_BITMAP_SIZE_GETTER(x_ppem, ftpy_PyFloat_FromF26DOT6, self->x->x_ppem);
MAKE_BITMAP_SIZE_GETTER(y_ppem, ftpy_PyFloat_FromF26DOT6, self->x->y_ppem);


static PyGetSetDef Py_Bitmap_Size_getset[] = {
    DEF_GETTER(height, doc_Bitmap_Size_height),
    DEF_GETTER(width, doc_Bitmap_Size_width),
    DEF_GETTER(size, doc_Bitmap_Size_size),
    DEF_GETTER(x_ppem, doc_Bitmap_Size_x_ppem),
    DEF_GETTER(y_ppem, doc_Bitmap_Size_y_ppem),
    {NULL}
};


/****************************************************************************
 Setup
*/


int setup_Bitmap_Size(PyObject *m)
{
    memset(&Py_Bitmap_Size_Type, 0, sizeof(PyTypeObject));
    Py_Bitmap_Size_Type = (PyTypeObject) {
        .tp_name = "freetypy.Bitmap_Size",
        .tp_basicsize = sizeof(Py_Bitmap_Size),
        .tp_doc = doc_Bitmap_Size__init__,
        .tp_getset = Py_Bitmap_Size_getset,
        .tp_init = (initproc)Py_Bitmap_Size_init,
        .tp_new = Py_Bitmap_Size_new
    };

    ftpy_setup_type(m, &Py_Bitmap_Size_Type);

    return 0;
}
