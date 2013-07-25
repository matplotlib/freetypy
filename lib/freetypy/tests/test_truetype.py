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

import datetime

import freetypy as ft
from .util import *


def test_tt_header():
    face = ft.Face(vera_path())

    x = face.tt_header
    assert x.checksum_adjust == 206572268
    assert x.created == datetime.datetime(2003, 4, 9, 15, 46)
    assert x.flags == 31
    assert x.font_direction == 1
    assert x.font_revision == 2.0
    assert x.glyph_data_format == 0
    assert x.index_to_loc_format == 0
    assert x.lowest_rec_ppem == 8
    assert x.mac_style == 0
    assert x.magic_number == 0x5f0f3cf5
    assert x.modified == datetime.datetime(2003, 4, 16, 1, 51, 13)
    assert x.table_version == 1.0
    assert x.units_per_em == 2048
    assert x.x_max == 2636
    assert x.x_min == -375
    assert x.y_max == 1901
    assert x.y_min == -483


def test_tt_horiheader():
    face = ft.Face(vera_path())

    x = face.tt_horiheader
    assert x.ascender == 1901
    assert x.descender == -483
    assert x.line_gap == 0
    assert x.version == 1.0
    assert x.advance_width_max == 2748
    assert x.caret_offset == 0
    assert x.caret_slope_rise == 1
    assert x.caret_slope_run == 0
    assert x.metric_data_format == 0
    assert x.min_left_side_bearing == -375
    assert x.min_right_side_bearing == -375
    assert x.number_of_hmetrics == 268
    assert x.xmax_extent == 2636


@raises(AttributeError)
def test_tt_vertheader():
    face = ft.Face(vera_path())

    x = face.tt_vertheader


def test_tt_os2():
    face = ft.Face(vera_path())

    x = face.tt_os2
    assert x.vend_id == 'Bits'
    assert x.selection == 64
    assert x.type == 4
    assert x.panose == b'\x02\x0b\x06\x03\x03\x08\x04\x02\x02\x04'
    assert x.cap_height == 0
    assert x.family_class == 8
    assert x.family_subclass == 2
    assert x.typo_ascender == 1556
    assert x.typo_descender == -492
    assert x.typo_line_gap == 410
    assert x.x_height == 0
    assert x.break_char == 0
    assert x.default_char == 0
    assert x.first_char_index == 32
    assert x.last_char_index == 64258
    assert x.max_context == 0
    assert x.weight_class == 400
    assert x.width_class == 5
    assert x.win_ascent == 1901
    assert x.win_descent == 483
    assert x.version == 1
    assert x.x_avg_char_width == 1038
    assert x.y_strikeout_position == 614
    assert x.y_strikeout_size == 143
    assert x.y_subscript_x_offset == 0
    assert x.y_subscript_x_size == 1351
    assert x.y_subscript_y_offset == -446
    assert x.y_subscript_y_size == 1228
    assert x.y_superscript_x_offset == 0
    assert x.y_superscript_x_size == 1351
    assert x.y_superscript_y_offset == 595
    assert x.y_superscript_y_size == 1228


# def test_pclt():
#     face = ft.Face(vera_path())

#     # TODO: There seems to be some weird offset in the PCLT table

#     x = face.get_tt_pclt()
#     make_assert(x)
#     assert False


def test_postscript():
    face = ft.Face(vera_path())

    x = face.tt_postscript
    assert x.format_type == 2.0
    assert x.is_fixed_pitch == False
    assert x.italic_angle == 0.0
    assert x.max_mem_type1 == 0
    assert x.max_mem_type42 == 0
    assert x.min_mem_type1 == 0
    assert x.min_mem_type42 == 0
    assert x.underline_position == -213
    assert x.underline_thickness == 143
