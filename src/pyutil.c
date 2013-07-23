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

#include "pyutil.h"
#include "vector.h"

#include "datetime.h"


static PyObject *tt_long_datetime_epoch;


static const char *qualified_name_to_name(const char *qualified_name)
{
    const char *name = qualified_name;
    for (; *name != 0 && *name != '.'; ++name) {
        ;
    }
    if (*name == '.') {
        ++name;
    }

    return name;
}



static int
generic_traverse(ftpy_Object *self, visitproc visit, void *arg)
{
    int vret;

    if (self->owner) {
        vret = visit(self->owner, arg);
        if (vret != 0)
            return vret;
    }

    return 0;
}


static int
generic_clear(ftpy_Object *self)
{
    PyObject *tmp;

    tmp = self->owner;
    self->owner = NULL;
    Py_XDECREF(tmp);

    return 0;
}


void ftpy_Object_dealloc(PyObject* self)
{
    generic_clear((ftpy_Object *)self);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


PyObject *ftpy_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    ftpy_Object *self;

    self = (ftpy_Object *)type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->owner = NULL;
    return (PyObject *)self;
}


int ftpy_setup_type(PyObject *module, PyTypeObject *type)
{
    const char *name;

    if (type->tp_dealloc == NULL) {
        type->tp_dealloc = (destructor)ftpy_Object_dealloc;
    }

    if (type->tp_flags == 0) {
        type->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC;
    }

    if (type->tp_traverse == NULL) {
        type->tp_traverse = (traverseproc)generic_traverse;
    }

    if (type->tp_clear == NULL) {
        type->tp_clear = (inquiry)generic_clear;
    }

    if (type->tp_new == NULL) {
        type->tp_new = ftpy_Object_new;
    }

    if (PyType_Ready(type) < 0) {
        return -1;
    }

    Py_INCREF(type);

    if (module == NULL) {
        return 0;
    } else {
        name = qualified_name_to_name(type->tp_name);
        return PyModule_AddObject(module, name, (PyObject *)type);
    }
}


static PyObject *convert_char(unsigned char *p)
{
    return PyLong_FromUnsignedLong(*p);
}


static PyObject *convert_int(unsigned char *p)
{
    return PyLong_FromLong(*((int *)p));
}


static PyObject *convert_longlong(unsigned char *p)
{
    return PyLong_FromLongLong(*((long long *)p));
}


static PyObject *convert_unsignedshort(unsigned char *p)
{
    return PyLong_FromUnsignedLong(*((unsigned short *)p));
}


static PyObject *convert_double(unsigned char *p)
{
    return PyFloat_FromDouble(*((double *)p));
}


static PyObject *convert_unsignedlong(unsigned char *p)
{
    return PyLong_FromUnsignedLong(*((unsigned long *)p));
}


