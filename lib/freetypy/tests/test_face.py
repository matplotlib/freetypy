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

import freetypy as ft
from .util import *


def _test_face(face):
    expected_flags = (
        ft.FACE_FLAG.GLYPH_NAMES |
        ft.FACE_FLAG.HINTER |
        ft.FACE_FLAG.HORIZONTAL |
        ft.FACE_FLAG.KERNING |
        ft.FACE_FLAG.SCALABLE |
        ft.FACE_FLAG.SFNT)

    assert face.face_flags & expected_flags == expected_flags
    assert face.num_faces == 1
    assert face.face_index == 0
    try:
        face.num_faces = 2
    except AttributeError:
        pass
    else:
        assert False, "Shouldn't be able to write to freetypy attributes"

    assert face.style_flags == 0

    assert face.num_glyphs == 268

    assert face.family_name == "Bitstream Vera Sans"
    assert isinstance(face.family_name, type(""))
    assert face.style_name == "Roman"
    assert isinstance(face.style_name, type(""))

    assert face.available_sizes == []
    assert face.units_per_em == 2048
    assert face.ascender == 1901
    assert face.descender == -483
    assert face.height == 2384
    assert face.max_advance_width == 2748
    assert face.max_advance_height == 2384
    assert face.underline_position == -284
    assert face.underline_thickness == 143

    assert repr(face) == "<freetypy.Face 'Bitstream Vera Sans'>"


def test_face():
    face = ft.Face(vera_path())
    _test_face(face)


def test_face_from_file_like():
    import io
    buf = io.BytesIO()

    with open(vera_path(), 'rb') as fd:
        buf.write(fd.read())

    buf.seek(0)

    face = ft.Face(buf)

    _test_face(face)


def test_face_set_transform():
    face = ft.Face(vera_path())
    face.set_transform([[2, 0], [0, 2]], [20, 20])

    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, 0)

    assert glyph.advance == (4352, 0)
    assert glyph.advance.x == 4352
    assert glyph.advance.y == 0
    x, y = glyph.advance
    assert x == 4352
    assert y == 0


def test_kerning():
    face = ft.Face(vera_path())
    face.set_char_size(24, 24, 300, 300)
    assert face.face_flags & ft.FACE_FLAG.KERNING
    A = face.get_char_index(ord('A'))
    V = face.get_char_index(ord('V'))
    assert face.get_kerning(A, V, ft.KERNING.UNSCALED) == (-2.046875, 0.0)
    assert face.get_kerning(A, V, ft.KERNING.DEFAULT) == (-6, 0)


def test_get_glyph_name():
    face = ft.Face(vera_path())

    names = (".notdef .null nonmarkingreturn space exclam quotedbl numbersign "
             "dollar percent ampersand quotesingle parenleft").split()

    for i, name in enumerate(names):
        assert face.get_glyph_name(i) == name


def test_char_iter():
    face = ft.Face(vera_path())

    chars = list(face.get_chars())
    assert len(chars) == 256
    assert chars[-1] == (64258, 193)
