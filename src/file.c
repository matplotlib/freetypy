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

#include "file.h"

/****************************************************************************
 This file contains a bunch of functionality to handle Python
 file-like objects in a way that is consistent between Python 2.x and
 3.x.
*/


/*
 * PyFile_* compatibility
 */
#if PY3K

/*
 * Get a FILE* handle to the file represented by the Python object
 */
FILE *ftpy_PyFile_Dup(PyObject *file, char *mode, ftpy_offset_t *orig_pos)
{
    int fd, fd2;
    PyObject *ret, *os;
    ftpy_offset_t pos;
    FILE *handle;
    /* Flush first to ensure things end up in the file in the correct order */
    ret = PyObject_CallMethod(file, (char *)"flush", (char *)"");
    if (ret == NULL) {
        return NULL;
    }
    Py_DECREF(ret);
    fd = PyObject_AsFileDescriptor(file);
    if (fd == -1) {
        return NULL;
    }

    /* The handle needs to be dup'd because we have to call fclose
       at the end */
    os = PyImport_ImportModule("os");
    if (os == NULL) {
        return NULL;
    }
    ret = PyObject_CallMethod(os, (char *)"dup", (char *)"i", fd);
    Py_DECREF(os);
    if (ret == NULL) {
        return NULL;
    }
    fd2 = PyNumber_AsSsize_t(ret, NULL);
    Py_DECREF(ret);

/* Convert to FILE* handle */
#ifdef _WIN32
    handle = _fdopen(fd2, mode);
#else
    handle = fdopen(fd2, mode);
#endif
    if (handle == NULL) {
        PyErr_SetString(PyExc_IOError,
                        "Getting a FILE* from a Python file object failed");
    }

    /* Record the original raw file handle position */
    *orig_pos = ftpy_ftell(handle);
    if (*orig_pos == -1) {
        // handle is a stream, so we don't have to worry about this
        return handle;
    }

    /* Seek raw handle to the Python-side position */
    ret = PyObject_CallMethod(file, (char *)"tell", (char *)"");
    if (ret == NULL) {
        fclose(handle);
        return NULL;
    }
    pos = PyNumber_AsSsize_t(ret, PyExc_OverflowError);
    Py_DECREF(ret);
    if (PyErr_Occurred()) {
        fclose(handle);
        return NULL;
    }

    if (ftpy_fseek(handle, pos, SEEK_SET) == -1) {
        PyErr_SetString(PyExc_IOError, "seeking file failed");
        return NULL;
    }

    return handle;
}

/*
 * Close the dup-ed file handle, and seek the Python one to the current position
 */
int ftpy_PyFile_DupClose(PyObject *file, FILE* handle, ftpy_offset_t orig_pos)
{
    int fd;
    PyObject *ret;
    ftpy_offset_t position;

    position = ftpy_ftell(handle);
    fclose(handle);

    /* Restore original file handle position, in order to not confuse
       Python-side data structures */
    fd = PyObject_AsFileDescriptor(file);
    if (fd == -1) {
        return -1;
    }

    if (ftpy_lseek(fd, orig_pos, SEEK_SET) != -1) {
        if (position == -1) {
            PyErr_SetString(PyExc_IOError, "obtaining file position failed");
            return -1;
        }

        /* Seek Python-side handle to the FILE* handle position */
        ret = PyObject_CallMethod(file, (char *)"seek", (char *)(FTPY_OFF_T_PYFMT "i"), position, 0);
        if (ret == NULL) {
            return -1;
        }
        Py_DECREF(ret);
    }

    return 0;
}

int ftpy_PyFile_Check(PyObject *file)
{
    int fd;
    fd = PyObject_AsFileDescriptor(file);
    if (fd == -1) {
        PyErr_Clear();
        return 0;
    }
    return 1;
}

#else

/* Python 2.x */

FILE* ftpy_PyFile_Dup(PyObject *file, char *mode, ftpy_offset_t *orig_pos)
{
    return PyFile_AsFile(file);
}

int ftpy_PyFile_DupClose(PyObject *file, FILE* handle, ftpy_offset_t orig_pos)
{
    // deliberately nothing
    return 0;
}

int ftpy_PyFile_Check(PyObject *file)
{
    return PyFile_Check(file);
}

#endif

PyObject* ftpy_PyFile_OpenFile(PyObject *filename, const char *mode)
{
    PyObject *open;
    PyObject *builtins;

    builtins = PyEval_GetBuiltins();
    if (builtins == NULL) {
        return NULL;
    }

    open = PyDict_GetItemString(builtins, "open");
    if (open == NULL) {
        PyErr_SetString(PyExc_AttributeError,
                        "Internal error: could not get open function");
        return NULL;
    }

    return PyObject_CallFunction(open, "Os", filename, mode);
}

int ftpy_PyFile_CloseFile(PyObject *file)
{
    PyObject *ret;

    ret = PyObject_CallMethod(file, "close", NULL);
    if (ret == NULL) {
        return -1;
    }
    Py_DECREF(ret);
    return 0;
}
