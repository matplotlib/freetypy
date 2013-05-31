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

#include "subglyphs.h"
#include "doc/subglyphs.h"
#include "subglyph.h"


#define SUBGLYPHS_GETTER(name, type) GETTER(name, type, FT_SubGlyphsRec, Py_SubGlyphs);


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    PyObject *owner;
} Py_SubGlyphs;


static PyTypeObject Py_SubGlyphs_Type;


PyObject *
Py_SubGlyphs_cnew(Py_Glyph *glyph)
{
    Py_SubGlyphs *self;
    self = (Py_SubGlyphs *)(&Py_SubGlyphs_Type)->tp_alloc(&Py_SubGlyphs_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    Py_INCREF((PyObject *)glyph);
    self->base.owner = (PyObject *)glyph;
    return (PyObject *)self;
}


static int
Py_SubGlyphs_init(Py_SubGlyphs *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "SubGlyphs objects may not be instantiated directly. "
        "Use Glyph.subglyphs instead.");
    return -1;
}


/****************************************************************************
 Sequence
*/


static Py_ssize_t Py_SubGlyphs_len(Py_SubGlyphs *self)
{
    Py_Glyph *glyph = (Py_Glyph *)self->owner;
    return (Py_ssize_t)glyph->x->num_subglyphs;
}


static PyObject *Py_SubGlyphs_getitem(Py_SubGlyphs *self, Py_ssize_t i)
{
    Py_ssize_t len;
    Py_Glyph *glyph = (Py_Glyph *)self->owner;

    len = (Py_ssize_t)glyph->x->num_subglyphs;
    if (i < len) {
        return Py_SubGlyph_cnew(glyph, i);
    } else {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }
}


static PySequenceMethods Py_SubGlyphs_sequence;


/****************************************************************************
 Setup
*/


static PyTypeObject Py_SubGlyphs_Type;


int setup_SubGlyphs(PyObject *m)
{
    memset(&Py_SubGlyphs_sequence, 0, sizeof(PySequenceMethods));
    Py_SubGlyphs_sequence = (PySequenceMethods) {
        .sq_length = (lenfunc)Py_SubGlyphs_len,
        .sq_item = (ssizeargfunc)Py_SubGlyphs_getitem
    };

    memset(&Py_SubGlyphs_Type, 0, sizeof(PyTypeObject));
    Py_SubGlyphs_Type = (PyTypeObject) {
        .tp_name = "freetypy.SubGlyphs",
        .tp_basicsize = sizeof(Py_SubGlyphs),
        .tp_doc = doc_SubGlyphs__init__,
        .tp_as_sequence = &Py_SubGlyphs_sequence,
        .tp_init = (initproc)Py_SubGlyphs_init,
    };

    ftpy_setup_type(m, &Py_SubGlyphs_Type);

    return 0;
}
