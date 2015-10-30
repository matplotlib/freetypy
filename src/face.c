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

#include "face.h"
#include "doc/face.h"

#include "bbox.h"
#include "bitmap_size.h"
#include "chariter.h"
#include "charmap.h"
#include "constants.h"
#include "encoding.h"
#include "file.h"
#include "glyph.h"
#include "sfntnames.h"
#include "size.h"
#include "tt_header.h"
#include "tt_horiheader.h"
#include "tt_os2.h"
#include "tt_postscript.h"
#include "tt_pclt.h"
#include "tt_vertheader.h"
#include "vector.h"

#include FT_TYPE1_TABLES_H


#define MAKE_FACE_GETTER(name, convert_func, member)    \
    MAKE_GETTER(Py_Face, name, convert_func, member)
#define DEF_FACE_GETTER(name) DEF_GETTER(name, doc_Face_ ## name)
#define FACE_METHOD(name) DEF_METHOD(name, Face)
#define FACE_METHOD_NOARGS(name) DEF_METHOD_NOARGS(name, Face)


ftpy_BitflagType Py_FT_FACE_FLAG_BitflagType;
ftpy_BitflagType Py_FT_STYLE_FLAG_BitflagType;
ftpy_BitflagType Py_FT_LOAD_BitflagType;
ftpy_ConstantType Py_FT_SIZE_REQUEST_TYPE_ConstantType;
ftpy_BitflagType Py_FT_FSTYPE_BitflagType;
ftpy_ConstantType Py_FT_KERNING_ConstantType;


/****************************************************************************
 Face stream handling
*/


typedef struct
{
    PyObject *py_file;
    FILE *fp;
    int close_file;
} py_file_def;


static unsigned long read_from_file_callback(
    FT_Stream stream, unsigned long offset, unsigned char *buffer,
    unsigned long count) {

    py_file_def *def = (py_file_def *)stream->descriptor.pointer;

    if (fseek(def->fp, offset, SEEK_SET) == -1) {
        return 0;
    }

    if (count > 0) {
        return fread(buffer, 1, count, def->fp);
    }

    return 0;
}


static void close_file_callback(FT_Stream stream)
{
    py_file_def *def = (py_file_def *)stream->descriptor.pointer;

    ftpy_PyFile_DupClose(def->py_file, def->fp);

    if (def->close_file) {
        ftpy_PyFile_CloseFile(def->py_file);
    }

    Py_DECREF(def->py_file);
}


/*
   Helper function to convert a Python object, which may be a file
   path, a "real" Python file object or a Python readable file-like
   object to an FT_Open_Args structure.
   */
static int _py_file_to_open_args(
    PyObject *py_file_arg,
    FT_Open_Args *open_args,
    Py_Face *face)
{
    PyObject *py_file = NULL;
    int close_file = 0;
    FILE *fp;
    PyObject *data = NULL;
    char *data_ptr;
    Py_ssize_t data_len;
    py_file_def *stream_info = NULL;
    long file_size;
    void *new_memory;
    PyObject *read_string = NULL;

    int result = -1;

    memset((void *)open_args, 0, sizeof(FT_Open_Args));

    if (PyBytes_Check(py_file_arg) || PyUnicode_Check(py_file_arg)) {
        if ((py_file = ftpy_PyFile_OpenFile(py_file_arg, (char *)"rb")) == NULL) {
            goto exit;
        }
        close_file = 1;
    } else {
        Py_INCREF(py_file_arg);
        py_file = py_file_arg;
    }

    if ((fp = ftpy_PyFile_Dup(py_file, (char *)"rb"))) {
        stream_info = PyMem_Malloc(sizeof(py_file_def));
        if (stream_info == NULL) {
            goto exit;
        }
        memset(stream_info, 0, sizeof(py_file_def));

        Py_INCREF(py_file);
        stream_info->py_file = py_file;
        stream_info->close_file = close_file;
        stream_info->fp = fp;
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        face->stream.base = NULL;
        face->stream.size = (unsigned long)file_size;
        face->stream.pos = 0;
        face->stream.descriptor.pointer = stream_info;
        face->stream.read = &read_from_file_callback;
        face->stream.close = &close_file_callback;

        open_args->flags = FT_OPEN_STREAM;
        open_args->stream = &face->stream;

        result = 0;
        goto exit;
    }

    PyErr_Clear();

    read_string = PyUnicode_FromString("read");
    if (read_string == NULL) {
        goto exit;
    }

    if (PyObject_HasAttrString(py_file_arg, "read")) {
        data = PyObject_CallMethodObjArgs(py_file_arg, read_string, NULL);
        if (data == NULL) {
            goto exit;
        }

        if (PyBytes_AsStringAndSize(data, &data_ptr, &data_len)) {
            goto exit;
        }

        if (face->mem) {
            free(face->mem);
        }
        face->mem = PyMem_Malloc(face->mem_size + data_len);
        if (face->mem == NULL) {
            goto exit;
        }
        new_memory = face->mem + face->mem_size;
        face->mem_size += data_len;

        memcpy(new_memory, data_ptr, data_len);
        open_args->flags = FT_OPEN_MEMORY;
        open_args->memory_base = new_memory;
        open_args->memory_size = data_len;
        open_args->stream = NULL;

        result = 0;
        goto exit;
    }

 exit:

    if (result && !PyErr_Occurred()) {
        PyErr_SetString(
            PyExc_TypeError,
            "First argument must be a path or file object reading bytes");
    }

    Py_XDECREF(read_string);
    Py_XDECREF(py_file);
    Py_XDECREF(data);

    return result;
}


