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

#include "size.h"
#include "doc/size.h"

#include "size_metrics.h"


#define SIZE_GETTER(name, type) GETTER(name, type, FT_Size, Py_Size);


/****************************************************************************
 Object basics
*/


static PyTypeObject Py_Size_Type;


PyObject *
Py_Size_cnew(FT_Size size, PyObject *owner)
{
    Py_Size *self;
    self = (Py_Size *)(&Py_Size_Type)->tp_alloc(&Py_Size_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->x = size;
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static PyObject *
Py_Size_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Size *self;

    self = (Py_Size *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_Size_init(Py_Size *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Size objects may not be instantiated directly. "
        "Use Face.size instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


static PyObject *face_get(Py_Size *self, PyObject *closure)
{
    Py_INCREF(self->base.owner);
    return self->base.owner;
}


static PyObject *metrics_get(Py_Size *self, PyObject *closure)
{
    return Py_Size_Metrics_cnew(&(self->x->metrics), (PyObject *)self);
}


static PyGetSetDef Py_Size_getset[] = {
    DEF_GETTER(face, doc_Size_face),
    DEF_GETTER(metrics, doc_Size_metrics),
    {NULL}
};


static PyTypeObject Py_Size_Type;


int setup_Size(PyObject *m)
{
    memset(&Py_Size_Type, 0, sizeof(PyTypeObject));
    Py_Size_Type = (PyTypeObject) {
        .tp_name = "freetypy.Size",
        .tp_basicsize = sizeof(Py_Size),
        .tp_doc = doc_Size__init__,
        .tp_getset = Py_Size_getset,
        .tp_init = (initproc)Py_Size_init,
        .tp_new = Py_Size_new
    };

    ftpy_setup_type(m, &Py_Size_Type);

    return 0;
}
