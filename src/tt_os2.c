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

#include "tt_os2.h"
#include "doc/tt_os2.h"

#include "constants.h"
#include "face.h"


#include FT_TRUETYPE_TABLES_H

#define MAKE_TT_OS2_GETTER(name, convert_func, member)  \
    MAKE_GETTER(Py_TT_OS2, name, convert_func, member)
#define TT_OS2_DEF_GETTER(name) DEF_GETTER(name, doc_TT_OS2_ ## name)


ftpy_ConstantType Py_TT_WEIGHT_CLASS_ConstantType;
ftpy_ConstantType Py_TT_WIDTH_CLASS_ConstantType;
ftpy_ConstantType Py_TT_FS_SELECTION_BitflagType;


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    PyObject *owner;
    TT_OS2 *x;
} Py_TT_OS2;


static PyTypeObject Py_TT_OS2_Type;


PyObject *
Py_TT_OS2_cnew(PyObject *face)
{
    Py_Face *f = (Py_Face *)face;
    Py_TT_OS2 *self;
    TT_OS2 *orig;

    orig = (TT_OS2 *)FT_Get_Sfnt_Table(f->x, ft_sfnt_os2);
    if (orig == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No os2 found");
        return NULL;
    }

    self = (Py_TT_OS2 *)(&Py_TT_OS2_Type)->tp_alloc(&Py_TT_OS2_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = face;
    self->x = orig;
    return (PyObject *)self;
}


static int
Py_TT_OS2_init(Py_TT_OS2 *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "TT_OS2 objects may not be instantiated directly. "
        "Use Face.get_os2() instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

MAKE_TT_OS2_GETTER(version, PyLong_FromUnsignedLong, self->x->version)
MAKE_TT_OS2_GETTER(x_avg_char_width, PyLong_FromLong, self->x->xAvgCharWidth)


static PyObject *weight_class_get(Py_TT_OS2 *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_WEIGHT_CLASS_ConstantType, self->x->usWeightClass);
}


static PyObject *width_class_get(Py_TT_OS2 *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_WIDTH_CLASS_ConstantType, self->x->usWidthClass);
}


static PyObject *type_get(Py_TT_OS2 *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_FT_FSTYPE_BitflagType, self->x->fsType);
}


MAKE_TT_OS2_GETTER(y_subscript_x_size, PyLong_FromLong, self->x->ySubscriptXSize)
MAKE_TT_OS2_GETTER(y_subscript_y_size, PyLong_FromLong, self->x->ySubscriptYSize)
MAKE_TT_OS2_GETTER(y_subscript_x_offset, PyLong_FromLong, self->x->ySubscriptXOffset)
MAKE_TT_OS2_GETTER(y_subscript_y_offset, PyLong_FromLong, self->x->ySubscriptYOffset)
MAKE_TT_OS2_GETTER(y_superscript_x_size, PyLong_FromLong, self->x->ySuperscriptXSize)
MAKE_TT_OS2_GETTER(y_superscript_y_size, PyLong_FromLong, self->x->ySuperscriptYSize)
MAKE_TT_OS2_GETTER(y_superscript_x_offset, PyLong_FromLong, self->x->ySuperscriptXOffset)
MAKE_TT_OS2_GETTER(y_superscript_y_offset, PyLong_FromLong, self->x->ySuperscriptYOffset)
MAKE_TT_OS2_GETTER(y_strikeout_size, PyLong_FromLong, self->x->yStrikeoutSize)
MAKE_TT_OS2_GETTER(y_strikeout_position, PyLong_FromLong, self->x->yStrikeoutPosition)


/* FamilyClass and FamilySubClass seem archaic, little used and
   complicated, so they aren't wrapped. */


static PyObject *family_class_get(Py_TT_OS2 *self, PyObject *closure)
{
    return PyLong_FromUnsignedLong(
        (self->x->sFamilyClass & 0xff00) >> 8);
}


