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

#include "subglyph.h"
#include "doc/subglyph.h"

#include "constants.h"
#include "matrix.h"


typedef struct {
    ftpy_Object base;
    FT_Int index;
    FT_UInt flags;
    FT_Int arg1;
    FT_Int arg2;
    FT_Matrix transform;
} Py_SubGlyph;


#define MAKE_SUBGLYPH_GETTER(name, convert_func, member)        \
    MAKE_GETTER(Py_SubGlyph, name, convert_func, member)
#define DEF_SUBGLYPH_GETTER(name) DEF_GETTER(name, doc_SubGlyph_ ## name)
#define SUBGLYPH_METHOD(name) DEF_METHOD(name, SubGlyph)


/****************************************************************************
 Object basics
*/


static ftpy_ConstantType Py_FT_SUBGLYPH_FLAG_ConstantType;

static PyTypeObject Py_SubGlyph_Type;


PyObject *
Py_SubGlyph_cnew(Py_Glyph *glyph, unsigned int index)
{
    Py_SubGlyph *self;
    self = (Py_SubGlyph *)(&Py_SubGlyph_Type)->tp_alloc(&Py_SubGlyph_Type, 0);
    if (self == NULL) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Get_SubGlyph_Info(
                glyph->x, index, &self->index, &self->flags,
                &self->arg1, &self->arg2, &self->transform))) {
        Py_DECREF(self);
        return NULL;
    }

    Py_INCREF((PyObject *)glyph);
    self->base.owner = (PyObject *)glyph;
    return (PyObject *)self;
}


static int
Py_SubGlyph_init(Py_SubGlyph *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "SubGlyph objects may not be instantiated directly. "
        "Use Glyph.subglyphs instead.");
    return -1;
}


/****************************************************************************
 Getters
*/


MAKE_SUBGLYPH_GETTER(index, PyLong_FromLong, self->index)


static PyObject *flags_get(Py_SubGlyph *self, PyObject *closure)
{
    return Py_Constant_cnew(&Py_FT_SUBGLYPH_FLAG_ConstantType, self->flags);
}


MAKE_SUBGLYPH_GETTER(arg1, PyLong_FromUnsignedLong, self->arg1);
MAKE_SUBGLYPH_GETTER(arg2, PyLong_FromUnsignedLong, self->arg2);
MAKE_SUBGLYPH_GETTER(transform, Py_Matrix_cnew, &self->transform);


static PyGetSetDef Py_SubGlyph_getset[] = {
    DEF_SUBGLYPH_GETTER(index),
    DEF_SUBGLYPH_GETTER(flags),
    DEF_SUBGLYPH_GETTER(arg1),
    DEF_SUBGLYPH_GETTER(arg2),
    DEF_SUBGLYPH_GETTER(transform),
    {NULL}
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_SubGlyph_Type;


static PyTypeObject Py_FT_SUBGLYPH_FLAG_Type;
#define SUBGLYPH_FLAG_CONST(name) DEF_CONST(name, FT_SUBGLYPH_FLAG)
static constant_def FT_SUBGLYPH_FLAG_constants[] = {
    SUBGLYPH_FLAG_CONST(ARGS_ARE_WORDS),
    SUBGLYPH_FLAG_CONST(ARGS_ARE_XY_VALUES),
    SUBGLYPH_FLAG_CONST(ROUND_XY_TO_GRID),
    SUBGLYPH_FLAG_CONST(SCALE),
    SUBGLYPH_FLAG_CONST(XY_SCALE),
    {"TWOXTWO", FT_SUBGLYPH_FLAG_2X2},
    SUBGLYPH_FLAG_CONST(USE_MY_METRICS),
    {NULL}
};


int setup_SubGlyph(PyObject *m)
{
    memset(&Py_SubGlyph_Type, 0, sizeof(PyTypeObject));
    Py_SubGlyph_Type = (PyTypeObject) {
        .tp_name = "freetypy.SubGlyph",
        .tp_basicsize = sizeof(Py_SubGlyph),
        .tp_doc = doc_SubGlyph__init__,
        .tp_getset = Py_SubGlyph_getset,
        .tp_init = (initproc)Py_SubGlyph_init,
    };

    ftpy_setup_type(m, &Py_SubGlyph_Type);

    if (PyType_Ready(&Py_SubGlyph_Type) < 0)
        return -1;
    Py_INCREF(&Py_SubGlyph_Type);
    PyModule_AddObject(m, "SubGlyph", (PyObject *)&Py_SubGlyph_Type);

    if (define_bitflag_namespace(
            m, &Py_FT_SUBGLYPH_FLAG_Type, &Py_FT_SUBGLYPH_FLAG_ConstantType,
            "freetypy.SUBGLYPH_FLAG",
            doc_SUBGLYPH_FLAG, FT_SUBGLYPH_FLAG_constants)) {
        return -1;
    }

    return 0;
}
