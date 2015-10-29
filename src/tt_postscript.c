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

#include "tt_postscript.h"
#include "doc/tt_postscript.h"
#include "face.h"

#include FT_TRUETYPE_TABLES_H


#define MAKE_TT_POSTSCRIPT_GETTER(name, convert_func, member)   \
    MAKE_GETTER(Py_TT_Postscript, name, convert_func, member)
#define TT_POSTSCRIPT_DEF_GETTER(name) DEF_GETTER(name, doc_TT_Postscript_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    TT_Postscript *x;
} Py_TT_Postscript;


static PyTypeObject Py_TT_Postscript_Type;


PyObject *
Py_TT_Postscript_cnew(PyObject *face)
{
    Py_Face *f = (Py_Face *)face;
    Py_TT_Postscript *self;
    TT_Postscript *orig;

    orig = (TT_Postscript *)FT_Get_Sfnt_Table(f->x, ft_sfnt_post);
    if (orig == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No post found");
        return NULL;
    }

    self = (Py_TT_Postscript *)(&Py_TT_Postscript_Type)->tp_alloc(&Py_TT_Postscript_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = face;
    self->x = orig;
    return (PyObject *)self;
}


static int
Py_TT_Postscript_init(Py_TT_Postscript *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "TT_Postscript objects may not be instantiated directly. "
        "Use Face.get_postscript() instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

MAKE_TT_POSTSCRIPT_GETTER(format_type, ftpy_PyFloat_FromFT_FIXED, self->x->FormatType)
MAKE_TT_POSTSCRIPT_GETTER(italic_angle, ftpy_PyFloat_FromFT_FIXED, self->x->italicAngle)
MAKE_TT_POSTSCRIPT_GETTER(underline_position, PyLong_FromLong, self->x->underlinePosition)
MAKE_TT_POSTSCRIPT_GETTER(underline_thickness, PyLong_FromLong, self->x->underlineThickness)
MAKE_TT_POSTSCRIPT_GETTER(is_fixed_pitch, PyBool_FromLong, self->x->isFixedPitch)
MAKE_TT_POSTSCRIPT_GETTER(min_mem_type42, PyLong_FromUnsignedLong, self->x->minMemType42)
MAKE_TT_POSTSCRIPT_GETTER(max_mem_type42, PyLong_FromUnsignedLong, self->x->maxMemType42)
MAKE_TT_POSTSCRIPT_GETTER(min_mem_type1, PyLong_FromUnsignedLong, self->x->minMemType1)
MAKE_TT_POSTSCRIPT_GETTER(max_mem_type1, PyLong_FromUnsignedLong, self->x->maxMemType1)


static PyGetSetDef Py_TT_Postscript_getset[] = {
    TT_POSTSCRIPT_DEF_GETTER(format_type),
    TT_POSTSCRIPT_DEF_GETTER(italic_angle),
    TT_POSTSCRIPT_DEF_GETTER(underline_position),
    TT_POSTSCRIPT_DEF_GETTER(underline_thickness),
    TT_POSTSCRIPT_DEF_GETTER(is_fixed_pitch),
    TT_POSTSCRIPT_DEF_GETTER(min_mem_type42),
    TT_POSTSCRIPT_DEF_GETTER(max_mem_type42),
    TT_POSTSCRIPT_DEF_GETTER(min_mem_type1),
    TT_POSTSCRIPT_DEF_GETTER(max_mem_type1),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_TT_Postscript_Type;


int setup_TT_Postscript(PyObject *m)
{
    memset(&Py_TT_Postscript_Type, 0, sizeof(PyTypeObject));
    Py_TT_Postscript_Type = (PyTypeObject) {
        .tp_name = "freetypy.TT_Postscript",
        .tp_basicsize = sizeof(Py_TT_Postscript),
        .tp_doc = doc_TT_Postscript__init__,
        .tp_getset = Py_TT_Postscript_getset,
        .tp_init = (initproc)Py_TT_Postscript_init,
    };

    ftpy_setup_type(m, &Py_TT_Postscript_Type);

    return 0;
}
