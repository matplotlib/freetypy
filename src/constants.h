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

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "freetypy.h"


#define DEF_CONST(name, category) {#name, category ## _ ## name}


typedef struct
{
    PyTypeObject type;
    PyTypeObject *namespace;
} ftpy_ConstantType;


typedef ftpy_ConstantType ftpy_BitflagType;


typedef struct
{
    char *name;
    long value;
} constant_def;


int define_constant_namespace(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name,
    const char *doc,
    const constant_def *constants);


int define_bitflag_namespace(
    PyObject *m,
    PyTypeObject *namespace_type,
    ftpy_ConstantType *constant_type,
    const char *qualified_name,
    const char *doc,
    const constant_def *constants);


PyObject *Py_Constant_cnew(ftpy_ConstantType *type, unsigned long value);


int setup_constants(PyObject *m);


#endif
