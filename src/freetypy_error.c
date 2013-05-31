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

#include "freetypy_error.h"


typedef struct
{
    FT_Error code;
    char *msg;
    PyObject *cls;
} error_def;


static error_def errors[83];
static const unsigned int nerrors = 83;


static int error_finder(const void *key, const void *member)
{
    FT_Error err = *((FT_Error *)key);
    error_def *entry = (error_def *)member;

    if (entry->code == err) {
        return 0;
    } else if (entry->code < err) {
        return 1;
    } else {
        return -1;
    }
}


int ftpy_exc(FT_Error error)
{
    error_def *entry;

    if (error) {
        entry = (error_def *)bsearch(
            &error, errors, nerrors, sizeof(error_def), error_finder);
        if (entry == NULL) {
            PyErr_Format(
                PyExc_RuntimeError, "unknown freetype exception %x", error);
            return 1;
        } else {
            PyErr_Format(entry->cls, "freetype error: %s", entry->msg);
            return 1;
        }
    }

    return 0;
}

#define DEF_ERROR(c, m, x)                                        \
    errors[i].code = c; errors[i].msg = m; errors[i].cls = x; ++i;

int setup_errors() {
    unsigned int i = 0;

    DEF_ERROR(0x00, "no error", PyExc_Exception);
    DEF_ERROR(0x01, "cannot open resource", PyExc_IOError);
    DEF_ERROR(0x02, "unknown file format", PyExc_ValueError);
    DEF_ERROR(0x03, "broken file", PyExc_IOError);
    DEF_ERROR(0x04, "invalid FreeType version", PyExc_RuntimeError);
    DEF_ERROR(0x05, "module version is too low", PyExc_RuntimeError);
    DEF_ERROR(0x06, "invalid argument", PyExc_ValueError);
    DEF_ERROR(0x07, "unimplemented feature", PyExc_NotImplementedError);
    DEF_ERROR(0x08, "broken table", PyExc_ValueError);
    DEF_ERROR(0x09, "broken offset within table", PyExc_ValueError);
    DEF_ERROR(0x10, "invalid glyph index", PyExc_IndexError);
    DEF_ERROR(0x11, "invalid character code", PyExc_ValueError);
    DEF_ERROR(0x12, "unsupported glyph image format", PyExc_ValueError);
    DEF_ERROR(0x13, "cannot render this glyph format", PyExc_NotImplementedError);
    DEF_ERROR(0x14, "invalid outline", PyExc_ValueError);
    DEF_ERROR(0x15, "invalid composite glyph", PyExc_ValueError);
    DEF_ERROR(0x16, "too many hints", PyExc_ValueError);
    DEF_ERROR(0x17, "invalid pixel size", PyExc_ValueError);
    DEF_ERROR(0x20, "invalid object handle", PyExc_ValueError);
    DEF_ERROR(0x21, "invalid library handle", PyExc_ValueError);
    DEF_ERROR(0x22, "invalid module handle", PyExc_ValueError);
    DEF_ERROR(0x23, "invalid face handle", PyExc_ValueError);
    DEF_ERROR(0x24, "invalid size handle", PyExc_ValueError);
    DEF_ERROR(0x25, "invalid glyph slot handle", PyExc_ValueError);
    DEF_ERROR(0x26, "invalid charmap handle", PyExc_ValueError);
    DEF_ERROR(0x27, "invalid cache manager handle", PyExc_ValueError);
    DEF_ERROR(0x28, "invalid stream handle", PyExc_ValueError);
    DEF_ERROR(0x30, "too many modules", PyExc_MemoryError);
    DEF_ERROR(0x31, "too many extensions", PyExc_MemoryError);
    DEF_ERROR(0x40, "out of memory", PyExc_MemoryError);
    DEF_ERROR(0x41, "unlisted object", PyExc_KeyError);
    DEF_ERROR(0x51, "cannot open stream", PyExc_IOError);
    DEF_ERROR(0x52, "invalid stream seek", PyExc_IOError);
    DEF_ERROR(0x53, "invalid stream skip", PyExc_IOError);
    DEF_ERROR(0x54, "invalid stream read", PyExc_IOError);
    DEF_ERROR(0x55, "invalid stream operation", PyExc_IOError);
    DEF_ERROR(0x56, "invalid frame operation", PyExc_IOError);
    DEF_ERROR(0x57, "nested frame access", PyExc_IOError);
    DEF_ERROR(0x58, "invalid frame read", PyExc_IOError);
    DEF_ERROR(0x60, "raster uninitialized", PyExc_ValueError);
    DEF_ERROR(0x61, "raster corrupted", PyExc_ValueError);
    DEF_ERROR(0x62, "raster overflow", PyExc_OverflowError);
    DEF_ERROR(0x63, "negative height while rastering", PyExc_ValueError);
    DEF_ERROR(0x70, "too many registered caches", PyExc_MemoryError);
    DEF_ERROR(0x80, "invalid opcode", PyExc_ValueError);
    DEF_ERROR(0x81, "too few arguments", PyExc_TypeError);
    DEF_ERROR(0x82, "stack overflow", PyExc_RuntimeError);
    DEF_ERROR(0x83, "code overflow", PyExc_OverflowError);
    DEF_ERROR(0x84, "bad argument", PyExc_TypeError);
    DEF_ERROR(0x85, "division by zero", PyExc_ZeroDivisionError);
    DEF_ERROR(0x86, "invalid reference", PyExc_ValueError);
    DEF_ERROR(0x87, "found debug opcode", PyExc_SyntaxError);
    DEF_ERROR(0x88, "found ENDF opcode in execution stream", PyExc_SyntaxError);
    DEF_ERROR(0x89, "nested DEFS", PyExc_SyntaxError);
    DEF_ERROR(0x8A, "invalid code range", PyExc_ValueError);
    DEF_ERROR(0x8B, "execution context too long", PyExc_RuntimeError);
    DEF_ERROR(0x8C, "too many function definitions", PyExc_MemoryError);
    DEF_ERROR(0x8D, "too many instruction definitions", PyExc_MemoryError);
    DEF_ERROR(0x8E, "SFNT font table missing", PyExc_AttributeError);
    DEF_ERROR(0x8F, "horizontal header (hhea) table missing", PyExc_AttributeError);
    DEF_ERROR(0x90, "locations (loca) table missing", PyExc_AttributeError);
    DEF_ERROR(0x91, "name table missing", PyExc_AttributeError);
    DEF_ERROR(0x92, "character map (cmap) table missing", PyExc_AttributeError);
    DEF_ERROR(0x93, "horizontal metrics (hmtx) table missing", PyExc_AttributeError);
    DEF_ERROR(0x94, "PostScript (post) table missing", PyExc_AttributeError);
    DEF_ERROR(0x95, "invalid horizontal metrics", PyExc_ValueError);
    DEF_ERROR(0x96, "invalid character map (cmap) format", PyExc_ValueError);
    DEF_ERROR(0x97, "invalid ppem value", PyExc_ValueError);
    DEF_ERROR(0x98, "invalid vertical metrics", PyExc_ValueError);
    DEF_ERROR(0x99, "could not find context", PyExc_AttributeError);
    DEF_ERROR(0x9A, "invalid PostScript (post, table format", PyExc_ValueError);
    DEF_ERROR(0x9B, "invalid PostScript (post, table", PyExc_ValueError);
    DEF_ERROR(0xA0, "opcode syntax error", PyExc_SyntaxError);
    DEF_ERROR(0xA1, "argument stack underflow", PyExc_RuntimeError);
    DEF_ERROR(0xA2, "ignore", PyExc_Exception);
    DEF_ERROR(0xB0, "`STARTFONT` field missing", PyExc_KeyError);
    DEF_ERROR(0xB1, "`FONT` field missing", PyExc_KeyError);
    DEF_ERROR(0xB2, "`SIZE` field missing", PyExc_KeyError);
    DEF_ERROR(0xB3, "`CHARS` field missing", PyExc_KeyError);
    DEF_ERROR(0xB4, "`STARTCHAR` field missing", PyExc_KeyError);
    DEF_ERROR(0xB5, "`ENCODING` field missing", PyExc_KeyError);
    DEF_ERROR(0xB6, "`BBX` field missing", PyExc_KeyError);
    DEF_ERROR(0xB7, "`BBX` too big", PyExc_MemoryError);

    return 0;
}
