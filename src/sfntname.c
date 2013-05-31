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

#include "sfntname.h"
#include "doc/sfntname.h"

#include "constants.h"
#include "encoding.h"
#include "truetype.h"


#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

#define DEF_SFNTNAME_GETTER(name) DEF_GETTER(name, doc_SfntName_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_SfntName x;
} Py_SfntName;


static PyTypeObject Py_SfntName_Type;


PyObject *
Py_SfntName_cnew(Py_Face *face, unsigned int idx)
{
    Py_SfntName *self;
    self = (Py_SfntName *)(&Py_SfntName_Type)->tp_alloc(&Py_SfntName_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->base.owner = NULL;

    memset(&self->x, 0, sizeof(FT_SfntName));

    if (ftpy_exc(
            FT_Get_Sfnt_Name(face->x, idx, &self->x))) {
        Py_DECREF(self);
        return NULL;
    }

    return (PyObject *)self;
}


static PyObject *
Py_SfntName_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_SfntName *self;

    self = (Py_SfntName *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    memset(&self->x, 0, sizeof(FT_SfntName));
    return (PyObject *)self;
}


static int
Py_SfntName_init(Py_SfntName *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "SfntName objects may not be instantiated directly. "
        "Use Face.sfnt_names[i] instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


static PyObject *platform_id_get(Py_SfntName *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_PLATFORM_ConstantType, self->x.platform_id);
}


static PyObject *encoding_id_get(Py_SfntName *self, PyObject *closure)
{
    switch (self->x.platform_id) {
    case TT_PLATFORM_APPLE_UNICODE:
        return Py_Constant_cnew(
            &Py_TT_APPLE_ID_ConstantType, self->x.encoding_id);

    case TT_PLATFORM_MACINTOSH:
        return Py_Constant_cnew(
            &Py_TT_MAC_ID_ConstantType, self->x.encoding_id);

    case TT_PLATFORM_ISO:
        return Py_Constant_cnew(
            &Py_TT_ISO_ID_ConstantType, self->x.encoding_id);

    case TT_PLATFORM_MICROSOFT:
        return Py_Constant_cnew(
            &Py_TT_MS_ID_ConstantType, self->x.encoding_id);

    case TT_PLATFORM_CUSTOM:
        return PyLong_FromUnsignedLong(self->x.encoding_id);

    case TT_PLATFORM_ADOBE:
        return Py_Constant_cnew(
            &Py_TT_ADOBE_ID_ConstantType, self->x.encoding_id);

    default:
        PyErr_Format(
            PyExc_ValueError, "Unknown platform value %d", self->x.encoding_id);
        return NULL;
    }
}


static PyObject *language_id_get(Py_SfntName *self, PyObject *closure)
{
    if (self->x.platform_id == TT_PLATFORM_MACINTOSH) {
        return Py_Constant_cnew(
            &Py_TT_MAC_LANGID_ConstantType, self->x.language_id);
    } else if (self->x.platform_id == TT_PLATFORM_MICROSOFT) {
        return Py_Constant_cnew(
            &Py_TT_MS_LANGID_ConstantType, self->x.language_id);
    } else {
        return PyLong_FromUnsignedLong(self->x.language_id);
    }
}


static PyObject *name_id_get(Py_SfntName *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_NAME_ID_ConstantType, self->x.name_id);
}


static PyObject *string_get(Py_SfntName *self, PyObject *closure)
{
    return ftpy_decode(self->x.platform_id, self->x.encoding_id,
                       (const char *)self->x.string, self->x.string_len);
}


static PyObject *string_bytes_get(Py_SfntName *self, PyObject *closure)
{
    return PyBytes_FromStringAndSize(
        (const char *)self->x.string, self->x.string_len);
}


static PyGetSetDef Py_SfntName_getset[] = {
    DEF_SFNTNAME_GETTER(platform_id),
    DEF_SFNTNAME_GETTER(encoding_id),
    DEF_SFNTNAME_GETTER(language_id),
    DEF_SFNTNAME_GETTER(name_id),
    DEF_SFNTNAME_GETTER(string),
    DEF_SFNTNAME_GETTER(string_bytes),
    {NULL}
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_SfntName_Type;


int setup_SfntName(PyObject *m)
{
    memset(&Py_SfntName_Type, 0, sizeof(PyTypeObject));
    Py_SfntName_Type = (PyTypeObject) {
        .tp_name = "freetypy.SfntName",
        .tp_basicsize = sizeof(Py_SfntName),
        .tp_doc = doc_SfntName__init__,
        .tp_getset = Py_SfntName_getset,
        .tp_init = (initproc)Py_SfntName_init,
        .tp_new = Py_SfntName_new
    };

    ftpy_setup_type(m, &Py_SfntName_Type);

    return 0;
}