PyObject *ftpy_PyBuffer_ToList(PyObject *obj)
{
    Py_buffer view;
    PyObject *list[3];
    PyObject *val;
    PyObject *result = NULL;
    unsigned char *row;
    unsigned char *col;
    unsigned char *plane;
    Py_ssize_t i;
    Py_ssize_t j;
    Py_ssize_t k;
    PyObject * (* converter)(unsigned char *p) = NULL;

    list[0] = NULL;
    list[1] = NULL;
    list[2] = NULL;

    if (PyObject_GetBuffer(obj, &view, 0)) {
        return NULL;
    }

    switch (view.format[0]) {
    case 'B':
        converter = convert_char;
        break;

    case 'i':
        converter = convert_int;
        break;

    case 'q':
        converter = convert_longlong;
        break;

    case 'H':
        converter = convert_unsignedshort;
        break;

    case 'd':
        converter = convert_double;
        break;

    case 'L':
        converter = convert_unsignedlong;
        break;

    default:
        PyErr_Format(
            PyExc_ValueError, "Unknown buffer type '%s'", view.format);
        goto exit;
    }

    list[0] = PyList_New(view.shape[0]);
    if (list[0] == NULL) {
        goto exit;
    }

    if (view.ndim == 1) {
        row = view.buf;
        for (i = 0, col = row; i < view.shape[0]; ++i, col += view.strides[0]) {
            val = converter(col);
            if (val == NULL) {
                goto exit;
            }
            PyList_SET_ITEM(list[0], i, val);
        }
    } else if (view.ndim == 2) {
        row = view.buf;
        for (i = 0; i < view.shape[0]; ++i, row += view.strides[0]) {
            list[1] = PyList_New(view.shape[1]);
            if (list[1] == NULL) {
                goto exit;
            }
            for (j = 0, col = row; j < view.shape[1]; ++j, col += view.strides[1]) {
                val = converter(col);
                if (val == NULL) {
                    Py_DECREF(list[1]);
                    goto exit;
                }
                PyList_SET_ITEM(list[1], j, val);
            }
            PyList_SET_ITEM(list[0], i, list[1]);
        }
    } else if (view.ndim == 3) {
        row = view.buf;
        for (i = 0; i < view.shape[0]; ++i, row += view.strides[0]) {
            list[1] = PyList_New(view.shape[1]);
            if (list[1] == NULL) {
                goto exit;
            }
            for (j = 0, col = row; j < view.shape[1]; ++j, col += view.strides[1]) {
                list[2] = PyList_New(view.shape[2]);
                if (list[2] == NULL) {
                    goto exit;
                }
                for (k = 0, plane = col; k < view.shape[2]; ++k, plane += view.strides[2]) {
                    val = converter(plane);
                    if (val == NULL) {
                        Py_DECREF(list[1]);
                        Py_DECREF(list[2]);
                        goto exit;
                    }
                    PyList_SET_ITEM(list[2], k, val);
                }
                PyList_SET_ITEM(list[1], j, list[2]);
            }
            PyList_SET_ITEM(list[0], i, list[1]);
        }
    }

    result = list[0];
    Py_INCREF(result);

 exit:
    PyBuffer_Release(&view);
    Py_XDECREF(list[0]);

    return result;
}


static PyMethodDef Py_Buffer_methods[] = {
    {"to_list", (PyCFunction)ftpy_PyBuffer_ToList, METH_NOARGS, NULL},
    {NULL}  /* Sentinel */
};



int ftpy_setup_buffer_type(
    PyTypeObject *type,
    const char *name,
    const char *doc,
    PyBufferProcs *buffer_procs,
    getbufferproc get_buffer)
{
    memset(buffer_procs, 0, sizeof(PyBufferProcs));
    buffer_procs->bf_getbuffer = get_buffer;

    memset(type, 0, sizeof(PyTypeObject));
    *type = (PyTypeObject) {
        .tp_name = name,
        .tp_basicsize = sizeof(ftpy_Buffer),
        .tp_as_buffer = buffer_procs,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC
        #if !PY3K
        | Py_TPFLAGS_HAVE_NEWBUFFER
        #endif
        ,
        .tp_methods = Py_Buffer_methods,
        .tp_doc = doc,
    };

    ftpy_setup_type(NULL, type);

    return 0;
}


PyObject *ftpy_PyUnicode_FromStringOrNull(const char *val)
{
    if (val == NULL) {
        Py_RETURN_NONE;
    } else {
        return PyUnicode_FromString(val);
    }
}


PyObject *ftpy_PyDateTime_FromTTDateTime(long *date)
{
    PyObject *delta = NULL;
    PyObject *result = NULL;
    long long seconds;
    long long days;
    long long seconds_remain;

    seconds = (date[0] & 0xffffffff) << 32 | (date[1] & 0xffffffff);
    days = seconds / (60 * 60 * 24);
    seconds_remain = seconds % (60 * 60 * 24);

    delta = PyDelta_FromDSU(days, seconds_remain, 0);
    if (delta == NULL) {
        goto exit;
    }

    result = PyNumber_Add(tt_long_datetime_epoch, delta);
    if (result == NULL) {
        goto exit;
    } else {
        Py_INCREF(result);
    }

 exit:
    Py_XDECREF(delta);
    Py_XDECREF(result);

    return result;
}


int setup_pyutil(PyObject *m)
{
    PyDateTime_IMPORT;

    tt_long_datetime_epoch = PyDateTime_FromDateAndTime(1904, 1, 1, 0, 0, 0, 0);
    if (tt_long_datetime_epoch == NULL) {
        return -1;
    }

    return 0;
}
