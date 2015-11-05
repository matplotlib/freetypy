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

#include "freetypy_error.h"


typedef struct
{
    FT_Error code;
    char *msg;
    PyObject *cls;
} error_def;


static error_def errors[256];


int ftpy_exc(FT_Error error)
{
    error_def *entry;

    if (error) {
        entry = &errors[error];
        if (entry->code == 0) {
            PyErr_Format(
                PyExc_RuntimeError, "Unknown FreeType exception %x", error);
            return 1;
        } else {
            PyErr_Format(entry->cls, "FreeType error %x: %s", error, entry->msg);
            return 1;
        }
    }

    return 0;
}

#define DEF_ERROR(c, ignored, m, x) \
    errors[c].code = c; errors[c].msg = m; errors[c].cls = x;

int setup_errors(void) {
    memset(errors, 0, sizeof(error_def) * 256);

    DEF_ERROR(FT_Err_Ok, 0x00, "no error", PyExc_Exception);

    DEF_ERROR(FT_Err_Cannot_Open_Resource, 0x01, "cannot open resource", PyExc_IOError);
    DEF_ERROR(FT_Err_Unknown_File_Format, 0x02, "unknown file format", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_File_Format, 0x03, "broken file", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Version, 0x04, "invalid FreeType version", PyExc_RuntimeError);
    DEF_ERROR(FT_Err_Lower_Module_Version, 0x05, "module version is too low", PyExc_RuntimeError);
    DEF_ERROR(FT_Err_Invalid_Argument, 0x06, "invalid argument", PyExc_ValueError);
    DEF_ERROR(FT_Err_Unimplemented_Feature, 0x07, "unimplemented feature", PyExc_NotImplementedError);
    DEF_ERROR(FT_Err_Invalid_Table, 0x08, "broken table", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Offset, 0x09, "broken offset within table", PyExc_ValueError);
    DEF_ERROR(FT_Err_Array_Too_Large, 0x0A, "array allocation size too large", PyExc_MemoryError);
    /* DEF_ERROR(FT_Err_Missing_Module, 0x0B, "missing module", PyExc_RuntimeError); */
    /* DEF_ERROR(FT_Err_Missing_Property, 0x0C, "missing property", PyExc_KeyError); */

    /* glyph/character errors */

    DEF_ERROR(FT_Err_Invalid_Glyph_Index, 0x10, "invalid glyph index", PyExc_IndexError);
    DEF_ERROR(FT_Err_Invalid_Character_Code, 0x11, "invalid character code", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Glyph_Format, 0x12, "unsupported glyph image format", PyExc_ValueError);
    DEF_ERROR(FT_Err_Cannot_Render_Glyph, 0x13, "cannot render this glyph format", PyExc_RuntimeError);
    DEF_ERROR(FT_Err_Invalid_Outline, 0x14, "invalid outline", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Composite, 0x15, "invalid composite glyph", PyExc_ValueError);
    DEF_ERROR(FT_Err_Too_Many_Hints, 0x16, "too many hints", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Pixel_Size, 0x17, "invalid pixel size", PyExc_ValueError);

    /* handle errors */

    DEF_ERROR(FT_Err_Invalid_Handle, 0x20, "invalid object handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Library_Handle, 0x21, "invalid library handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Driver_Handle, 0x22, "invalid module handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Face_Handle, 0x23, "invalid face handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Size_Handle, 0x24, "invalid size handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Slot_Handle, 0x25, "invalid glyph slot handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_CharMap_Handle, 0x26, "invalid charmap handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Cache_Handle, 0x27, "invalid cache manager handle", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Stream_Handle, 0x28, "invalid stream handle", PyExc_ValueError);

    /* driver errors */

    DEF_ERROR(FT_Err_Too_Many_Drivers, 0x30, "too many modules", PyExc_ValueError);
    DEF_ERROR(FT_Err_Too_Many_Extensions, 0x31, "too many extensions", PyExc_ValueError);

    /* memory errors */

    DEF_ERROR(FT_Err_Out_Of_Memory, 0x40, "out of memory", PyExc_MemoryError);
    DEF_ERROR(FT_Err_Unlisted_Object, 0x41, "unlisted object", PyExc_MemoryError);

    /* stream errors */

    DEF_ERROR(FT_Err_Cannot_Open_Stream, 0x51, "cannot open stream", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Stream_Seek, 0x52, "invalid stream seek", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Stream_Skip, 0x53, "invalid stream skip", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Stream_Read, 0x54, "invalid stream read", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Stream_Operation, 0x55, "invalid stream operation", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Frame_Operation, 0x56, "invalid frame operation", PyExc_IOError);
    DEF_ERROR(FT_Err_Nested_Frame_Access, 0x57, "nested frame access", PyExc_IOError);
    DEF_ERROR(FT_Err_Invalid_Frame_Read, 0x58, "invalid frame read", PyExc_IOError);

    /* raster errors */

    DEF_ERROR(FT_Err_Raster_Uninitialized, 0x60, "raster uninitialized", PyExc_ValueError);
    DEF_ERROR(FT_Err_Raster_Corrupted, 0x61, "raster corrupted", PyExc_ValueError);
    DEF_ERROR(FT_Err_Raster_Overflow, 0x62, "raster overflow", PyExc_ValueError);
    DEF_ERROR(FT_Err_Raster_Negative_Height, 0x63, "negative height while rastering", PyExc_ValueError);

    /* cache errors */

    DEF_ERROR(FT_Err_Too_Many_Caches, 0x70, "too many registered caches", PyExc_MemoryError);

    /* TrueType and SFNT errors */

    DEF_ERROR(FT_Err_Invalid_Opcode, 0x80, "invalid opcode", PyExc_ValueError);
    DEF_ERROR(FT_Err_Too_Few_Arguments, 0x81, "too few arguments", PyExc_ValueError);
    DEF_ERROR(FT_Err_Stack_Overflow, 0x82, "stack overflow", PyExc_ValueError);
    DEF_ERROR(FT_Err_Code_Overflow, 0x83, "code overflow", PyExc_ValueError);
    DEF_ERROR(FT_Err_Bad_Argument, 0x84, "bad argument", PyExc_ValueError);
    DEF_ERROR(FT_Err_Divide_By_Zero, 0x85, "division by zero", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Reference, 0x86, "invalid reference", PyExc_ValueError);
    DEF_ERROR(FT_Err_Debug_OpCode, 0x87, "found debug opcode", PyExc_ValueError);
    DEF_ERROR(FT_Err_ENDF_In_Exec_Stream, 0x88, "found ENDF opcode in execution stream", PyExc_ValueError);
    DEF_ERROR(FT_Err_Nested_DEFS, 0x89, "nested DEFS", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_CodeRange, 0x8A, "invalid code range", PyExc_ValueError);
    DEF_ERROR(FT_Err_Execution_Too_Long, 0x8B, "execution context too long", PyExc_ValueError);
    DEF_ERROR(FT_Err_Too_Many_Function_Defs, 0x8C, "too many function definitions", PyExc_ValueError);
    DEF_ERROR(FT_Err_Too_Many_Instruction_Defs, 0x8D, "too many instruction definitions", PyExc_ValueError);
    DEF_ERROR(FT_Err_Table_Missing, 0x8E, "SFNT font table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Horiz_Header_Missing, 0x8F, "horizontal header (FT_Err_hhea) table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Locations_Missing, 0x90, "locations (FT_Err_loca) table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Name_Table_Missing, 0x91, "name table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_CMap_Table_Missing, 0x92, "character map (FT_Err_cmap) table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Hmtx_Table_Missing, 0x93, "horizontal metrics (FT_Err_hmtx) table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Post_Table_Missing, 0x94, "PostScript (FT_Err_post) table missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Horiz_Metrics, 0x95, "invalid horizontal metrics", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_CharMap_Format, 0x96, "invalid character map (FT_Err_cmap) format", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_PPem, 0x97, "invalid ppem value", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Vert_Metrics, 0x98, "invalid vertical metrics", PyExc_ValueError);
    DEF_ERROR(FT_Err_Could_Not_Find_Context, 0x99, "could not find context", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Post_Table_Format, 0x9A, "invalid PostScript (FT_Err_post) table format", PyExc_ValueError);
    DEF_ERROR(FT_Err_Invalid_Post_Table, 0x9B, "invalid PostScript (FT_Err_post) table", PyExc_ValueError);

    /* CFF, CID, and Type 1 errors */

    DEF_ERROR(FT_Err_Syntax_Error, 0xA0, "opcode syntax error", PyExc_ValueError);
    DEF_ERROR(FT_Err_Stack_Underflow, 0xA1, "argument stack underflow", PyExc_ValueError);
    DEF_ERROR(FT_Err_Ignore, 0xA2, "ignore", PyExc_ValueError);
    DEF_ERROR(FT_Err_No_Unicode_Glyph_Name, 0xA3, "no Unicode glyph name found", PyExc_ValueError);
    /* DEF_ERROR(FT_Err_Glyph_Too_Big, 0xA4, "glyph to big for hinting", PyExc_ValueError); */

    /* BDF errors */

    DEF_ERROR(FT_Err_Missing_Startfont_Field, 0xB0, "`STARTFONT' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Font_Field, 0xB1, "`FONT' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Size_Field, 0xB2, "`SIZE' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Fontboundingbox_Field, 0xB3, "`FONTBOUNDINGBOX' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Chars_Field, 0xB4, "`CHARS' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Startchar_Field, 0xB5, "`STARTCHAR' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Encoding_Field, 0xB6, "`ENCODING' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Missing_Bbx_Field, 0xB7, "`BBX' field missing", PyExc_ValueError);
    DEF_ERROR(FT_Err_Bbx_Too_Big, 0xB8, "`BBX' too big", PyExc_ValueError);
    DEF_ERROR(FT_Err_Corrupted_Font_Header, 0xB9, "Font header corrupted or missing fields", PyExc_ValueError);
    DEF_ERROR(FT_Err_Corrupted_Font_Glyphs, 0xBA, "Font glyphs corrupted or missing !fields", PyExc_ValueError);

    return 0;
}
