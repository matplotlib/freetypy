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

#include "chariter.h"
#include "doc/chariter.h"

#include "face.h"


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    unsigned int glyph_index;
    unsigned long charcode;
    unsigned int started;
} Py_CharIter;


static PyTypeObject Py_CharIter_Type;


PyObject *
Py_CharIter_cnew(PyObject *face)
{
    Py_CharIter *self;
    self = (Py_CharIter *)(&Py_CharIter_Type)->tp_alloc(&Py_CharIter_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF(face);
    self->base.owner = (PyObject *)face;
    self->started = 0;
    return (PyObject *)self;
}


static PyObject *
Py_CharIter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_CharIter *self;

    self = (Py_CharIter *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->started = 0;
    return (PyObject *)self;
}


static int
Py_CharIter_init(Py_CharIter *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "CharIter objects may not be instantiated directly. "
        "Use Face.get_glyphs instead.");
    return -1;
}


static PyObject *
Py_CharIter_next(Py_CharIter *self)
{
    Py_Face *face = (Py_Face *)self->base.owner;

    if (self->started) {
        self->charcode = FT_Get_Next_Char(
            face->x, self->charcode, &self->glyph_index);
    } else {
        self->charcode = FT_Get_First_Char(
            face->x, &self->glyph_index);
        self->started = 1;
    }

    if (self->glyph_index) {
        return Py_BuildValue("kI", self->charcode, self->glyph_index);
    }

    return NULL;
}


/****************************************************************************
 Setup
*/


static PyTypeObject Py_CharIter_Type;


int setup_CharIter(PyObject *m)
{
    memset(&Py_CharIter_Type, 0, sizeof(PyTypeObject));
    Py_CharIter_Type = (PyTypeObject) {
        .tp_name = "freetypy.CharIter",
        .tp_basicsize = sizeof(Py_CharIter),
        .tp_doc = doc_CharIter__init__,
        .tp_iter = PyObject_SelfIter,
        .tp_iternext = (iternextfunc)Py_CharIter_next,
        .tp_init = (initproc)Py_CharIter_init,
        .tp_new = Py_CharIter_new
    };

    ftpy_setup_type(m, &Py_CharIter_Type);

    return 0;
}
