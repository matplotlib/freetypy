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

#include "constants.h"
#include "truetype.h"


/****************************************************************************
 Integer constants
*/


PyObject *
Py_Constant_cnew(ftpy_ConstantType *type, unsigned long value)
{
    PyObject *self;
    PyObject *args;

    args = Py_BuildValue("(k)", value);
    if (args == NULL) {
        return NULL;
    }
    self = (PyLong_Type.tp_new)((PyTypeObject *)type, args, NULL);
    if (self == NULL) {
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(args);
    return (PyObject *)self;
}


static PyObject *
_Py_Bitflag_repr_get_parts(PyObject *self, PyObject *namespace)
{
    PyObject *dict = NULL;
    PyObject *parts = NULL;
    PyObject *bitwise_and = NULL;
    const char *namespace_name;
    PyObject *key = NULL;
    PyObject *value = NULL;
    PyObject *name = NULL;
    PyObject *result = NULL;
    Py_ssize_t i = 0;

    namespace_name = ((PyTypeObject *)namespace)->tp_name;

    if ((dict = PyObject_GetAttrString(namespace, "_inverse_map")) == NULL) {
        goto exit;
    }

    if ((parts = PyList_New(0)) == NULL) {
        goto exit;
    }

    while (PyDict_Next(dict, &i, &key, &value)) {
        if ((bitwise_and = PyNumber_And(self, key)) == NULL) {
            return NULL;
        }

        if (PyObject_IsTrue(bitwise_and)) {
            if ((name = PyUnicode_FromFormat("%s.%U", namespace_name, value)) == NULL) {
                goto exit;
            }
            if (PyList_Append(parts, name)) {
                goto exit;
            }
            Py_DECREF(name);
            name = NULL;
        }
        Py_DECREF(bitwise_and);
        bitwise_and = NULL;
    }

    Py_INCREF(parts);
    result = parts;

 exit:
    Py_XDECREF(dict);
    Py_XDECREF(bitwise_and);
    Py_XDECREF(name);
    Py_XDECREF(parts);

    return result;
}


static PyObject *
Py_Bitflag_repr(PyObject *self)
{
    PyObject *parts = NULL;
    PyObject *separator = NULL;
    PyObject *namespace = (PyObject *)((ftpy_ConstantType *)self->ob_type)->namespace;
    PyObject *result = NULL;

    parts = _Py_Bitflag_repr_get_parts(self, namespace);
    if (parts == NULL) {
        goto exit;
    }

    if (PyList_Size(parts) == 0) {
        result = PyUnicode_FromString("0");
    } else {
        if (PyList_Sort(parts)) {
            goto exit;
        }

        if ((separator = PyUnicode_FromString(" | ")) == NULL) {
            goto exit;
        }

        result = PyUnicode_Join(separator, parts);
    }

 exit:

    Py_XDECREF(parts);
    Py_XDECREF(separator);
    return result;
}


static PyObject *
Py_Constant_repr(PyObject *self)
{
    PyObject *dict = NULL;
    PyObject *name = NULL;
    PyObject *result = NULL;
    PyObject *namespace = (PyObject *)((ftpy_ConstantType *)self->ob_type)->namespace;
    const char *namespace_name;

    dict = PyObject_GetAttrString(namespace, "_inverse_map");
    if (dict == NULL) {
        goto exit;
    }

    name = PyDict_GetItem(dict, self);
    if (name == NULL) {
        result = PyObject_Str(self);
    } else {
        namespace_name = ((PyTypeObject *)namespace)->tp_name;
        result = PyUnicode_FromFormat("%s.%U", namespace_name, name);
    }

 exit:

    Py_XDECREF(dict);
    return result;
}


/****************************************************************************
 Constant namespaces
*/

static int make_namespace(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name,
    const char *doc,
    const constant_def *constants)
{
    const constant_def *p;
    PyObject *py_value;
    PyObject *inverse_dict;
    PyObject *py_name;

    memset(namespace_type, 0, sizeof(PyTypeObject));
    *namespace_type = (PyTypeObject) {
        .tp_name = qualified_name,
        .tp_basicsize = sizeof(PyObject),
        .tp_doc = doc,
    };

    ftpy_setup_type(m, namespace_type);

    inverse_dict = PyDict_New();
    if (inverse_dict == NULL) {
        return -1;
    }

    for (p = constants; p->name; ++p) {
        if (constant_type == NULL) {
            py_value = PyLong_FromLong(p->value);
        } else {
            py_value = Py_Constant_cnew(constant_type, p->value);
        }
        py_name = PyUnicode_FromString(p->name);
        PyDict_SetItem(namespace_type->tp_dict, py_name, py_value);
        PyDict_SetItem(inverse_dict, py_value, py_name);
        Py_DECREF(py_name);
        Py_DECREF(py_value);
    }

    PyDict_SetItemString(namespace_type->tp_dict, "_inverse_map", inverse_dict);
    Py_DECREF(inverse_dict);

    return 0;
}


static int make_constant_type(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name)
{
    PyTypeObject *py_constant_type = (PyTypeObject *)constant_type;
    char *buffer;
    buffer = PyMem_Malloc(80);
    if (buffer == NULL) {
        return -1;
    }

    PyOS_snprintf(buffer, 80, "%s.Constant", qualified_name);

    memset(constant_type, 0, sizeof(ftpy_ConstantType));
    *py_constant_type = (PyTypeObject) {
        .tp_name = buffer,
        .tp_repr = (unaryfunc)Py_Constant_repr,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_base = &PyLong_Type
    };

    constant_type->namespace = namespace_type;

    if (PyType_Ready(py_constant_type) < 0)
        return -1;

    return 0;
}


static int make_bitflag_type(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name)
{
    PyTypeObject *py_constant_type = (PyTypeObject *)constant_type;
    char *buffer;
    buffer = PyMem_Malloc(80);
    if (buffer == NULL) {
        return -1;
    }

    PyOS_snprintf(buffer, 80, "%s.Bitflag", qualified_name);

    memset(constant_type, 0, sizeof(ftpy_ConstantType));
    *py_constant_type = (PyTypeObject) {
        .tp_name = buffer,
        .tp_repr = (unaryfunc)Py_Bitflag_repr,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_base = &PyLong_Type
    };

    constant_type->namespace = namespace_type;

    if (PyType_Ready(py_constant_type) < 0)
        return -1;

    return 0;
}


int define_constant_namespace(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name,
    const char *doc,
    const constant_def *constants)
{
    if (make_constant_type(m, namespace_type, constant_type, qualified_name)) {
        return 1;
    }

    if (make_namespace(m, namespace_type, constant_type,
                       qualified_name, doc, constants)) {
        return 1;
    }

    return 0;
}


int define_bitflag_namespace(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name,
    const char *doc,
    const constant_def *constants)
{
    if (make_bitflag_type(m, namespace_type, constant_type, qualified_name)) {
        return 1;
    }

    if (make_namespace(m, namespace_type, constant_type,
                       qualified_name, doc, constants)) {
        return 1;
    }

    return 0;
}



/****************************************************************************
 Setup
*/


int setup_constants(PyObject *m)
{
    return 0;
}