/****************************************************************************
 Object basics
*/


static void
Py_Face_dealloc(Py_Face* self)
{
    FT_Done_Face(self->x);
    Py_XDECREF(self->filename);
    free(self->mem);
    Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject *
Py_Face_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Py_Face *self;

    self = (Py_Face *)ftpy_Object_new(type, args, kwds);
    if (self == NULL) {
        return NULL;
    }
    self->base.owner = NULL;
    self->x = NULL;
    memset(&self->stream, 0, sizeof(FT_StreamRec));
    self->mem = NULL;
    self->mem_size = 0;
    self->filename = NULL;
    return (PyObject *)self;
}


static int
Py_Face_init(Py_Face *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"file", "face_index"};
    PyObject *py_file_arg = NULL;
    long face_index = 0;
    FT_Open_Args open_args;

    int result = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|l:Face.__init__", kwlist,
                                     &py_file_arg,
                                     &face_index)) {
        return -1;
    }

    if (_py_file_to_open_args(py_file_arg, &open_args, self)) {
        goto exit;
    }

    if (ftpy_exc(
            FT_Open_Face(
                get_ft_library(), &open_args, face_index, &self->x))) {
        goto exit;
    }

    Py_INCREF(py_file_arg);
    self->filename = py_file_arg;

    result = 0;

 exit:

    return result;
}


/****************************************************************************
 Getters
*/


MAKE_FACE_GETTER(num_faces, PyLong_FromLong, self->x->num_faces);
MAKE_FACE_GETTER(face_index, PyLong_FromLong, self->x->face_index);


static PyObject *face_flags_get(Py_Face *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_FT_FACE_FLAG_BitflagType, self->x->face_flags);
}


static PyObject *style_flags_get(Py_Face *self, PyObject *closure)
{
    return Py_Constant_cnew(
        &Py_FT_STYLE_FLAG_BitflagType, self->x->style_flags);
}


MAKE_FACE_GETTER(num_glyphs, PyLong_FromLong, self->x->num_glyphs);
MAKE_FACE_GETTER(family_name, ftpy_PyUnicode_FromStringOrNull, self->x->family_name);
MAKE_FACE_GETTER(style_name, ftpy_PyUnicode_FromStringOrNull, self->x->style_name);


static PyObject *available_sizes_get(Py_Face *self, PyObject *closure)
{
    PyObject *result = NULL;
    PyObject *list = NULL;
    PyObject *item;
    int size = self->x->num_fixed_sizes;
    int i;

    if (!(list = PyList_New(size))) {
        goto exit;
    }

    for (i = 0; i < size; ++i) {
        item = Py_Bitmap_Size_cnew(
            &(self->x->available_sizes[i]),
            (PyObject *)self);
        if (item == NULL) {
            goto exit;
        }
        PyList_SET_ITEM(list, i, item);
    }

    Py_INCREF(list);
    result = list;

 exit:

    Py_XDECREF(list);
    return result;
}


