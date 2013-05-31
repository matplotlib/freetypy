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

#include "matrix.h"
#include "doc/matrix.h"

#define MAKE_MATRIX_GETTER(name, convert_func, member)  \
    MAKE_GETTER(Py_Matrix, name, convert_func, member)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Matrix x;
} Py_Matrix;


static PyTypeObject Py_Matrix_Type;


PyObject *
Py_Matrix_cnew(FT_Matrix *matrix)
{
    Py_Matrix *self;
    self = (Py_Matrix *)(&Py_Matrix_Type)->tp_alloc(&Py_Matrix_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->base.owner = NULL;
    self->x = *matrix;
    return (PyObject *)self;
}


static int
Py_Matrix_init(Py_Matrix *self, PyObject *args, PyObject *kwds)
{
    /* TODO: maybe make this work */

    PyErr_SetString(
        PyExc_RuntimeError,
        "Matrix objects may not be instantiated directly.");
    return -1;
}


/****************************************************************************
 Getters
*/


MAKE_MATRIX_GETTER(xx, ftpy_PyFloat_FromFT_FIXED, self->x.xx);
MAKE_MATRIX_GETTER(xy, ftpy_PyFloat_FromFT_FIXED, self->x.xy);
MAKE_MATRIX_GETTER(yx, ftpy_PyFloat_FromFT_FIXED, self->x.yx);
MAKE_MATRIX_GETTER(yy, ftpy_PyFloat_FromFT_FIXED, self->x.yy);


static PyGetSetDef Py_Matrix_getset[] = {
    DEF_GETTER(xx, doc_Matrix_coefficient),
    DEF_GETTER(xy, doc_Matrix_coefficient),
    DEF_GETTER(yx, doc_Matrix_coefficient),
    DEF_GETTER(yy, doc_Matrix_coefficient),
    {NULL}
};


/****************************************************************************
 Sequence
*/


static Py_ssize_t Py_Matrix_len(Py_Matrix *self)
{
    return 4;
}


static PyObject *Py_Matrix_getitem(Py_Matrix *self, Py_ssize_t i)
{
    FT_Fixed *result;

    switch (i) {
    case 0:
        result = &self->x.xx;
        break;

    case 1:
        result = &self->x.xy;
        break;

    case 2:
        result = &self->x.yx;
        break;

    case 3:
        result = &self->x.yy;
        break;

    default:
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }

    return PyFloat_FromDouble(FROM_FT_FIXED(*result));
}


static PySequenceMethods Py_Matrix_sequence;


/****************************************************************************
 Comparison
*/


static PyObject *Py_Matrix_richcompare(PyObject *a, PyObject *b, int op)
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


static PyObject *Py_Matrix_repr(PyObject *self)
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


static PyTypeObject Py_Matrix_Type;


int setup_Matrix(PyObject *m)
{
    memset(&Py_Matrix_sequence, 0, sizeof(PySequenceMethods));
    Py_Matrix_sequence = (PySequenceMethods) {
        .sq_length = (lenfunc)Py_Matrix_len,
        .sq_item = (ssizeargfunc)Py_Matrix_getitem
    };

    memset(&Py_Matrix_Type, 0, sizeof(PyTypeObject));
    Py_Matrix_Type = (PyTypeObject) {
        .tp_name = "freetypy.Matrix",
        .tp_basicsize = sizeof(Py_Matrix),
        .tp_as_sequence = &Py_Matrix_sequence,
        .tp_repr = (unaryfunc)Py_Matrix_repr,
        .tp_doc = doc_Matrix__init__,
        .tp_getset = Py_Matrix_getset,
        .tp_richcompare = Py_Matrix_richcompare,
        .tp_init = (initproc)Py_Matrix_init,
    };

    ftpy_setup_type(m, &Py_Matrix_Type);

    return 0;
}
