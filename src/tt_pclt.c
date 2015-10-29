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

#include "tt_pclt.h"
#include "doc/tt_pclt.h"
#include "face.h"

#include FT_TRUETYPE_TABLES_H


#define MAKE_TT_PCLT_GETTER(name, convert_func, member) \
    MAKE_GETTER(Py_TT_Pclt, name, convert_func, member)
#define TT_PCLT_DEF_GETTER(name) DEF_GETTER(name, NULL)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    TT_PCLT *x;
} Py_TT_Pclt;


static PyTypeObject Py_TT_Pclt_Type;


PyObject *
Py_TT_Pclt_cnew(PyObject *face)
{
    Py_Face *f = (Py_Face *)face;
    Py_TT_Pclt *self;
    TT_PCLT *orig;

    orig = (TT_PCLT *)FT_Get_Sfnt_Table(f->x, ft_sfnt_pclt);
    if (orig == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No pclt found");
        return NULL;
    }

    self = (Py_TT_Pclt *)(&Py_TT_Pclt_Type)->tp_alloc(&Py_TT_Pclt_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = face;
    self->x = orig;
    return (PyObject *)self;
}


static int
Py_TT_Pclt_init(Py_TT_Pclt *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "TT_Pclt objects may not be instantiated directly. "
        "Use Face.get_pclt() instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

MAKE_TT_PCLT_GETTER(version, ftpy_PyFloat_FromFT_FIXED, self->x->Version)
MAKE_TT_PCLT_GETTER(font_number, PyLong_FromUnsignedLong, self->x->FontNumber)
MAKE_TT_PCLT_GETTER(pitch, PyLong_FromUnsignedLong, self->x->Pitch)
MAKE_TT_PCLT_GETTER(x_height, PyLong_FromUnsignedLong, self->x->xHeight)
MAKE_TT_PCLT_GETTER(style, PyLong_FromUnsignedLong, self->x->Style)
MAKE_TT_PCLT_GETTER(type_family, PyLong_FromUnsignedLong, self->x->TypeFamily)
MAKE_TT_PCLT_GETTER(cap_height, PyLong_FromUnsignedLong, self->x->CapHeight)
MAKE_TT_PCLT_GETTER(symbol_set, PyLong_FromUnsignedLong, self->x->SymbolSet)


static PyObject *type_face_get(Py_TT_Pclt *self, PyObject *closure)
{
    return PyUnicode_FromString((const char *)self->x->TypeFace + 2);
}


static PyObject *character_complement_get(Py_TT_Pclt *self, PyObject *closure)
{
    unsigned long result;
    int i;

    result = 0;
    for (i = 0; i < 8; ++i) {
        result |= (self->x->CharacterComplement[i] << (CHAR_BIT * i));
    }

    return PyLong_FromUnsignedLong(result);
}


static PyObject *file_name_get(Py_TT_Pclt *self, PyObject *closure)
{
    return PyUnicode_FromStringAndSize((const char *)self->x->FileName, 6);
}


MAKE_TT_PCLT_GETTER(stroke_weight, PyLong_FromLong, self->x->StrokeWeight)
MAKE_TT_PCLT_GETTER(width_type, PyLong_FromLong, self->x->WidthType)
MAKE_TT_PCLT_GETTER(serif_style, PyLong_FromLong, self->x->SerifStyle)


static PyGetSetDef Py_TT_Pclt_getset[] = {
    TT_PCLT_DEF_GETTER(version),
    TT_PCLT_DEF_GETTER(font_number),
    TT_PCLT_DEF_GETTER(pitch),
    TT_PCLT_DEF_GETTER(x_height),
    TT_PCLT_DEF_GETTER(style),
    TT_PCLT_DEF_GETTER(type_family),
    TT_PCLT_DEF_GETTER(cap_height),
    TT_PCLT_DEF_GETTER(symbol_set),
    TT_PCLT_DEF_GETTER(type_face),
    TT_PCLT_DEF_GETTER(character_complement),
    TT_PCLT_DEF_GETTER(file_name),
    TT_PCLT_DEF_GETTER(stroke_weight),
    TT_PCLT_DEF_GETTER(width_type),
    TT_PCLT_DEF_GETTER(serif_style),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_TT_Pclt_Type;


int setup_TT_Pclt(PyObject *m)
{
    memset(&Py_TT_Pclt_Type, 0, sizeof(PyTypeObject));
    Py_TT_Pclt_Type = (PyTypeObject) {
        .tp_name = "freetypy.TT_Pclt",
        .tp_basicsize = sizeof(Py_TT_Pclt),
        .tp_doc = doc_TT_Pclt__init__,
        .tp_getset = Py_TT_Pclt_getset,
        .tp_init = (initproc)Py_TT_Pclt_init,
    };

    ftpy_setup_type(m, &Py_TT_Pclt_Type);

    if (PyType_Ready(&Py_TT_Pclt_Type) < 0)
        return -1;
    Py_INCREF(&Py_TT_Pclt_Type);
    PyModule_AddObject(m, "TT_Pclt", (PyObject *)&Py_TT_Pclt_Type);

    return 0;
}