static PyObject *family_subclass_get(Py_TT_OS2 *self, PyObject *closure)
{
    return PyLong_FromUnsignedLong(
        self->x->sFamilyClass & 0xff);
}


static PyObject *panose_get(Py_TT_OS2 *self, PyObject *closure)
{
    return PyBytes_FromStringAndSize(
        (const char *)self->x->panose, 10);
}

/* unicode_range -- the specification is unfinished, so there is no
   point in getting this out. */

static PyObject *vend_id_get(Py_TT_OS2 *self, PyObject *closure)
{
    return PyUnicode_FromStringAndSize(
        (const char *)self->x->achVendID, 4);
}


static PyObject *selection_get(Py_TT_OS2 *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_TT_FS_SELECTION_BitflagType, self->x->fsSelection);
}


MAKE_TT_OS2_GETTER(first_char_index, PyLong_FromUnsignedLong, self->x->usFirstCharIndex);
MAKE_TT_OS2_GETTER(last_char_index, PyLong_FromUnsignedLong, self->x->usLastCharIndex);
MAKE_TT_OS2_GETTER(typo_ascender, PyLong_FromLong, self->x->sTypoAscender);
MAKE_TT_OS2_GETTER(typo_descender, PyLong_FromLong, self->x->sTypoDescender);
MAKE_TT_OS2_GETTER(typo_line_gap, PyLong_FromLong, self->x->sTypoLineGap);
MAKE_TT_OS2_GETTER(win_ascent, PyLong_FromUnsignedLong, self->x->usWinAscent);
MAKE_TT_OS2_GETTER(win_descent, PyLong_FromUnsignedLong, self->x->usWinDescent);
MAKE_TT_OS2_GETTER(x_height, PyLong_FromLong, self->x->sxHeight);
MAKE_TT_OS2_GETTER(cap_height, PyLong_FromLong, self->x->sCapHeight);
MAKE_TT_OS2_GETTER(default_char, PyLong_FromUnsignedLong, self->x->usDefaultChar);
MAKE_TT_OS2_GETTER(break_char, PyLong_FromUnsignedLong, self->x->usBreakChar);
MAKE_TT_OS2_GETTER(max_context, PyLong_FromUnsignedLong, self->x->usMaxContext);


