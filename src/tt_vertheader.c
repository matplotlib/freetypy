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

#include "tt_vertheader.h"
#include "doc/tt_vertheader.h"

#include "face.h"

#include FT_TRUETYPE_TABLES_H


#define MAKE_TT_VERTHEADER_GETTER(name, convert_func, member)   \
    MAKE_GETTER(Py_TT_VertHeader, name, convert_func, member)
#define TT_VERTHEADER_DEF_GETTER(name) DEF_GETTER(name, doc_TT_VertHeader_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    TT_VertHeader *x;
} Py_TT_VertHeader;


static PyTypeObject Py_TT_VertHeader_Type;


PyObject *
Py_TT_VertHeader_cnew(PyObject *face)
{
    Py_Face *f = (Py_Face *)face;
    Py_TT_VertHeader *self;
    TT_VertHeader *orig;

    orig = (TT_VertHeader *)FT_Get_Sfnt_Table(f->x, ft_sfnt_vhea);
    if (orig == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No vhea found");
        return NULL;
    }

    self = (Py_TT_VertHeader *)(&Py_TT_VertHeader_Type)->tp_alloc(&Py_TT_VertHeader_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = face;
    self->x = orig;
    return (PyObject *)self;
}


static int
Py_TT_VertHeader_init(Py_TT_VertHeader *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "TT_VertHeader objects may not be instantiated directly. "
        "Use Face.tt_vertheader instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

MAKE_TT_VERTHEADER_GETTER(version, ftpy_PyFloat_FromFT_FIXED, self->x->Version);
MAKE_TT_VERTHEADER_GETTER(ascender, PyLong_FromLong, self->x->Ascender);
MAKE_TT_VERTHEADER_GETTER(descender, PyLong_FromLong, self->x->Descender);
MAKE_TT_VERTHEADER_GETTER(line_gap, PyLong_FromLong, self->x->Line_Gap);
MAKE_TT_VERTHEADER_GETTER(advance_height_max, PyLong_FromUnsignedLong, self->x->advance_Height_Max);
MAKE_TT_VERTHEADER_GETTER(min_top_side_bearing, PyLong_FromLong, self->x->min_Top_Side_Bearing);
MAKE_TT_VERTHEADER_GETTER(min_bottom_side_bearing, PyLong_FromLong, self->x->min_Bottom_Side_Bearing);
MAKE_TT_VERTHEADER_GETTER(ymax_extent, PyLong_FromLong, self->x->yMax_Extent);
MAKE_TT_VERTHEADER_GETTER(caret_slope_rise, PyLong_FromLong, self->x->caret_Slope_Rise);
MAKE_TT_VERTHEADER_GETTER(caret_slope_run, PyLong_FromLong, self->x->caret_Slope_Run);
MAKE_TT_VERTHEADER_GETTER(caret_offset, PyLong_FromLong, self->x->caret_Offset);
MAKE_TT_VERTHEADER_GETTER(metric_data_format, PyLong_FromLong, self->x->metric_Data_Format);
MAKE_TT_VERTHEADER_GETTER(number_of_vmetrics, PyLong_FromLong, self->x->number_Of_VMetrics);


static PyGetSetDef Py_TT_VertHeader_getset[] = {
    TT_VERTHEADER_DEF_GETTER(version),
    TT_VERTHEADER_DEF_GETTER(ascender),
    TT_VERTHEADER_DEF_GETTER(descender),
    TT_VERTHEADER_DEF_GETTER(line_gap),
    TT_VERTHEADER_DEF_GETTER(advance_height_max),
    TT_VERTHEADER_DEF_GETTER(min_top_side_bearing),
    TT_VERTHEADER_DEF_GETTER(min_bottom_side_bearing),
    TT_VERTHEADER_DEF_GETTER(ymax_extent),
    TT_VERTHEADER_DEF_GETTER(caret_slope_rise),
    TT_VERTHEADER_DEF_GETTER(caret_slope_run),
    TT_VERTHEADER_DEF_GETTER(caret_offset),
    TT_VERTHEADER_DEF_GETTER(metric_data_format),
    TT_VERTHEADER_DEF_GETTER(number_of_vmetrics),
    {NULL}
};

/* TODO: vmtx table */


/****************************************************************************
 Setup
*/


static PyTypeObject Py_TT_VertHeader_Type;


int setup_TT_VertHeader(PyObject *m)
{
    memset(&Py_TT_VertHeader_Type, 0, sizeof(PyTypeObject));
    Py_TT_VertHeader_Type = (PyTypeObject) {
        .tp_name = "freetypy.TT_VertHeader",
        .tp_basicsize = sizeof(Py_TT_VertHeader),
        .tp_doc = doc_TT_VertHeader__init__,
        .tp_getset = Py_TT_VertHeader_getset,
        .tp_init = (initproc)Py_TT_VertHeader_init,
    };

    ftpy_setup_type(m, &Py_TT_VertHeader_Type);

    return 0;
}