static PyObject *charmaps_get(Py_Face *self, PyObject *closure)
{
    PyObject *result = NULL;
    PyObject *list = NULL;
    PyObject *item;
    int size = self->x->num_charmaps;
    int i;

    if (!(list = PyList_New(size))) {
        goto exit;
    }

    for (i = 0; i < size; ++i) {
        item = Py_CharMap_cnew(self->x->charmaps[i], (PyObject *)self);
        PyList_SET_ITEM(list, i, item);
    }

    result = list;

 exit:

    return result;
}


static PyObject *bbox_get(Py_Face *self, PyObject *closure)
{
    return Py_BBox_cnew(&self->x->bbox, 1.0);
}


MAKE_FACE_GETTER(units_per_em, PyLong_FromUnsignedLong, self->x->units_per_EM);
MAKE_FACE_GETTER(ascender, PyLong_FromLong, self->x->ascender);
MAKE_FACE_GETTER(descender, PyLong_FromLong, self->x->descender);
MAKE_FACE_GETTER(height, PyLong_FromLong, self->x->height);
MAKE_FACE_GETTER(max_advance_width, PyLong_FromLong, self->x->max_advance_width);
MAKE_FACE_GETTER(max_advance_height, PyLong_FromLong, self->x->max_advance_height);
MAKE_FACE_GETTER(underline_position, PyLong_FromLong, self->x->underline_position);
MAKE_FACE_GETTER(underline_thickness, PyLong_FromLong, self->x->underline_thickness);


static PyObject *glyph_get(Py_Face *self, PyObject *closure)
{
    return Py_Glyph_cnew(self->x->glyph, (PyObject *)self, self->load_flags);
}


static PyObject *size_get(Py_Face *self, PyObject *closure)
{
    return Py_Size_cnew(self->x->size, (PyObject *)self);
}


static PyObject *charmap_get(Py_Face *self, PyObject *closure)
{
    return Py_CharMap_cnew(self->x->charmap, (PyObject *)self);
}


static PyObject *sfnt_names_get(Py_Face *self, PyObject *closure)
{
    return Py_SfntNames_cnew(self);
}


MAKE_FACE_GETTER(has_horizontal, PyBool_FromLong, FT_HAS_HORIZONTAL(self->x))
MAKE_FACE_GETTER(has_vertical, PyBool_FromLong, FT_HAS_VERTICAL(self->x))
MAKE_FACE_GETTER(has_kerning, PyBool_FromLong, FT_HAS_KERNING(self->x))
MAKE_FACE_GETTER(is_scalable, PyBool_FromLong, FT_IS_SCALABLE(self->x))
MAKE_FACE_GETTER(is_sfnt, PyBool_FromLong, FT_IS_SFNT(self->x))
MAKE_FACE_GETTER(is_fixed_width, PyBool_FromLong, FT_IS_FIXED_WIDTH(self->x))


static PyObject *filename_get(Py_Face *self, PyObject *closure)
{
    Py_INCREF(self->filename);
    return self->filename;
}


static PyObject *tt_header_get(Py_Face *self, PyObject *closure)
{
    return Py_TT_Header_cnew((PyObject *)self);
}


static PyObject *tt_horiheader_get(Py_Face *self, PyObject *closure)
{
    return Py_TT_HoriHeader_cnew((PyObject *)self);
}


static PyObject *tt_os2_get(Py_Face *self, PyObject *closure)
{
    return Py_TT_OS2_cnew((PyObject *)self);
}


static PyObject *tt_pclt_get(Py_Face *self, PyObject *closure)
{
    return Py_TT_Pclt_cnew((PyObject *)self);
}


static PyObject *tt_postscript_get(Py_Face *self, PyObject *closure)
{
    return Py_TT_Postscript_cnew((PyObject *)self);
}


static PyObject *tt_vertheader_get(Py_Face *self, PyObject *closure)
{
    return Py_TT_VertHeader_cnew((PyObject *)self);
}


