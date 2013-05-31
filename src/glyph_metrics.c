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

#include "glyph_metrics.h"
#include "doc/glyph_metrics.h"


#define MAKE_GLYPH_METRICS_GETTER(name, convert_func, member)   \
    MAKE_GETTER(Py_Glyph_Metrics, name, convert_func, member)
#define DEF_GLYPH_METRICS_GETTER(name) DEF_GETTER(name, doc_Glyph_Metrics_ ## name)


/****************************************************************************
 Object basics
*/


typedef struct {
    ftpy_Object base;
    FT_Glyph_Metrics *x;
} Py_Glyph_Metrics;


static PyTypeObject Py_Glyph_Metrics_Type;


PyObject *
Py_Glyph_Metrics_cnew(FT_Glyph_Metrics *glyph_metrics, PyObject *owner)
{
    Py_Glyph_Metrics *self;
    self = (Py_Glyph_Metrics *)(&Py_Glyph_Metrics_Type)->tp_alloc(&Py_Glyph_Metrics_Type, 0);
    if (self == NULL) {
        return NULL;
    }
    self->x = glyph_metrics;
    Py_INCREF(owner);
    self->base.owner = owner;
    return (PyObject *)self;
}


static PyObject *
Py_Glyph_Metrics_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Glyph_Metrics *self;

    self = (Py_Glyph_Metrics *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->x = NULL;
    return (PyObject *)self;
}


static int
Py_Glyph_Metrics_init(Py_Glyph_Metrics *self, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(
        PyExc_RuntimeError,
        "Glyph_Metrics objects may not be instantiated directly. "
        "Use Face.glyph.metrics instead.");
    return -1;
}


/****************************************************************************
 Getters
*/

/* TODO: Determine what scale these values are in based on the load flags */

MAKE_GLYPH_METRICS_GETTER(width, ftpy_PyFloat_FromF26DOT6, self->x->width);
MAKE_GLYPH_METRICS_GETTER(height, ftpy_PyFloat_FromF26DOT6, self->x->height);
MAKE_GLYPH_METRICS_GETTER(hori_bearing_x, ftpy_PyFloat_FromF26DOT6, self->x->horiBearingX);
MAKE_GLYPH_METRICS_GETTER(hori_bearing_y, ftpy_PyFloat_FromF26DOT6, self->x->horiBearingY);
MAKE_GLYPH_METRICS_GETTER(hori_advance, ftpy_PyFloat_FromF26DOT6, self->x->horiAdvance);
MAKE_GLYPH_METRICS_GETTER(vert_bearing_x, ftpy_PyFloat_FromF26DOT6, self->x->vertBearingX);
MAKE_GLYPH_METRICS_GETTER(vert_bearing_y, ftpy_PyFloat_FromF26DOT6, self->x->vertBearingY);
MAKE_GLYPH_METRICS_GETTER(vert_advance, ftpy_PyFloat_FromF26DOT6, self->x->vertAdvance);


static PyGetSetDef Py_Glyph_Metrics_getset[] = {
    DEF_GLYPH_METRICS_GETTER(width),
    DEF_GLYPH_METRICS_GETTER(height),
    DEF_GLYPH_METRICS_GETTER(hori_bearing_x),
    DEF_GLYPH_METRICS_GETTER(hori_bearing_y),
    DEF_GLYPH_METRICS_GETTER(hori_advance),
    DEF_GLYPH_METRICS_GETTER(vert_bearing_x),
    DEF_GLYPH_METRICS_GETTER(vert_bearing_y),
    DEF_GLYPH_METRICS_GETTER(vert_advance),
    {NULL}
};


/****************************************************************************
 Setup
*/


static PyTypeObject Py_Glyph_Metrics_Type;


int setup_Glyph_Metrics(PyObject *m)
{
    memset(&Py_Glyph_Metrics_Type, 0, sizeof(PyTypeObject));
    Py_Glyph_Metrics_Type = (PyTypeObject) {
        .tp_name = "freetypy.Glyph_Metrics",
        .tp_basicsize = sizeof(Py_Glyph_Metrics),
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
        .tp_doc = doc_Glyph_Metrics__init__,
        .tp_getset = Py_Glyph_Metrics_getset,
        .tp_init = (initproc)Py_Glyph_Metrics_init,
        .tp_new = Py_Glyph_Metrics_new
    };

    ftpy_setup_type(m, &Py_Glyph_Metrics_Type);

    return 0;
}
