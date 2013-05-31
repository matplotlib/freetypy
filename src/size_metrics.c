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

#include "size_metrics.h"
#include "doc/size_metrics.h"

#define MAKE_SIZE_METRICS_GETTER(name, convert_func, member)    \
    MAKE_GETTER(Py_Size_Metrics, name, convert_func, member)
#define DEF_SIZE_METRICS_GETTER(name) DEF_GETTER(name, doc_Size_Metrics_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Size_Metrics *x;
} Py_Size_Metrics;


static PyTypeObject Py_Size_Metrics_Type;


PyObject *
Py_Size_Metrics_cnew(FT_Size_Metrics *size_metrics, PyObject *owner)
{
    Py_Size_Metrics *self;
    self = (Py_Size_Metrics *)(&Py_Size_Metrics_Type)->tp_alloc(&Py_Size_Metrics_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->x = size_metrics;
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static PyObject *
Py_Size_Metrics_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Size_Metrics *self;

    self = (Py_Size_Metrics *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_Size_Metrics_init(Py_Size_Metrics *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Size_Metrics objects may not be instantiated directly. "
        "Use Face.size.metrics instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


MAKE_SIZE_METRICS_GETTER(x_ppem, PyLong_FromLong, self->x->x_ppem)
MAKE_SIZE_METRICS_GETTER(y_ppem, PyLong_FromLong, self->x->y_ppem)
MAKE_SIZE_METRICS_GETTER(x_scale, ftpy_PyFloat_FromFT_FIXED, self->x->x_scale)
MAKE_SIZE_METRICS_GETTER(y_scale, ftpy_PyFloat_FromFT_FIXED, self->x->y_scale)
MAKE_SIZE_METRICS_GETTER(ascender, ftpy_PyFloat_FromF26DOT6, self->x->ascender)
MAKE_SIZE_METRICS_GETTER(descender, ftpy_PyFloat_FromF26DOT6, self->x->descender)
MAKE_SIZE_METRICS_GETTER(height, ftpy_PyFloat_FromF26DOT6, self->x->height)
MAKE_SIZE_METRICS_GETTER(max_advance, ftpy_PyFloat_FromF26DOT6, self->x->max_advance)


static PyGetSetDef Py_Size_Metrics_getset[] = {
    DEF_SIZE_METRICS_GETTER(x_ppem),
    DEF_SIZE_METRICS_GETTER(y_ppem),
    DEF_SIZE_METRICS_GETTER(x_scale),
    DEF_SIZE_METRICS_GETTER(y_scale),
    DEF_SIZE_METRICS_GETTER(ascender),
    DEF_SIZE_METRICS_GETTER(descender),
    DEF_SIZE_METRICS_GETTER(height),
    DEF_SIZE_METRICS_GETTER(max_advance),
    {NULL}
};


/****************************************************************************
 Setup
*/


int setup_Size_Metrics(PyObject *m)
{
    memset(&Py_Size_Metrics_Type, 0, sizeof(PyTypeObject));
    Py_Size_Metrics_Type = (PyTypeObject) {
        .tp_name = "freetypy.Size_Metrics",
        .tp_basicsize = sizeof(Py_Size_Metrics),
        .tp_doc = doc_Size_Metrics__init__,
        .tp_getset = Py_Size_Metrics_getset,
        .tp_init = (initproc)Py_Size_Metrics_init,
        .tp_new = Py_Size_Metrics_new
    };

    ftpy_setup_type(m, &Py_Size_Metrics_Type);

    return 0;
}
