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

#include "simple_layout.h"

#define FROM_FT_FIXED(v) (((double)(v) / (double)(1 << 16)))


FT_Error ftpy_calculate_simple_layout(
    FT_Face face, FT_Int32 load_flags, const uint32_t *text, size_t text_length,
    ftpy_Layout *layout)
{
    FT_ULong charcode;
    FT_UInt glyph_index, previous_glyph_index;
    int use_kerning = 1;
    FT_Vector pen;
    FT_Vector delta;
    FT_Glyph glyph;
    FT_BBox glyph_bbox;
    size_t i;
    FT_Error status = 1;

    layout->glyph_indices = NULL;
    layout->xys = NULL;

    layout->glyph_indices = calloc(sizeof(FT_ULong), text_length);
    if (layout->glyph_indices == NULL) {
        goto exit;
    }

    layout->xys = calloc(sizeof(FT_Vector), text_length);
    if (layout->xys == NULL) {
        goto exit;
    }

    layout->size = text_length;

    layout->bbox.xMin = layout->bbox.yMin = 0;
    layout->bbox.xMax = layout->bbox.yMax = 0;

    use_kerning = FT_HAS_KERNING(face);
    pen.x = 0;
    pen.y = 0;
    previous_glyph_index = 0;

    for (i = 0; i < text_length; ++i) {
        /* TODO: This assumes the font encoding is Unicode */
        charcode = text[i];

        glyph_index = FT_Get_Char_Index(face, charcode);
        if (use_kerning && previous_glyph_index && glyph_index) {
            FT_Get_Kerning(
                face, previous_glyph_index, glyph_index,
                /* TODO: Choose the right kerning mode based on the other flags
                   */
                FT_KERNING_DEFAULT, &delta);
            pen.x += delta.x;
        }

        layout->glyph_indices[i] = glyph_index;

        status = FT_Load_Glyph(face, glyph_index, load_flags);
        if (status) {
            goto exit;
        }

        status = FT_Get_Glyph(face->glyph, &glyph);
        if (status) {
            goto exit;
        }

        layout->xys[i].x = FROM_FT_FIXED(pen.x);
        layout->xys[i].y = FROM_FT_FIXED(pen.y);

        /* TODO: Bounding box should be calculated from advance width */
        FT_Outline_Get_BBox(&((FT_OutlineGlyph)glyph)->outline, &glyph_bbox);
        glyph_bbox.xMin += pen.x >> 10;
        glyph_bbox.yMin += pen.y >> 10;
        glyph_bbox.xMax += pen.x >> 10;
        glyph_bbox.yMax += pen.y >> 10;
        if (glyph_bbox.xMin < layout->bbox.xMin) layout->bbox.xMin = glyph_bbox.xMin;
        if (glyph_bbox.yMin < layout->bbox.yMin) layout->bbox.yMin = glyph_bbox.yMin;
        if (glyph_bbox.xMax > layout->bbox.xMax) layout->bbox.xMax = glyph_bbox.xMax;
        if (glyph_bbox.yMax > layout->bbox.yMax) layout->bbox.yMax = glyph_bbox.yMax;

        pen.x += glyph->advance.x;

        previous_glyph_index = glyph_index;
    }

    status = 0;

 exit:

    if (status != 0) {
        free(layout->glyph_indices);
        layout->glyph_indices = NULL;
        free(layout->xys);
        layout->xys = NULL;
    }

    return status;
}
