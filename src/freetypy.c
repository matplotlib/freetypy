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

#include "freetypy.h"
#include "doc/freetypy.h"

#include "bbox.h"
#include "bitmap.h"
#include "bitmap_size.h"
#include "chariter.h"
#include "charmap.h"
#include "constants.h"
#include "face.h"
#include "glyph.h"
#include "glyph_metrics.h"
#include "layout.h"
#include "matrix.h"
#include "outline.h"
#include "sfntname.h"
#include "sfntnames.h"
#include "size.h"
#include "size_metrics.h"
#include "subglyph.h"
#include "subglyphs.h"
#include "truetype.h"
#include "tt_header.h"
#include "tt_horiheader.h"
#include "tt_os2.h"
#include "tt_pclt.h"
#include "tt_postscript.h"
#include "tt_vertheader.h"
#include "vector.h"
#include "version.h"


static FT_Library ft_library;

FT_Library get_ft_library()
{
    return ft_library;
}

static PyMethodDef module_methods[] = {
    {NULL}  /* Sentinel */
};


/* TODO: Hide all exported symbols in the shared library except this one */

#if PY3K
    static void
    freetypy_module_dealloc(PyObject *m)
    {
        FT_Done_FreeType(ft_library);
    }

    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_freetypy",
        NULL,
        0,
        module_methods,
        NULL,
        NULL,
        NULL,
        (freefunc)freetypy_module_dealloc
    };

    #define INITERROR return NULL

    PyMODINIT_FUNC
    PyInit__freetypy(void)
#else
    #define INITERROR return

    PyMODINIT_FUNC
    init_freetypy(void)
#endif
{
    PyObject* m;

    if (ftpy_exc(FT_Init_FreeType(&ft_library))) {
       INITERROR;
    }

#if PY3K
    m = PyModule_Create(&moduledef);
#else
    m = Py_InitModule3(
        "_freetypy", module_methods,
        "Freetype bindings");
#endif

    if (m == NULL) {
        INITERROR;
    }

    if (setup_pyutil(m) ||
        setup_constants(m) ||
        setup_version(m) ||
        setup_errors() ||
        setup_BBox(m) ||
        setup_Bitmap(m) ||
        setup_Bitmap_Size(m) ||
        setup_CharIter(m) ||
        setup_CharMap(m) ||
        setup_Face(m) ||
        setup_Glyph(m) ||
        setup_Glyph_Metrics(m) ||
        setup_Layout(m) ||
        setup_Matrix(m) ||
        setup_Outline(m) ||
        setup_SfntName(m) ||
        setup_SfntNames(m) ||
        setup_Size(m) ||
        setup_Size_Metrics(m) ||
        setup_SubGlyph(m) ||
        setup_SubGlyphs(m) ||
        setup_TrueType(m) ||
        setup_TT_Header(m) ||
        setup_TT_HoriHeader(m) ||
        setup_TT_OS2(m) ||
        setup_TT_Pclt(m) ||
        setup_TT_Postscript(m) ||
        setup_TT_VertHeader(m) ||
        setup_Vector(m)
        )
        INITERROR;

    #if PY3K
    return m;
    #endif
}