static PyGetSetDef Py_Face_getset[] = {
    DEF_FACE_GETTER(num_faces),
    DEF_FACE_GETTER(face_index),
    DEF_FACE_GETTER(face_flags),
    DEF_FACE_GETTER(style_flags),
    DEF_FACE_GETTER(num_glyphs),
    DEF_FACE_GETTER(family_name),
    DEF_FACE_GETTER(style_name),
    DEF_FACE_GETTER(available_sizes),
    DEF_FACE_GETTER(charmaps),
    DEF_FACE_GETTER(bbox),
    DEF_FACE_GETTER(units_per_em),
    DEF_FACE_GETTER(ascender),
    DEF_FACE_GETTER(descender),
    DEF_FACE_GETTER(height),
    DEF_FACE_GETTER(max_advance_width),
    DEF_FACE_GETTER(max_advance_height),
    DEF_FACE_GETTER(underline_position),
    DEF_FACE_GETTER(underline_thickness),
    DEF_FACE_GETTER(glyph),
    DEF_FACE_GETTER(size),
    DEF_FACE_GETTER(charmap),
    DEF_FACE_GETTER(sfnt_names),
    DEF_FACE_GETTER(has_horizontal),
    DEF_FACE_GETTER(has_vertical),
    DEF_FACE_GETTER(has_kerning),
    DEF_FACE_GETTER(is_scalable),
    DEF_FACE_GETTER(is_sfnt),
    DEF_FACE_GETTER(is_fixed_width),
    DEF_FACE_GETTER(filename),
    DEF_FACE_GETTER(tt_header),
    DEF_FACE_GETTER(tt_horiheader),
    DEF_FACE_GETTER(tt_os2),
    DEF_FACE_GETTER(tt_pclt),
    DEF_FACE_GETTER(tt_postscript),
    DEF_FACE_GETTER(tt_vertheader),
    {NULL}
};


/****************************************************************************
 Methods
*/


static PyObject*
Py_Face_attach(Py_Face *self, PyObject *args, PyObject *kwds)
{
    PyObject *py_file_arg = NULL;
    FT_Open_Args open_args;

    static char *kwlist[] = {"file", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:attach", kwlist,
                                     &py_file_arg)) {
        return NULL;
    }

    if (_py_file_to_open_args(py_file_arg, &open_args, self)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Attach_Stream(self->x, &open_args))) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject*
Py_Face_get_chars(Py_Face *self, PyObject *args, PyObject *kwds)
{
    return Py_CharIter_cnew((PyObject *)self);
}


static PyObject*
Py_Face_get_char_index(Py_Face *self, PyObject *args, PyObject *kwds)
{
    unsigned long charcode;
    unsigned int glyph_index;

    static char *kwlist[] = {"charcode", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "k:get_char_index", kwlist,
            &charcode)) {
        return NULL;
    }

    glyph_index = FT_Get_Char_Index(self->x, charcode);

    return PyLong_FromUnsignedLong(glyph_index);
}


static PyObject*
Py_Face_get_char_index_unicode(Py_Face *self, PyObject *args, PyObject *kwds)
{
    PyObject *py_unicode;
    unsigned long charcode;
    unsigned int glyph_index;

    static char *kwlist[] = {"charcode", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "O:get_char_index_unicode", kwlist,
            &py_unicode)) {
        return NULL;
    }

    if (ftpy_get_charcode_from_unicode(
            py_unicode,
            self->x->charmap->platform_id,
            self->x->charmap->encoding_id,
            &charcode)) {
        return NULL;
    }

    glyph_index = FT_Get_Char_Index(self->x, charcode);

    return PyLong_FromUnsignedLong(glyph_index);
}


static PyObject*
Py_Face_get_char_name(Py_Face *self, PyObject *args, PyObject *kwds)
{
    unsigned long charcode;
    unsigned int glyph_index;
    char glyph_name[80];

    static char *kwlist[] = {"charcode", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "k:get_char_name_unicode", kwlist,
            &charcode)) {
        return NULL;
    }

    glyph_index = FT_Get_Char_Index(self->x, charcode);

    if (glyph_index != 0 &&
        !FT_Get_Glyph_Name(self->x, glyph_index, glyph_name, 80)) {
        return PyUnicode_FromString(glyph_name);
    }

    return PyUnicode_FromFormat("uni%08x", charcode);
}


static PyObject*
Py_Face_get_char_variant_index(Py_Face *self, PyObject *args, PyObject *kwds)
{
    unsigned long charcode;
    unsigned long variantSelector;
    unsigned int glyph_index;

    static char *kwlist[] = {"charcode", "variantSelector", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "kk:get_char_variant_index", kwlist,
            &charcode, &variantSelector)) {
        return NULL;
    }

    glyph_index = FT_Face_GetCharVariantIndex(self->x, charcode, variantSelector);

    return PyLong_FromUnsignedLong(glyph_index);
}


