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

#include "bbox.h"
#include "doc/bbox.h"

#define BBOX_GETTER(name, type) GETTER(name, type, FT_BBoxRec, Py_BBox);
#define DEF_BBOX_GETTER(name) DEF_GETTER(name, doc_BBox_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    double xMin;
    double xMax;
    double yMin;
    double yMax;
} Py_BBox;


static PyTypeObject Py_BBox_Type;


PyObject *
Py_BBox_cnew(FT_BBox *bbox, double scale)
{
    Py_BBox *self;
    self = (Py_BBox *)(&Py_BBox_Type)->tp_alloc(&Py_BBox_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->base.owner = NULL;
    self->xMin = bbox->xMin * scale;
    self->xMax = bbox->xMax * scale;
    self->yMin = bbox->yMin * scale;
    self->yMax = bbox->yMax * scale;
    return (PyObject *)self;
}


static PyObject *
Py_BBox_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_BBox *self;

    self = (Py_BBox *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->xMin = 0;
    self->xMax = 0;
    self->yMin = 0;
    self->yMax = 0;
    return (PyObject *)self;
}


static int
Py_BBox_init(Py_BBox *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "BBox objects may not be instantiated directly. "
        "Use Face.bbox instead.");
    return -1;
}


/****************************************************************************
 Get/Set
*/


static PyObject *depth_get(Py_BBox *self, PyObject *closure)
{
    return PyFloat_FromDouble(self->yMin);
}


static PyObject *height_get(Py_BBox *self, PyObject *closure)
{
    return PyFloat_FromDouble(self->yMax);
}


static PyObject *width_get(Py_BBox *self, PyObject *closure)
{
    return PyFloat_FromDouble(self->xMax - self->xMin);
}


static PyGetSetDef Py_BBox_getset[] = {
    DEF_BBOX_GETTER(depth),
    DEF_BBOX_GETTER(height),
    DEF_BBOX_GETTER(width),
};


/****************************************************************************
 Members
*/


static PyMemberDef Py_BBox_members[] = {
    {"x_min", T_DOUBLE, offsetof(Py_BBox, xMin), READONLY, doc_BBox_x_min},
    {"x_max", T_DOUBLE, offsetof(Py_BBox, xMax), READONLY, doc_BBox_x_max},
    {"y_min", T_DOUBLE, offsetof(Py_BBox, yMin), READONLY, doc_BBox_y_min},
    {"y_max", T_DOUBLE, offsetof(Py_BBox, yMax), READONLY, doc_BBox_y_max},
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Sequence
*/


static Py_ssize_t Py_BBox_len(Py_BBox *self)
{
    return 4;
}


static PyObject *Py_BBox_getitem(Py_BBox *self, Py_ssize_t i)
{
    if (i == 0) {
        return PyFloat_FromDouble(self->xMin);
    } else if (i == 1) {
        return PyFloat_FromDouble(self->yMin);
    } else if (i == 2) {
        return PyFloat_FromDouble(self->xMax);
    } else if (i == 3) {
        return PyFloat_FromDouble(self->yMax);
    }

    PyErr_SetString(PyExc_IndexError, "index out of range");
    return NULL;
}


static PySequenceMethods Py_BBox_sequence;


/****************************************************************************
 repr
*/


static PyObject *Py_BBox_repr(PyObject *self)
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


static PyTypeObject Py_BBox_Type;


int setup_BBox(PyObject *m)
{
    memset(&Py_BBox_sequence, 0, sizeof(PySequenceMethods));
    Py_BBox_sequence = (PySequenceMethods) {
        .sq_length = (lenfunc)Py_BBox_len,
        .sq_item = (ssizeargfunc)Py_BBox_getitem
    };

    memset(&Py_BBox_Type, 0, sizeof(PyTypeObject));
    Py_BBox_Type = (PyTypeObject) {
        .tp_name = "freetypy.BBox",
        .tp_basicsize = sizeof(Py_BBox),
        .tp_as_sequence = &Py_BBox_sequence,
        .tp_repr = Py_BBox_repr,
        .tp_doc = doc_BBox__init__,
        .tp_members = Py_BBox_members,
        .tp_getset = Py_BBox_getset,
        .tp_init = (initproc)Py_BBox_init,
        .tp_new = Py_BBox_new
    };

    if (ftpy_setup_type(m, &Py_BBox_Type)) {
        return -1;
    }

    return 0;
}
