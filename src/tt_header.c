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

#include "tt_header.h"
#include "doc/tt_header.h"
#include "face.h"

#include FT_TRUETYPE_TABLES_H


#define MAKE_TT_HEADER_GETTER(name, convert_func, member)       \
    MAKE_GETTER(Py_TT_Header, name, convert_func, member);
#define DEF_TT_HEADER_GETTER(name) DEF_GETTER(name, doc_TT_Header_ ## name)


ftpy_ConstantType Py_TT_HEADER_FLAGS_BitflagType;
ftpy_ConstantType Py_TT_MAC_STYLE_BitflagType;


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    TT_Header *x;
} Py_TT_Header;


static PyTypeObject Py_TT_Header_Type;


PyObject *
Py_TT_Header_cnew(PyObject *face)
{
    Py_Face *f = (Py_Face *)face;
    Py_TT_Header *self;
    TT_Header *orig;

    orig = (TT_Header *)FT_Get_Sfnt_Table(f->x, ft_sfnt_head);
    if (orig == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No head found");
        return NULL;
    }

    self = (Py_TT_Header *)(&Py_TT_Header_Type)->tp_alloc(&Py_TT_Header_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = face;
    self->x = orig;
    return (PyObject *)self;
}


static int
Py_TT_Header_init(Py_TT_Header *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "TT_Header objects may not be instantiated directly. "
        "Use Face.tt_header instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

MAKE_TT_HEADER_GETTER(table_version, ftpy_PyFloat_FromFT_FIXED, self->x->Table_Version)
MAKE_TT_HEADER_GETTER(font_revision, ftpy_PyFloat_FromFT_FIXED, self->x->Font_Revision)
MAKE_TT_HEADER_GETTER(checksum_adjust, PyLong_FromLong, self->x->CheckSum_Adjust)
MAKE_TT_HEADER_GETTER(magic_number, PyLong_FromLong, self->x->Magic_Number)


static PyObject *flags_get(Py_TT_Header *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_HEADER_FLAGS_BitflagType, self->x->Flags);
}


MAKE_TT_HEADER_GETTER(units_per_em, PyLong_FromUnsignedLong, self->x->Units_Per_EM);
MAKE_TT_HEADER_GETTER(created, ftpy_PyDateTime_FromTTDateTime, self->x->Created);
MAKE_TT_HEADER_GETTER(modified, ftpy_PyDateTime_FromTTDateTime, self->x->Modified);
MAKE_TT_HEADER_GETTER(x_min, PyLong_FromLong, self->x->xMin);
MAKE_TT_HEADER_GETTER(y_min, PyLong_FromLong, self->x->yMin);
MAKE_TT_HEADER_GETTER(x_max, PyLong_FromLong, self->x->xMax);
MAKE_TT_HEADER_GETTER(y_max, PyLong_FromLong, self->x->yMax);


static PyObject *mac_style_get(Py_TT_Header *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_MAC_STYLE_BitflagType, self->x->Mac_Style);
}


MAKE_TT_HEADER_GETTER(lowest_rec_ppem, PyLong_FromUnsignedLong, self->x->Lowest_Rec_PPEM);
MAKE_TT_HEADER_GETTER(font_direction, PyLong_FromLong, self->x->Font_Direction);


static PyGetSetDef Py_TT_Header_getset[] = {
    DEF_TT_HEADER_GETTER(table_version),
    DEF_TT_HEADER_GETTER(font_revision),
    DEF_TT_HEADER_GETTER(checksum_adjust),
    DEF_TT_HEADER_GETTER(magic_number),
    DEF_TT_HEADER_GETTER(flags),
    DEF_TT_HEADER_GETTER(units_per_em),
    DEF_TT_HEADER_GETTER(created),
    DEF_TT_HEADER_GETTER(modified),
    DEF_TT_HEADER_GETTER(x_min),
    DEF_TT_HEADER_GETTER(y_min),
    DEF_TT_HEADER_GETTER(x_max),
    DEF_TT_HEADER_GETTER(y_max),
    DEF_TT_HEADER_GETTER(mac_style),
    DEF_TT_HEADER_GETTER(lowest_rec_ppem),
    DEF_TT_HEADER_GETTER(font_direction),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_TT_Header_Type;


static PyTypeObject Py_TT_HEADER_FLAGS_Type;
#define TT_HEADER_FLAGS_CONST(name) DEF_CONST(name, TT_HEADER_FLAGS)
static constant_def TT_HEADER_FLAGS_constants[] = {
    {"BASELINE_AT_ZERO", 0x1},
    {"LEFT_BLACK_BIT_IS_LSB", 0x2},
    {"NONLINEAR_SCALE", 0x4},
    {"INTEGER_SCALING", 0x8},
    {"VERTICAL", 0x20},
    {"REQUIRES_LAYOUT", 0x80},
    {"METAMORPHOSIS_EFFECTS", 0x100},
    {"RIGHT_TO_LEFT_GLYPHS", 0x200},
    {"INDIC_REARRANGEMENT_EFFECTS", 0x400},
    {NULL}
};


static PyTypeObject Py_TT_MAC_STYLE_Type;
#define TT_MAC_STYLE_CONST(name) DEF_CONST(name, TT_MAC_STYLE)
static constant_def TT_MAC_STYLE_constants[] = {
    {"BOLD", 0x1},
    {"ITALIC", 0x2},
    {"UNDERLINE", 0x4},
    {"OUTLINE", 0x8},
    {"SHADOW", 0x10},
    {"CONDENSED", 0x20},
    {"EXTENDED", 0x40},
    {NULL}
};


int setup_TT_Header(PyObject *m)
{
    memset(&Py_TT_Header_Type, 0, sizeof(PyTypeObject));
    Py_TT_Header_Type = (PyTypeObject) {
        .tp_name = "freetypy.TT_Header",
        .tp_basicsize = sizeof(Py_TT_Header),
        .tp_doc = doc_TT_Header__init__,
        .tp_getset = Py_TT_Header_getset,
        .tp_init = (initproc)Py_TT_Header_init,
    };

    ftpy_setup_type(m, &Py_TT_Header_Type);

    if (define_bitflag_namespace(
            m, &Py_TT_HEADER_FLAGS_Type, &Py_TT_HEADER_FLAGS_BitflagType,
            "freetypy.TT_HEADER_FLAGS",
            doc_TT_HEADER_FLAGS, TT_HEADER_FLAGS_constants)) {
        return -1;
    }

    if (define_bitflag_namespace(
            m, &Py_TT_MAC_STYLE_Type, &Py_TT_MAC_STYLE_BitflagType,
            "freetypy.TT_MAC_STYLE",
            doc_TT_MAC_STYLE, TT_MAC_STYLE_constants)) {
        return -1;
    }

    return 0;
}
