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

#include "sfntnames.h"
#include "doc/sfntnames.h"

#include "encoding.h"
#include "sfntname.h"

#include FT_SFNT_NAMES_H


#define SFNTNAMES_GETTER(name, type) GETTER(name, type, FT_SfntNamesRec, Py_SfntNames);
#define SFNTNAMES_METHOD(name) DEF_METHOD(name, SfntNames)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
} Py_SfntNames;


static PyTypeObject Py_SfntNames_Type;


PyObject *
Py_SfntNames_cnew(Py_Face *face)
{
    Py_SfntNames *self;
    self = (Py_SfntNames *)(&Py_SfntNames_Type)->tp_alloc(&Py_SfntNames_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF((PyObject *)face);
    self->base.owner = (PyObject *)face;
    return (PyObject *)self;
}


static int
Py_SfntNames_init(Py_SfntNames *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "SfntNames objects may not be instantiated directly. "
        "Use Face.sfnt_names instead.");
    return -1;
}


/****************************************************************************
 Methods
*/


static PyObject*
Py_SfntNames_get_name(Py_SfntNames* self, PyObject* args, PyObject* kwds) {
    unsigned short name;
    unsigned int count;
    unsigned int i;
    FT_Face face = NULL;
    FT_SfntName sfnt_name;

    const char* keywords[] = {"name", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "H:get_name", (char **)keywords,
            &name)) {
        return NULL;
    }

    face = ((Py_Face *)self->base.owner)->x;

    count = FT_Get_Sfnt_Name_Count(face);

    for (i = 0; i < count; ++i) {
        if (ftpy_exc(
            FT_Get_Sfnt_Name(face, i, &sfnt_name))) {
            return NULL;
        }

        if (sfnt_name.name_id == name) {
            if (ftpy_is_unicode_encoding(
                        sfnt_name.platform_id, sfnt_name.encoding_id)) {
                return Py_SfntName_cnew((Py_Face *)self->base.owner, i);
            }
        }
    }

    for (i = 0; i < count; ++i) {
        if (ftpy_exc(
            FT_Get_Sfnt_Name(face, i, &sfnt_name))) {
            return NULL;
        }

        if (sfnt_name.name_id == name) {
            return Py_SfntName_cnew((Py_Face *)self->base.owner, i);
        }
    }

    PyErr_Format(PyExc_KeyError, "No SFNT name of type '%d'", name);

    return NULL;
};


static PyMethodDef Py_SfntNames_methods[] = {
    SFNTNAMES_METHOD(get_name),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Sequence
*/


static Py_ssize_t Py_SfntNames_len(Py_SfntNames *self)
{
    return (Py_ssize_t)FT_Get_Sfnt_Name_Count(((Py_Face *)self->base.owner)->x);
}


static PyObject *Py_SfntNames_getitem(Py_SfntNames *self, Py_ssize_t i)
{
    Py_ssize_t len;

    len = (Py_ssize_t)FT_Get_Sfnt_Name_Count(((Py_Face *)self->base.owner)->x);
    if (i < len) {
        return Py_SfntName_cnew((Py_Face *)self->base.owner, i);
    } else {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }
}


static PySequenceMethods Py_SfntNames_sequence;


/****************************************************************************
 Setup
*/


static PyTypeObject Py_SfntNames_Type;


int setup_SfntNames(PyObject *m)
{
    memset(&Py_SfntNames_sequence, 0, sizeof(PySequenceMethods));
    Py_SfntNames_sequence = (PySequenceMethods) {
        .sq_length = (lenfunc)Py_SfntNames_len,
        .sq_item = (ssizeargfunc)Py_SfntNames_getitem
    };

    memset(&Py_SfntNames_Type, 0, sizeof(PyTypeObject));
    Py_SfntNames_Type = (PyTypeObject) {
        .tp_name = "freetypy.SfntNames",
        .tp_basicsize = sizeof(Py_SfntNames),
        .tp_doc = doc_SfntNames__init__,
        .tp_as_sequence = &Py_SfntNames_sequence,
        .tp_init = (initproc)Py_SfntNames_init,
        .tp_methods = Py_SfntNames_methods,
    };

    ftpy_setup_type(m, &Py_SfntNames_Type);

    return 0;
}
