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

#include "charmap.h"
#include "doc/charmap.h"
#include "constants.h"
#include "truetype.h"

#include FT_TRUETYPE_TABLES_H
#include FT_TRUETYPE_IDS_H

/* TODO: Add a nice repr here */


#define CHARMAP_GETTER(name, type) GETTER(name, type, FT_CharMapRec, Py_CharMap);
#define CHARMAP_METHOD(name) DEF_METHOD(name, CharMap)
#define CHARMAP_METHOD_NOARGS(name) DEF_METHOD_NOARGS(name, CharMap)

static ftpy_ConstantType Py_FT_ENCODING_ConstantType;


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_CharMap x;
} Py_CharMap;


static PyTypeObject Py_CharMap_Type;


PyObject *
Py_CharMap_cnew(FT_CharMap charmap, PyObject *face)
{
    Py_CharMap *self;
    self = (Py_CharMap *)(&Py_CharMap_Type)->tp_alloc(&Py_CharMap_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->x = charmap;
    Py_INCREF(face);
    self->base.owner = face;
    return (PyObject *)self;
}


static PyObject *
Py_CharMap_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_CharMap *self;

    self = (Py_CharMap *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_CharMap_init(Py_CharMap *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "CharMap objects may not be instantiated directly. "
        "Use Face.charmaps instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


static PyObject *face_get(Py_CharMap *self, PyObject *closure)
{
    Py_INCREF(self->base.owner);
    return self->base.owner;
}


static PyObject *encoding_get(Py_CharMap *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_FT_ENCODING_ConstantType, self->x->encoding);
}


static PyObject *platform_id_get(Py_CharMap *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_PLATFORM_ConstantType, self->x->platform_id);
}


static PyObject *encoding_id_get(Py_CharMap *self, PyObject *closure)
{
    switch (self->x->platform_id) {
    case TT_PLATFORM_APPLE_UNICODE:
        return Py_Constant_cnew(
            &Py_TT_APPLE_ID_ConstantType, self->x->encoding_id);

    case TT_PLATFORM_MACINTOSH:
        return Py_Constant_cnew(
            &Py_TT_MAC_ID_ConstantType, self->x->encoding_id);

    case TT_PLATFORM_ISO:
        return Py_Constant_cnew(
            &Py_TT_ISO_ID_ConstantType, self->x->encoding_id);

    case TT_PLATFORM_MICROSOFT:
        return Py_Constant_cnew(
            &Py_TT_MS_ID_ConstantType, self->x->encoding_id);

    case TT_PLATFORM_CUSTOM:
        return PyLong_FromUnsignedLong(self->x->encoding_id);

    case TT_PLATFORM_ADOBE:
        return Py_Constant_cnew(
            &Py_TT_ADOBE_ID_ConstantType, self->x->encoding_id);

    default:
        PyErr_Format(
            PyExc_ValueError, "Unknown platform value %d", self->x->encoding_id);
        return NULL;
    }
}


static PyGetSetDef Py_CharMap_getset[] = {
    DEF_GETTER(face, doc_CharMap_face),
    DEF_GETTER(encoding, doc_CharMap_encoding),
    DEF_GETTER(platform_id, doc_CharMap_platform_id),
    DEF_GETTER(encoding_id, doc_CharMap_encoding_id),
    {NULL}
};


/****************************************************************************
 Methods
*/


static PyObject*
Py_CharMap_get_format(Py_CharMap *self, PyObject *args, PyObject *kwds)
{
    long result;

    result = FT_Get_CMap_Format(self->x);

    if (result == -1) {
        PyErr_SetString(
            PyExc_ValueError,
            "CharMap does not have a TrueType/sfnt table");
        return NULL;
    }

    return PyLong_FromLong(result);
}


static PyObject*
Py_CharMap_get_language_id(Py_CharMap *self, PyObject *args, PyObject *kwds)
{
    unsigned long lang_id;

    lang_id = FT_Get_CMap_Language_ID(self->x);

    if (self->x->platform_id == TT_PLATFORM_MACINTOSH) {
        return Py_Constant_cnew(
            &Py_TT_MAC_LANGID_ConstantType, lang_id);
    } else if (self->x->platform_id == TT_PLATFORM_MICROSOFT) {
        return Py_Constant_cnew(
            &Py_TT_MS_LANGID_ConstantType, lang_id);
    }

    return PyLong_FromUnsignedLong(lang_id);
}


static PyMethodDef Py_CharMap_methods[] = {
    CHARMAP_METHOD_NOARGS(get_format),
    CHARMAP_METHOD_NOARGS(get_language_id),
    {NULL}
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_CharMap_Type;


static PyTypeObject Py_FT_ENCODING_Type;
#define ENCODING_CONST(name) DEF_CONST(name, FT_ENCODING)
static constant_def FT_ENCODING_constants[] = {
    ENCODING_CONST(NONE),
    ENCODING_CONST(UNICODE),
    ENCODING_CONST(MS_SYMBOL),
    ENCODING_CONST(SJIS),
    ENCODING_CONST(GB2312),
    ENCODING_CONST(BIG5),
    ENCODING_CONST(WANSUNG),
    ENCODING_CONST(JOHAB),
    ENCODING_CONST(ADOBE_LATIN_1),
    ENCODING_CONST(ADOBE_STANDARD),
    ENCODING_CONST(ADOBE_EXPERT),
    ENCODING_CONST(ADOBE_CUSTOM),
    ENCODING_CONST(APPLE_ROMAN),
    {NULL}
};


int setup_CharMap(PyObject *m)
{
    memset(&Py_CharMap_Type, 0, sizeof(PyTypeObject));
    Py_CharMap_Type = (PyTypeObject) {
        .tp_name = "freetypy.CharMap",
        .tp_basicsize = sizeof(Py_CharMap),
        .tp_doc = doc_CharMap__init__,
        .tp_getset = Py_CharMap_getset,
        .tp_methods = Py_CharMap_methods,
        .tp_init = (initproc)Py_CharMap_init,
        .tp_new = Py_CharMap_new
    };

    if (ftpy_setup_type(m, &Py_CharMap_Type)) {
        return -1;
    }

    if (define_constant_namespace(
            m, &Py_FT_ENCODING_Type, &Py_FT_ENCODING_ConstantType,
            "freetypy.ENCODING",
            doc_ENCODING, FT_ENCODING_constants)) {
        return -1;
    }

    return 0;
}
