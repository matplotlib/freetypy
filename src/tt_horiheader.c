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

#include "tt_horiheader.h"
#include "doc/tt_horiheader.h"

#include "face.h"

#include FT_TRUETYPE_TABLES_H


#define MAKE_TT_HORIHEADER_GETTER(name, convert_func, member)   \
    MAKE_GETTER(Py_TT_HoriHeader, name, convert_func, member)
#define TT_HORIHEADER_DEF_GETTER(name) DEF_GETTER(name, doc_TT_HoriHeader_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    TT_HoriHeader *x;
} Py_TT_HoriHeader;


static PyTypeObject Py_TT_HoriHeader_Type;


PyObject *
Py_TT_HoriHeader_cnew(PyObject *face)
{
    Py_Face *f = (Py_Face *)face;
    Py_TT_HoriHeader *self;
    TT_HoriHeader *orig;

    orig = (TT_HoriHeader *)FT_Get_Sfnt_Table(f->x, ft_sfnt_hhea);
    if (orig == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No hhea found");
        return NULL;
    }

    self = (Py_TT_HoriHeader *)(&Py_TT_HoriHeader_Type)->tp_alloc(&Py_TT_HoriHeader_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = face;
    self->x = orig;
    return (PyObject *)self;
}


static int
Py_TT_HoriHeader_init(Py_TT_HoriHeader *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "TT_HoriHeader objects may not be instantiated directly. "
        "Use Face.tt_horiheader instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

MAKE_TT_HORIHEADER_GETTER(version, ftpy_PyFloat_FromFT_FIXED, self->x->Version);
MAKE_TT_HORIHEADER_GETTER(ascender, PyLong_FromLong, self->x->Ascender);
MAKE_TT_HORIHEADER_GETTER(descender, PyLong_FromLong, self->x->Descender);
MAKE_TT_HORIHEADER_GETTER(line_gap, PyLong_FromLong, self->x->Line_Gap);
MAKE_TT_HORIHEADER_GETTER(advance_width_max, PyLong_FromUnsignedLong, self->x->advance_Width_Max);
MAKE_TT_HORIHEADER_GETTER(min_left_side_bearing, PyLong_FromLong, self->x->min_Left_Side_Bearing);
MAKE_TT_HORIHEADER_GETTER(min_right_side_bearing, PyLong_FromLong, self->x->min_Right_Side_Bearing);
MAKE_TT_HORIHEADER_GETTER(xmax_extent, PyLong_FromLong, self->x->xMax_Extent);
MAKE_TT_HORIHEADER_GETTER(caret_slope_rise, PyLong_FromLong, self->x->caret_Slope_Rise);
MAKE_TT_HORIHEADER_GETTER(caret_slope_run, PyLong_FromLong, self->x->caret_Slope_Run);
MAKE_TT_HORIHEADER_GETTER(caret_offset, PyLong_FromLong, self->x->caret_Offset);
MAKE_TT_HORIHEADER_GETTER(metric_data_format, PyLong_FromLong, self->x->metric_Data_Format);
MAKE_TT_HORIHEADER_GETTER(number_of_hmetrics, PyLong_FromLong, self->x->number_Of_HMetrics);

static PyGetSetDef Py_TT_HoriHeader_getset[] = {
    TT_HORIHEADER_DEF_GETTER(version),
    TT_HORIHEADER_DEF_GETTER(ascender),
    TT_HORIHEADER_DEF_GETTER(descender),
    TT_HORIHEADER_DEF_GETTER(line_gap),
    TT_HORIHEADER_DEF_GETTER(advance_width_max),
    TT_HORIHEADER_DEF_GETTER(min_left_side_bearing),
    TT_HORIHEADER_DEF_GETTER(min_right_side_bearing),
    TT_HORIHEADER_DEF_GETTER(xmax_extent),
    TT_HORIHEADER_DEF_GETTER(caret_slope_rise),
    TT_HORIHEADER_DEF_GETTER(caret_slope_run),
    TT_HORIHEADER_DEF_GETTER(caret_offset),
    TT_HORIHEADER_DEF_GETTER(metric_data_format),
    TT_HORIHEADER_DEF_GETTER(number_of_hmetrics),
    {NULL}
};

/* TODO: hmtx table */


/****************************************************************************
 Setup
*/


static PyTypeObject Py_TT_HoriHeader_Type;


int setup_TT_HoriHeader(PyObject *m)
{
    memset(&Py_TT_HoriHeader_Type, 0, sizeof(PyTypeObject));
    Py_TT_HoriHeader_Type = (PyTypeObject) {
        .tp_name = "freetypy.TT_HoriHeader",
        .tp_basicsize = sizeof(Py_TT_HoriHeader),
        .tp_doc = doc_TT_HoriHeader__init__,
        .tp_getset = Py_TT_HoriHeader_getset,
        .tp_init = (initproc)Py_TT_HoriHeader_init,
    };

    ftpy_setup_type(m, &Py_TT_HoriHeader_Type);

    return 0;
}
