# -*- coding: utf-8 -*-

# Copyright (c) 2015, Michael Droettboom
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies,
# either expressed or implied, of the FreeBSD Project.

from __future__ import print_function, unicode_literals, absolute_import

import freetypy as ft
from .util import *


def test_glyph():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER)

    assert glyph.advance == (2176.0, 0.0)
    assert glyph.bitmap_left == 0
    assert glyph.bitmap_top == 36
    assert glyph.format == 1651078259
    assert glyph.linear_hori_advance == 34.2041015625
    assert glyph.linear_vert_advance == 50.0
    assert glyph.lsb_delta == 0
    assert glyph.rsb_delta == 0

    x = glyph.metrics
    assert x.height == 36.0
    assert x.hori_advance == 34.0
    assert x.hori_bearing_x == 0.0
    assert x.hori_bearing_y == 36.0
    assert x.vert_advance == 50.0
    assert x.vert_bearing_x == -17.0
    assert x.vert_bearing_y == 6.0
    assert x.width == 33.0


def test_lookup_by_unicode():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    glyph_macroman = face.get_char_index(0xCB)
    glyph_macroman_wrong = face.get_char_index_unicode('\u00cb')
    glyph_unicode = face.get_char_index_unicode('\u00c0')
    assert glyph_macroman == glyph_unicode
    assert glyph_macroman != glyph_macroman_wrong
    assert glyph_macroman == 173

    face.set_charmap(1)
    glyph_unicode_wrong = face.get_char_index(0xCB)
    glyph_unicode_right = face.get_char_index(0xC0)
    glyph_unicode_right2 = face.get_char_index_unicode('\u00c0')

    assert glyph_unicode_wrong != glyph_unicode
    assert glyph_unicode_right == glyph_unicode
    assert glyph_unicode_right2 == glyph_unicode
    assert glyph_unicode == 173


def test_subglyphs():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)

    # TODO: Vera doesn't seem to have any subglyphs, so
    # we can't use it to test this

    for charcode, glyph_index in face.get_chars():
        glyph = face.load_glyph(glyph_index)
        if glyph.format == ft.GLYPH_FORMAT.COMPOSITE:
            print(glyph_index)


def test_glyph_reload():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER)

    assert glyph.linear_hori_advance == 34.2041015625

    glyph2 = face.load_char(ord('l'), ft.LOAD.RENDER)

    assert glyph.linear_hori_advance == 34.2041015625
    assert glyph2.linear_hori_advance == 13.8916015625

    del face

    assert glyph.linear_hori_advance == 34.2041015625


def test_instantiate_Glyph():
    try:
        x = ft.Glyph()
    except RuntimeError:
        pass
    else:
        assert False, "Shouldn't be able to directly instantiate a Glyph"