static PyObject*
Py_Face_get_fstype_flags(Py_Face *self, PyObject *args, PyObject *kwds)
{
    unsigned short flags;

    flags = FT_Get_FSType_Flags(self->x);

    return Py_Constant_cnew(&Py_FT_FSTYPE_BitflagType, flags);
}


static PyObject*
Py_Face_get_glyph_name(Py_Face *self, PyObject *args, PyObject *kwds)
{
    unsigned int glyph_index;
    char glyph_name[80];

    static char *kwlist[] = {"glyph_index", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "I:get_glyph_name", kwlist,
            &glyph_index)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Get_Glyph_Name(self->x, glyph_index, glyph_name, 80))) {
        return NULL;
    }

    return PyUnicode_FromString(glyph_name);
}


static PyObject*
Py_Face_get_kerning(Py_Face *self, PyObject *args, PyObject *kwds)
{
    unsigned int left_glyph;
    unsigned int right_glyph;
    unsigned int kern_mode = FT_KERNING_DEFAULT;
    FT_Vector akerning;

    static char *kwlist[] = {"left_glyph", "right_glyph", "kern_mode", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "II|I:get_kerning", kwlist,
            &left_glyph, &right_glyph, &kern_mode)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Get_Kerning(self->x, left_glyph, right_glyph, kern_mode,
                           &akerning))) {
        return NULL;
    }

    return Py_Vector_cnew(&akerning, 1 << 6);
}


static PyObject*
Py_Face_get_name_index(Py_Face *self, PyObject *args, PyObject *kwds)
{
    char *name;
    unsigned int glyph_index;

    static char *kwlist[] = {"glyph_name", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "es:get_name_index", kwlist,
            "ascii", &name)) {
        return NULL;
    }

    glyph_index = FT_Get_Name_Index(self->x, name);

    return PyLong_FromUnsignedLong(glyph_index);
}


static PyObject*
Py_Face_get_postscript_name(Py_Face *self, PyObject *args, PyObject *kwds)
{
    const char *postscript_name;

    postscript_name = FT_Get_Postscript_Name(self->x);

    if (postscript_name == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No postscript name found.");
        return NULL;
    }

    return PyUnicode_FromString(postscript_name);
}


static PyObject*
Py_Face_get_track_kerning(Py_Face *self, PyObject *args, PyObject *kwds)
{
    double point_size;
    int degree;
    FT_Fixed akerning;
    double kerning;

    static char *kwlist[] = {"point_size", "degree", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "di:get_track_kerning", kwlist,
            &point_size, &degree)) {
        return NULL;
    }


    if (ftpy_exc(
            FT_Get_Track_Kerning(self->x, TO_FT_FIXED(point_size), degree,
                                 &akerning))) {
        return NULL;
    }

    kerning = FROM_FT_FIXED(akerning);
    return PyFloat_FromDouble(kerning);
}


static PyObject*
Py_Face_has_ps_glyph_names(Py_Face* self, PyObject* args, PyObject* kwds)
{
    int has_ps_glyph_names;

    has_ps_glyph_names = FT_Has_PS_Glyph_Names(self->x);

    return PyBool_FromLong(has_ps_glyph_names);
}


static PyObject*
Py_Face_load_char(Py_Face* self, PyObject* args, PyObject* kwds) {
    unsigned long charcode = 0;
    int load_flags = FT_LOAD_DEFAULT;

    const char* keywords[] = {"charcode", "load_flags", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "K|i:load_char", (char **)keywords,
            &charcode, &load_flags)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Load_Char(self->x, charcode, load_flags))) {
        return NULL;
    }

    self->load_flags = load_flags;

    return Py_Glyph_cnew(self->x->glyph, (PyObject *)self, load_flags);
}


static PyObject*
Py_Face_load_char_unicode(Py_Face* self, PyObject* args, PyObject* kwds) {
    PyObject *py_unicode = NULL;
    int load_flags = FT_LOAD_DEFAULT;
    unsigned long charcode = 0;

    const char* keywords[] = {"charcode", "load_flags", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "O|i:load_char_unicode", (char **)keywords,
            &py_unicode, &load_flags)) {
        return NULL;
    }

    if (ftpy_get_charcode_from_unicode(
            py_unicode,
            self->x->charmap->platform_id,
            self->x->charmap->encoding_id,
            &charcode)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Load_Char(self->x, charcode, load_flags))) {
        return NULL;
    }

    self->load_flags = load_flags;

    return Py_Glyph_cnew(self->x->glyph, (PyObject *)self, load_flags);
}


