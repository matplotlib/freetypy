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

#include "vector.h"
#include "doc/vector.h"

#define VECTOR_GETTER(name, type) GETTER(name, type, FT_VectorRec, Py_Vector);

/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    double x;
    double y;
} Py_Vector;


static PyTypeObject Py_Vector_Type;


PyObject *
Py_Vector_cnew(FT_Vector *vector, double scale)
{
    Py_Vector *self;
    self = (Py_Vector *)(&Py_Vector_Type)->tp_alloc(&Py_Vector_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->base.owner = NULL;
    self->x = (double)vector->x / scale;
    self->y = (double)vector->y / scale;
    return (PyObject *)self;
}


static PyObject *
Py_Vector_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Vector *self;

    self = (Py_Vector *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = 0;
    self->y = 0;
    return (PyObject *)self;
}


static int
Py_Vector_init(Py_Vector *self, PyObject *args, PyObject *kwds)
{
    /* TODO: maybe make this work */

    PyErr_SetString(
        PyExc_RuntimeError,
        "Vector objects may not be instantiated directly.");
    return -1;
}


/****************************************************************************
 Members
*/


static PyMemberDef Py_Vector_members[] = {
    {"x", T_DOUBLE, offsetof(Py_Vector, x), READONLY, doc_Vector_x},
    {"y", T_DOUBLE, offsetof(Py_Vector, y), READONLY, doc_Vector_y},
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Sequence
*/


static Py_ssize_t Py_Vector_len(Py_Vector *self)
{
    return 2;
}


static PyObject *Py_Vector_getitem(Py_Vector *self, Py_ssize_t i)
{
    if (i == 0) {
        return PyFloat_FromDouble(self->x);
    } else if (i == 1) {
        return PyFloat_FromDouble(self->y);
    } else {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }
}


static PySequenceMethods Py_Vector_sequence;


/****************************************************************************
 Comparison
*/


static PyObject *Py_Vector_richcompare(PyObject *a, PyObject *b, int op)
{
    PyObject *tmp = NULL;
    PyObject *result = NULL;

    if (PySequence_Check(b)) {
        tmp = PySequence_Tuple(a);
        if (tmp == NULL) {
            goto exit;
        }
        result = PyObject_RichCompare(tmp, b, op);
    } else {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

 exit:
    Py_XDECREF(tmp);

    return result;
}


/****************************************************************************
 repr
*/


static PyObject *Py_Vector_repr(PyObject *self)
{
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    tuple = PySequence_Tuple(self);
    if (tuple == NULL) {
        goto exit;
    }

    result = PyObject_Repr(tuple);

 exit:
    Py_XDECREF(tuple);

    return result;
}


/****************************************************************************
 Setup
*/


static PyTypeObject Py_Vector_Type;


int setup_Vector(PyObject *m)
{
    memset(&Py_Vector_sequence, 0, sizeof(PySequenceMethods));
    Py_Vector_sequence = (PySequenceMethods) {
        .sq_length = (lenfunc)Py_Vector_len,
        .sq_item = (ssizeargfunc)Py_Vector_getitem
    };

    memset(&Py_Vector_Type, 0, sizeof(PyTypeObject));
    Py_Vector_Type = (PyTypeObject) {
        .tp_name = "freetypy.Vector",
        .tp_basicsize = sizeof(Py_Vector),
        .tp_as_sequence = &Py_Vector_sequence,
        .tp_repr = (unaryfunc)Py_Vector_repr,
        .tp_doc = doc_Vector__init__,
        .tp_members = Py_Vector_members,
        .tp_richcompare = Py_Vector_richcompare,
        .tp_init = (initproc)Py_Vector_init,
        .tp_new = Py_Vector_new
    };

    ftpy_setup_type(m, &Py_Vector_Type);

    return 0;
}