static PyGetSetDef Py_TT_OS2_getset[] = {
    TT_OS2_DEF_GETTER(version),
    TT_OS2_DEF_GETTER(x_avg_char_width),
    TT_OS2_DEF_GETTER(weight_class),
    TT_OS2_DEF_GETTER(width_class),
    TT_OS2_DEF_GETTER(type),
    TT_OS2_DEF_GETTER(y_subscript_x_size),
    TT_OS2_DEF_GETTER(y_subscript_y_size),
    TT_OS2_DEF_GETTER(y_subscript_x_offset),
    TT_OS2_DEF_GETTER(y_subscript_y_offset),
    TT_OS2_DEF_GETTER(y_superscript_x_size),
    TT_OS2_DEF_GETTER(y_superscript_y_size),
    TT_OS2_DEF_GETTER(y_superscript_x_offset),
    TT_OS2_DEF_GETTER(y_superscript_y_offset),
    TT_OS2_DEF_GETTER(y_strikeout_size),
    TT_OS2_DEF_GETTER(y_strikeout_position),
    TT_OS2_DEF_GETTER(family_class),
    TT_OS2_DEF_GETTER(family_subclass),
    TT_OS2_DEF_GETTER(panose),
    TT_OS2_DEF_GETTER(vend_id),
    TT_OS2_DEF_GETTER(selection),
    TT_OS2_DEF_GETTER(first_char_index),
    TT_OS2_DEF_GETTER(last_char_index),
    TT_OS2_DEF_GETTER(typo_ascender),
    TT_OS2_DEF_GETTER(typo_descender),
    TT_OS2_DEF_GETTER(typo_line_gap),
    TT_OS2_DEF_GETTER(win_ascent),
    TT_OS2_DEF_GETTER(win_descent),
    TT_OS2_DEF_GETTER(x_height),
    TT_OS2_DEF_GETTER(cap_height),
    TT_OS2_DEF_GETTER(default_char),
    TT_OS2_DEF_GETTER(break_char),
    TT_OS2_DEF_GETTER(max_context),
    {NULL}
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_TT_WEIGHT_CLASS_Type;
#define TT_WEIGHT_CLASS_CONST(name) DEF_CONST(name, TT_WEIGHT_CLASS)
static constant_def TT_WEIGHT_CLASS_constants[] = {
    {"ULTRA_LIGHT", 1},
    {"EXTRA_LIGHT", 2},
    {"LIGHT", 3},
    {"SEMI_LIGHT", 4},
    {"MEDIUM", 5},
    {"NORMAL", 5},
    {"SEMI_BOLD", 6},
    {"BOLD", 7},
    {"EXTRA_BOLD", 8},
    {"ULTRA_BOLD", 9},
    {NULL}
};


static PyTypeObject Py_TT_WIDTH_CLASS_Type;
#define TT_WIDTH_CLASS_CONST(name) DEF_CONST(name, TT_WIDTH_CLASS)
static constant_def TT_WIDTH_CLASS_constants[] = {
    {"ULTRA_CONDENSED", 100},
    {"EXTRA_CONDENSED", 200},
    {"CONDENSED", 300},
    {"SEMI_CONDENSED", 400},
    {"MEDIUM", 500},
    {"NORMAL", 500},
    {"SEMI_EXPANDED", 600},
    {"EXPANDED", 700},
    {"EXTRA_EXPANDED", 800},
    {"ULTRA_EXPANDED", 900},
    {NULL}
};


static PyTypeObject Py_TT_FS_SELECTION_Type;
#define TT_FS_SELECTION_CONST(name) DEF_CONST(name, TT_FS_SELECTION)
static constant_def TT_FS_SELECTION_constants[] = {
    {"ITALIC", 0x1},
    {"UNDERSCORE", 0x2},
    {"NEGATIVE", 0x4},
    {"OUTLINED", 0x8},
    {"STRIKEOUT", 0x10},
    {"BOLD", 0x20},
    {NULL}
};


static PyTypeObject Py_TT_OS2_Type;


int setup_TT_OS2(PyObject *m)
{
    memset(&Py_TT_OS2_Type, 0, sizeof(PyTypeObject));
    Py_TT_OS2_Type = (PyTypeObject) {
        .tp_name = "freetypy.TT_OS2",
        .tp_basicsize = sizeof(Py_TT_OS2),
        .tp_doc = doc_TT_OS2__init__,
        .tp_getset = Py_TT_OS2_getset,
        .tp_init = (initproc)Py_TT_OS2_init,
    };

    ftpy_setup_type(m, &Py_TT_OS2_Type);

    if (PyType_Ready(&Py_TT_OS2_Type) < 0)
        return -1;
    Py_INCREF(&Py_TT_OS2_Type);
    PyModule_AddObject(m, "TT_OS2", (PyObject *)&Py_TT_OS2_Type);

    if (define_constant_namespace(
            m, &Py_TT_WEIGHT_CLASS_Type, &Py_TT_WEIGHT_CLASS_ConstantType,
            "freetypy.TT_WEIGHT_CLASS",
            doc_TT_WEIGHT_CLASS, TT_WEIGHT_CLASS_constants)) {
        return -1;
    }

    if (define_constant_namespace(
            m, &Py_TT_WIDTH_CLASS_Type, &Py_TT_WIDTH_CLASS_ConstantType,
            "freetypy.TT_WIDTH_CLASS",
            doc_TT_WIDTH_CLASS, TT_WIDTH_CLASS_constants)) {
        return -1;
    }

    if (define_bitflag_namespace(
            m, &Py_TT_FS_SELECTION_Type, &Py_TT_FS_SELECTION_BitflagType,
            "freetypy.TT_FS_SELECTION",
            doc_TT_FS_SELECTION, TT_FS_SELECTION_constants)) {
        return -1;
    }

    return 0;
}
