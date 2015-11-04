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

#ifndef __FILE_H__
#define __FILE_H__

#include "freetypy.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
#if defined(_MSC_VER) && defined(_WIN64) && (_MSC_VER > 1400)
    #include <io.h>
    #define ftpy_offset_t npy_int64
    #define ftpy_fseek _fseeki64
    #define ftpy_ftell _ftelli64
    #define ftpy_lseek _lseeki64

    #if NPY_SIZEOF_INT == 8
        #define FTPY_OFF_T_PYFMT "i"
    #elif NPY_SIZEOF_LONG == 8
        #define FTPY_OFF_T_PYFMT "l"
    #elif NPY_SIZEOF_LONGLONG == 8
        #define FTPY_OFF_T_PYFMT "L"
    #else
        #error Unsupported size for type off_t
    #endif
#else
    #define ftpy_fseek fseek
    #define ftpy_ftell ftell
    #define ftpy_lseek lseek
    #define ftpy_offset_t off_t

    #if NPY_SIZEOF_INT == NPY_SIZEOF_SHORT
        #define FTPY_OFF_T_PYFMT "h"
    #elif NPY_SIZEOF_INT == NPY_SIZEOF_INT
        #define FTPY_OFF_T_PYFMT "i"
    #elif NPY_SIZEOF_INT == NPY_SIZEOF_LONG
        #define FTPY_OFF_T_PYFMT "l"
    #elif NPY_SIZEOF_INT == NPY_SIZEOF_LONGLONG
        #define FTPY_OFF_T_PYFMT "L"
    #else
        #error Unsupported size for type off_t
    #endif
#endif

FILE* ftpy_PyFile_Dup(PyObject *file, char *mode, ftpy_offset_t *orig_pos);
int ftpy_PyFile_DupClose(PyObject *file, FILE* handle, ftpy_offset_t orig_pos);
int ftpy_PyFile_Check(PyObject *file);
PyObject* ftpy_PyFile_OpenFile(PyObject *filename, const char *mode);
int ftpy_PyFile_CloseFile(PyObject *file);

#endif
