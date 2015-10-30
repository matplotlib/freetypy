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


def test_layout():
    face = ft.Face(vera_path())
    face.select_charmap(ft.ENCODING.UNICODE)
    face.set_char_size(24.0)

    layout = ft.Layout(
        face, "The quick brown fox jumped over the lazy dog")

    assert layout.ink_bbox.width == 553.984375
    assert layout.ink_bbox.ascent == 18.0
    assert layout.ink_bbox.depth == -5.0

    assert tuple(layout.layout_bbox)[:3] == (0.0, -6.0, 555.984375)

    assert layout.glyph_indices.to_list() == [
        55, 75, 72, 3, 84, 88, 76, 70, 78, 3, 69, 85, 82, 90, 81, 3,
        73, 82, 91, 3, 77, 88, 80, 83, 72, 71, 3, 82, 89, 72, 85, 3,
        87, 75, 72, 3, 79, 68, 93, 92, 3, 71, 82, 74]


@raises(ValueError)
def test_layout_wrong_encoding():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(24.0)

    layout = ft.Layout(
        face, "The quick brown fox jumped over the lazy dog")
