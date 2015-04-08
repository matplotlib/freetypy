# -*- coding: utf-8 -*-

# Copyright (c) 2013, Michael Droettboom All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# The views and conclusions contained in the software and
# documentation are those of the authors and should not be interpreted
# as representing official policies, either expressed or implied, of
# the FreeBSD Project.

from __future__ import print_function, unicode_literals, absolute_import

TT_Header__init__ = """
TrueType header.
"""

TT_Header_table_version = """
The version of the TrueType header.
"""

TT_Header_font_revision = """
The version of the font.  Set by the font manufacturer.
"""

TT_Header_checksum_adjust = """
A checksum for the header.
"""

TT_Header_magic_number = """
Always 0x5F0F3CF5.
"""

TT_Header_flags = """
`TT_HEADER_FLAGS` bitflags that apply to the face as a whole.
"""

TT_HEADER_FLAGS = """
|freetypy| Bitflags giving global information about the font.

- `BASELINE_AT_ZERO`: If set, the baselines for the font is at *y* = 0
  (that is, the *x*-axis)

- `LEFT_BLACK_BIT_IS_LSB`: If set, the *x*-position of the leftmost
  black bit is assumed to be the left side bearing.

- `NONLINEAR_SCALE`: If set, instructions may use point size
  explicitly in place of pixels per em. Scaled point size and actual
  point size will differ (i.e. 24 point glyph differs from 12 point
  glyph scaled by factor of 2).

- `INTEGER_SCALING`: Use integer scaling instead of fractional.

- `VERTICAL`: Set in fonts that are intended to ne laid out
  vertically, and in which the glyphs have been drawn such that an
  *x*-coordinate of 0 corresponds to the desired vertical baseline.

- `REQUIRES_LAYOUT`: Set if the font requires layout for correct
  linguistic rendering (e.g. Arabic fonts).

- `METAMORPHOSIS_EFFECTS`: Set for a GX font which has one or more
  metamorphosis effects designated as happening by default.

- `RIGHT_TO_LEFT_GLYPHS`: Set if the font contains any strong
  right-to-left glyphs.

- `INDIC_REARRANGEMENT_EFFECTS`: Set if the font contains Indic-style
  rearrangement effects.
"""

TT_Header_units_per_em = """
Units per em.  In the range 64 - 16384.
"""

TT_Header_created = """
The date the font was created.
"""

TT_Header_modified = """
The date the font was modified.
"""

TT_Header_x_min = """
The minimum x for all bounding boxes.
"""

TT_Header_y_min = """
The minimum y for all bounding boxes.
"""

TT_Header_x_max = """
The maximum x for all bounding boxes.
"""

TT_Header_y_max = """
The maximum y for all bounding boxes.
"""

TT_Header_mac_style = """
`TT_MAC_STYLE` bitflags.
"""

TT_MAC_STYLE = """
Bit flags indicating the style of the face.

- `BOLD`

- `ITALIC`

- `UNDERLINE`

- `OUTLINE`

- `SHADOW`

- `CONDENSED`

- `EXTENDED`
"""

TT_Header_lowest_rec_ppem = """
The smallest readable size in pixels.
"""

TT_Header_font_direction = """
The direction of the font.

- 0: Mixed directional glyphs

- 1: Only strong left to right glyphs

- 2: Like 1, but also contains neutrals

- -1: Only strong right to left glyphs

- -2: Like -1, but also contains neutrals
"""