static PyObject*
Py_Face_load_glyph(Py_Face* self, PyObject* args, PyObject* kwds) {
    unsigned int glyph_index = 0;
    int load_flags = FT_LOAD_DEFAULT;

    const char* keywords[] = {"glyph_index", "load_flags", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "I|i:load_glyph", (char **)keywords,
            &glyph_index, &load_flags)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Load_Glyph(self->x, glyph_index, load_flags))) {
        return NULL;
    }

    self->load_flags = load_flags;

    return Py_Glyph_cnew(self->x->glyph, (PyObject *)self, load_flags);
}


static PyObject*
Py_Face_request_size(Py_Face* self, PyObject* args, PyObject* kwds) {
    int type = FT_SIZE_REQUEST_TYPE_NOMINAL;
    double width = 0;
    double height = 0;
    unsigned int horiResolution = 0;
    unsigned int vertResolution = 0;
    long width_fixed;
    long height_fixed;
    FT_Size_RequestRec request;

    const char* keywords[] = {"type", "width", "height", "horiResolution",
                              "vertResolution"};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "i|ddII:request_size", (char **)keywords,
            &type, &width, &height, &horiResolution, &vertResolution)) {
        return NULL;
    }

    if (horiResolution == 0) {
        width_fixed = (long)(width * (1L << 6));
    } else {
        width_fixed = (long)width;
    }

    if (vertResolution == 0) {
        height_fixed = (long)(height * (1L << 6));
    } else {
        height_fixed = (long)height;
    }

    request.type = type;
    request.width = width_fixed;
    request.height = height_fixed;
    request.horiResolution = horiResolution;
    request.vertResolution = vertResolution;

    if (ftpy_exc(
            FT_Request_Size(self->x, &request))) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject*
Py_Face_select_charmap(Py_Face* self, PyObject* args, PyObject* kwds) {
    int encoding = 0;

    const char* keywords[] = {"encoding", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "i:select_charmap", (char **)keywords,
            &encoding)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Select_Charmap(self->x, encoding))) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject*
Py_Face_select_size(Py_Face* self, PyObject* args, PyObject* kwds) {
    int strike_index = 0;

    const char* keywords[] = {"strike_index", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "i:select_size", (char **)keywords,
            &strike_index)) {
        return NULL;
    }

    if (ftpy_exc(
            FT_Select_Size(self->x, strike_index))) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject*
Py_Face_set_charmap(Py_Face* self, PyObject* args, PyObject* kwds) {

    unsigned long map = 0;

    const char* keywords[] = {"charmap", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "k:set_charmap", (char **)keywords,
            &map)) {
        return NULL;
    }

    if (map > self->x->num_charmaps) {
        PyErr_Format(
            PyExc_ValueError,
            "%lu is greater than the number of charmaps in the face (%d)",
            map, self->x->num_charmaps);
        return NULL;
    }

    if (ftpy_exc(
            FT_Set_Charmap(self->x, self->x->charmaps[map]))) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject*
Py_Face_set_char_size(Py_Face* self, PyObject* args, PyObject* kwds) {
    double char_width = 0;
    double char_height = 0;
    unsigned int horz_resolution = 0;
    unsigned int vert_resolution = 0;

    const char* keywords[] = {"char_width", "char_height", "horz_resolution",
                              "vert_resolution", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "|ddII:set_char_size", (char **)keywords,
            &char_width, &char_height, &horz_resolution,
            &vert_resolution)) {
        return NULL;
    }

    if (char_width == 0.0 && char_height == 0.0) {
        char_width = 12.0;
    }

    if (ftpy_exc(
            FT_Set_Char_Size(
                self->x, TO_F26DOT6(char_width), TO_F26DOT6(char_height),
                horz_resolution, vert_resolution))) {
        return NULL;
    }

    Py_RETURN_NONE;
}


