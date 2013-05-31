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

#ifndef __PYUTIL_H__
#define __PYUTIL_H__

#include "freetypy.h"


typedef struct {
    PyObject_HEAD
    PyObject *owner;
} ftpy_Object;


int ftpy_setup_type(PyObject *m, PyTypeObject *type);


int setup_pyutil(PyObject *m);


/* For an 8-bit string in Freetype that should be returned as a Unicode
   string in Python.  If NULL, return None to Python. */
#define T_NULL_STRING_AS_UNICODE 0x10001
/* For an 8-bit string in Freetype that should be returned as a Unicode
   string in Python.  Should never be NULL. */
#define T_STRING_AS_UNICODE      0x10002
/* FT_Fixed 16.16 fixed point values.  Returned to Python as a float. */
#define T_FT_FIXED               0x10003
/* FT_Vector (x, y) coordinate.  Returned to Python as a tuple. */
#define T_FT_VECTOR              0x10004
/* FT_Pos 26.6 fixed point values.  Returned to Python as a float. */
#define T_F26DOT6                0x10005
/* TrueType longDateTime values, which are seconds since Jan 1, 1904 */
#define T_TT_DATETIME            0x10006
/* A boolean value stored in an int32 */
#define T_BOOL_INT               0x10007


void ftpy_Object_dealloc(PyObject* self);


PyObject *ftpy_Object_new(PyTypeObject *type, PyObject *args, PyObject *kwds);


#define MAKE_GETTER(pytype, name, convert_func, member) \
    static PyObject * name ## _get(pytype *self, PyObject *closure) \
    {                                           \
        return convert_func(member);      \
    }                                           \


#define DEF_GETTER(name, doc)                \
  { #name, (getter)name ## _get, NULL, (doc) }


#define DEF_METHOD(name, type) \
    { #name, (PyCFunction)Py_ ## type ## _ ## name, METH_VARARGS|METH_KEYWORDS, doc_ ## type ## _ ## name }


#define DEF_METHOD_NOARGS(name, type) \
    { #name, (PyCFunction)Py_ ## type ## _ ## name, METH_VARARGS|METH_KEYWORDS, doc_ ## type ## _ ## name }


#define TO_F26DOT6(v) ((FT_F26Dot6)((double)(v) * (double)(1 << 6)))
#define TO_FT_FIXED(v) ((FT_Fixed)((double)(v) * (double)(1 << 16)))


#define FROM_F26DOT6(v) (((double)(v) / (double)(1 << 6)))
#define FROM_FT_FIXED(v) (((double)(v) / (double)(1 << 16)))


#define ftpy_PyFloat_FromF26DOT6(v) PyFloat_FromDouble(FROM_F26DOT6(v))
#define ftpy_PyFloat_FromFT_FIXED(v) PyFloat_FromDouble(FROM_FT_FIXED(v))
PyObject *ftpy_PyUnicode_FromStringOrNull(const char *val);
#define ftpy_Py_Vector_FromVector(v) Py_Vector_cnew(v, 1.0)
PyObject *ftpy_PyDateTime_FromTTDateTime(long *date);


#endif
