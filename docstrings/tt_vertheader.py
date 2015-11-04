# -*- coding: utf-8 -*-

# Copyright (c) 2015, Michael Droettboom All rights reserved.

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

TT_VertHeader_ascender = """
The font's ascender, i.e., the distance from the baseline to the
top-most of all glyph points found in the font.

This value is invalid in many fonts, as it is usually set by the font
designer, and often reflects only a portion of the glyphs found in the
font (maybe ASCII).

You should use the `Face.get_os2_table().sTypoAscender` instead if you
want the correct one.
"""

TT_VertHeader_descender = """
The font's descender, i.e., the distance from the baseline to the
bottom-most of all glyph points found in the font. It is negative.

This value is invalid in many fonts, as it is usually set by the font
designer, and often reflects only a portion of the glyphs found in the
font (maybe ASCII).

You should use the `Face.get_os2_table().sTypoDescender` if you want
the correct one.
"""

TT_VertHeader_line_gap = """
The font's line gap, i.e., the distance to add to the ascender and
descender to get the BTB, i.e., the baseline-to-baseline distance for
the font.
"""

TT_VertHeader_version = """
The table version.
"""

TT_VertHeader__init__ = """
TrueType vertical header (‘vhea’).
"""

TT_VertHeader_advance_height_max = """
This field is the maximum of all advance heights found in the font. It
can be used to compute the maximum height of an arbitrary string of
text.
"""

TT_VertHeader_caret_offset = """

"""

TT_VertHeader_caret_slope_rise = """
The rise coefficient of the cursor's slope of the cursor
(slope=rise/run).
"""

TT_VertHeader_caret_slope_run = """
The run coefficient of the cursor's slope.
"""

TT_VertHeader_metric_data_format = """
Always 0.
"""

TT_VertHeader_min_bottom_side_bearing = """
The minimum bottom side bearing of all glyphs within the font.
"""

TT_VertHeader_min_top_side_bearing = """
The minimum top side bearing of all glyphs within the font.
"""

TT_VertHeader_number_of_vmetrics = """
Number of VMetrics entries in the ‘hmtx’ table -- this value can be
smaller than the total number of glyphs in the font.
"""

TT_VertHeader_ymax_extent = """
The maximum vertical extent (i.e., the ‘height’ of a glyph's bounding
box) for all glyphs in the font.
"""