static PyObject*
Py_Face_set_transform(Py_Face* self, PyObject* args, PyObject* kwds) {
    double xx = 1.0;
    double xy = 0.0;
    double yx = 0.0;
    double yy = 1.0;
    double x = 0.0;
    double y = 0.0;

    FT_Matrix matrix;
    FT_Vector delta;

    const char* keywords[] = {"matrix", "delta", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "|((dd)(dd))(dd):set_transform", (char **)keywords,
            &xx, &xy, &yx, &yy, &x, &y)) {
        return NULL;
    }

    matrix.xx = TO_FT_FIXED(xx);
    matrix.xy = TO_FT_FIXED(xy);
    matrix.yx = TO_FT_FIXED(yx);
    matrix.yy = TO_FT_FIXED(yy);
    delta.x = TO_FT_FIXED(x);
    delta.x = TO_FT_FIXED(x);

    FT_Set_Transform(self->x, &matrix, &delta);

    Py_RETURN_NONE;
}


static PyMethodDef Py_Face_methods[] = {
    FACE_METHOD(attach),
    FACE_METHOD(get_char_index),
    FACE_METHOD(get_char_index_unicode),
    FACE_METHOD(get_char_name),
    FACE_METHOD(get_char_variant_index),
    FACE_METHOD_NOARGS(get_chars),
    FACE_METHOD_NOARGS(get_fstype_flags),
    FACE_METHOD(get_glyph_name),
    FACE_METHOD(get_kerning),
    FACE_METHOD(get_name_index),
    FACE_METHOD_NOARGS(get_postscript_name),
    FACE_METHOD(get_track_kerning),
    FACE_METHOD_NOARGS(has_ps_glyph_names),
    FACE_METHOD(load_char),
    FACE_METHOD(load_char_unicode),
    FACE_METHOD(load_glyph),
    FACE_METHOD(request_size),
    FACE_METHOD(select_charmap),
    FACE_METHOD(select_size),
    FACE_METHOD(set_charmap),
    FACE_METHOD(set_char_size),
    FACE_METHOD(set_transform),
    {NULL}  /* Sentinel */
};


/****************************************************************************
 repr
*/


static PyObject *
Py_Face_repr(Py_Face *self)
{
    return PyUnicode_FromFormat("<freetypy.Face '%s'>", self->x->family_name);
}


/****************************************************************************
 Setup
*/


PyTypeObject Py_Face_Type;


static PyTypeObject Py_FT_FACE_FLAG_Type;
#define FACE_FLAG_CONST(name) DEF_CONST(name, FT_FACE_FLAG)
static constant_def FT_FACE_FLAG_constants[] = {
    FACE_FLAG_CONST(SCALABLE),
    FACE_FLAG_CONST(FIXED_SIZES),
    FACE_FLAG_CONST(FIXED_WIDTH),
    FACE_FLAG_CONST(SFNT),
    FACE_FLAG_CONST(HORIZONTAL),
    FACE_FLAG_CONST(VERTICAL),
    FACE_FLAG_CONST(KERNING),
    FACE_FLAG_CONST(MULTIPLE_MASTERS),
    FACE_FLAG_CONST(GLYPH_NAMES),
    FACE_FLAG_CONST(EXTERNAL_STREAM),
    FACE_FLAG_CONST(HINTER),
    FACE_FLAG_CONST(CID_KEYED),
    FACE_FLAG_CONST(TRICKY),
    {NULL}
};


static PyTypeObject Py_FT_STYLE_FLAG_Type;
#define STYLE_FLAG_CONST(name) DEF_CONST(name, FT_STYLE_FLAG)
static constant_def FT_STYLE_FLAG_constants[] = {
    STYLE_FLAG_CONST(ITALIC),
    STYLE_FLAG_CONST(BOLD),
    {NULL}
};


static PyTypeObject Py_FT_LOAD_Type;
#define LOAD_CONST(name) DEF_CONST(name, FT_LOAD)
static constant_def FT_LOAD_constants[] = {
    LOAD_CONST(DEFAULT),
    LOAD_CONST(NO_SCALE),
    LOAD_CONST(NO_HINTING),
    LOAD_CONST(RENDER),
    LOAD_CONST(NO_BITMAP),
    LOAD_CONST(VERTICAL_LAYOUT),
    LOAD_CONST(FORCE_AUTOHINT),
    LOAD_CONST(CROP_BITMAP),
    LOAD_CONST(PEDANTIC),
    LOAD_CONST(IGNORE_GLOBAL_ADVANCE_WIDTH),
    LOAD_CONST(NO_RECURSE),
    LOAD_CONST(IGNORE_TRANSFORM),
    LOAD_CONST(MONOCHROME),
    LOAD_CONST(LINEAR_DESIGN),
    LOAD_CONST(NO_AUTOHINT),
    LOAD_CONST(TARGET_NORMAL),
    LOAD_CONST(TARGET_LIGHT),
    LOAD_CONST(TARGET_MONO),
    LOAD_CONST(TARGET_LCD),
    LOAD_CONST(TARGET_LCD_V),
    {NULL}
};


