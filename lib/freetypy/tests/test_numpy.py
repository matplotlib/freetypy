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

import freetypy as ft
from .util import *


try:
    import numpy as np
except ImportError:
    HAS_NUMPY = False
else:
    HAS_NUMPY = True


@skip_if(not HAS_NUMPY)
def test_glyph():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER)

    array = np.array(glyph.bitmap)
    assert array.shape == (36, 33)
    assert array.dtype == np.uint8


@skip_if(not HAS_NUMPY)
def test_lcd_glyph():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER|ft.LOAD.TARGET_LCD)

    array = np.array(glyph.bitmap)
    assert array.shape == (36, 33, 3)
    assert array.dtype == np.uint8


@skip_if(not HAS_NUMPY)
def test_lcd_mono():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER|ft.LOAD.TARGET_MONO)

    array = np.array(glyph.bitmap)
    assert array.shape == (36, 5)
    assert array.dtype == np.uint8

    array = np.array(glyph.bitmap.convert())
    assert array.shape == (36, 33)
    assert array.dtype == np.uint8


@skip_if(not HAS_NUMPY)
def test_bitmap_stickaround():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER|ft.LOAD.TARGET_LCD)

    bitmap = glyph.bitmap

    del glyph
    del face

    array = np.array(bitmap)
    assert array.shape == (36, 33, 3)
    assert array.dtype == np.uint8


@skip_if(not HAS_NUMPY)
def test_glyph_render_after():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65, ft.LOAD.RENDER)
    array1 = np.array(glyph.bitmap)

    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65)
    glyph.render()
    array2 = np.array(glyph.bitmap)

    assert np.all(array1 == array2)

    old_bitmap = glyph.bitmap
    new_bitmap = glyph.render()

    array3 = np.array(old_bitmap)
    array4 = np.array(new_bitmap)

    assert np.all(array1 == array3)
    assert np.all(array3 == array4)


@skip_if(not HAS_NUMPY)
def test_outline():
    face = ft.Face(vera_path())
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(65)

    points = np.array(glyph.outline.points)
    assert points.shape == (11, 2)

    tags = np.array(glyph.outline.tags)
    assert tags.shape == (11,)

    contours = np.array(glyph.outline.contours)
    assert contours.shape == (2,)
    assert list(contours) == [2, 10]
