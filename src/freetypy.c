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


PyObject *freetypy_module;


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
    if (ftpy_exc(FT_Init_FreeType(&ft_library))) {
       INITERROR;
    }

#if PY3K
    freetypy_module = PyModule_Create(&moduledef);
#else
    freetypy_module = Py_InitModule3(
        "_freetypy", module_methods,
        "Freetype bindings");
#endif

    if (freetypy_module == NULL) {
        INITERROR;
    }

    {
        FT_Int major, minor, patch;
        char version_string[64];

        FT_Library_Version(ft_library, &major, &minor, &patch);
        sprintf(version_string, "%d.%d.%d", major, minor, patch);
        if (PyModule_AddStringConstant(
                freetypy_module, "__freetype_version__", version_string)) {
            INITERROR;
        }
    }

    if (setup_pyutil(freetypy_module) ||
        setup_constants(freetypy_module) ||
        setup_version(freetypy_module) ||
        setup_errors() ||
        setup_BBox(freetypy_module) ||
        setup_Bitmap(freetypy_module) ||
        setup_Bitmap_Size(freetypy_module) ||
        setup_CharIter(freetypy_module) ||
        setup_CharMap(freetypy_module) ||
        setup_Face(freetypy_module) ||
        setup_Glyph(freetypy_module) ||
        setup_Glyph_Metrics(freetypy_module) ||
        setup_Layout(freetypy_module) ||
        setup_Matrix(freetypy_module) ||
        setup_Outline(freetypy_module) ||
        setup_SfntName(freetypy_module) ||
        setup_SfntNames(freetypy_module) ||
        setup_Size(freetypy_module) ||
        setup_Size_Metrics(freetypy_module) ||
        setup_SubGlyph(freetypy_module) ||
        setup_SubGlyphs(freetypy_module) ||
        setup_TrueType(freetypy_module) ||
        setup_TT_Header(freetypy_module) ||
        setup_TT_HoriHeader(freetypy_module) ||
        setup_TT_OS2(freetypy_module) ||
        setup_TT_Pclt(freetypy_module) ||
        setup_TT_Postscript(freetypy_module) ||
        setup_TT_VertHeader(freetypy_module) ||
        setup_Vector(freetypy_module)
        )
        INITERROR;

    #if PY3K
    return freetypy_module;
    #endif
}