static PyTypeObject Py_FT_SIZE_REQUEST_TYPE_Type;
#define SIZE_REQUEST_TYPE_CONST(name) DEF_CONST(name, FT_SIZE_REQUEST_TYPE)
static constant_def FT_SIZE_REQUEST_TYPE_constants[] = {
    SIZE_REQUEST_TYPE_CONST(NOMINAL),
    SIZE_REQUEST_TYPE_CONST(REAL_DIM),
    SIZE_REQUEST_TYPE_CONST(BBOX),
    SIZE_REQUEST_TYPE_CONST(CELL),
    SIZE_REQUEST_TYPE_CONST(SCALES),
    {NULL}
};


static PyTypeObject Py_FT_FSTYPE_Type;
#define FSTYPE_CONST(name) DEF_CONST(name, FT_FSTYPE)
static constant_def FT_FSTYPE_constants[] = {
    FSTYPE_CONST(INSTALLABLE_EMBEDDING),
    FSTYPE_CONST(RESTRICTED_LICENSE_EMBEDDING),
    FSTYPE_CONST(PREVIEW_AND_PRINT_EMBEDDING),
    FSTYPE_CONST(EDITABLE_EMBEDDING),
    FSTYPE_CONST(NO_SUBSETTING),
    FSTYPE_CONST(BITMAP_EMBEDDING_ONLY),
    {NULL}
};


static PyTypeObject Py_FT_KERNING_Type;
#define KERNING_CONST(name) DEF_CONST(name, FT_KERNING)
static constant_def FT_KERNING_constants[] = {
    KERNING_CONST(DEFAULT),
    KERNING_CONST(UNFITTED),
    KERNING_CONST(UNSCALED),
    {NULL}
};


int setup_Face(PyObject *m)
{
    memset(&Py_Face_Type, 0, sizeof(PyTypeObject));
    Py_Face_Type = (PyTypeObject) {
        .tp_name = "freetypy.Face",
        .tp_basicsize = sizeof(Py_Face),
        .tp_dealloc = (destructor)Py_Face_dealloc,
        .tp_doc = doc_Face__init__,
        .tp_repr = (unaryfunc)Py_Face_repr,
        .tp_methods = Py_Face_methods,
        .tp_getset = Py_Face_getset,
        .tp_init = (initproc)Py_Face_init,
        .tp_new = Py_Face_new
    };

    ftpy_setup_type(m, &Py_Face_Type);

    if (define_bitflag_namespace(
            m, &Py_FT_FACE_FLAG_Type, &Py_FT_FACE_FLAG_BitflagType,
            "freetypy.FACE_FLAG",
            doc_FACE_FLAG, FT_FACE_FLAG_constants) ||

        define_bitflag_namespace(
            m, &Py_FT_STYLE_FLAG_Type, &Py_FT_STYLE_FLAG_BitflagType,
            "freetypy.STYLE_FLAG",
            doc_STYLE_FLAG, FT_STYLE_FLAG_constants) ||

        define_bitflag_namespace(
            m, &Py_FT_LOAD_Type, &Py_FT_LOAD_BitflagType,
            "freetypy.LOAD",
            doc_LOAD, FT_LOAD_constants) ||

        define_constant_namespace(
            m, &Py_FT_SIZE_REQUEST_TYPE_Type, &Py_FT_SIZE_REQUEST_TYPE_ConstantType,
            "freetypy.SIZE_REQUEST_TYPE",
            doc_SIZE_REQUEST_TYPE, FT_SIZE_REQUEST_TYPE_constants) ||

        define_bitflag_namespace(
            m, &Py_FT_FSTYPE_Type, &Py_FT_FSTYPE_BitflagType,
            "freetypy.FSTYPE",
            doc_FSTYPE, FT_FSTYPE_constants) ||

        define_constant_namespace(
            m, &Py_FT_KERNING_Type, &Py_FT_KERNING_ConstantType,
            "freetypy.KERNING",
            doc_KERNING, FT_KERNING_constants)) {

        return -1;
    }

    return 0;
}
