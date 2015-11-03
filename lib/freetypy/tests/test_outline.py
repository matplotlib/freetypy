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


def test_outline():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(ord('B'))

    assert glyph.outline.contours.to_list() == [8, 17, 32]
    assert glyph.outline.tags.to_list() == [
        61, 17, 9, 0, 0, 9, 0, 0, 9, 25, 17, 9, 0, 0, 9, 0, 0, 9, 25,
        9, 0, 0, 9, 0, 0, 25, 0, 0, 9, 0, 0, 25, 1]
    assert glyph.outline.points.to_list() == [
        [640, 1088], [640, 256], [1130, 256], [1402, 256], [1664,
        461], [1664, 673], [1664, 886], [1402, 1088], [1130, 1088],
        [640, 2048], [640, 1344], [1093, 1344], [1317, 1344], [1536,
        1518], [1536, 1696], [1536, 1873], [1317, 2048], [1093, 2048],
        [320, 2304], [1115, 2304], [1471, 2304], [1856, 2005], [1856,
        1729], [1856, 1516], [1656, 1263], [1462, 1232], [1710, 1183],
        [1984, 872], [1984, 640], [1984, 334], [1548, 0], [1146, 0],
        [320, 0]]
    assert glyph.outline.flags == 1


def test_outline_decompose():
    class Decomposer(object):
        def __init__(self):
            self.entries = []

        def move_to(self, point):
            self.entries.append(('move_to', point))

        def line_to(self, point):
            self.entries.append(('line_to', point))

        def conic_to(self, a, b):
            self.entries.append(('conic_to', a, b))

        def cubic_to(self, a, b, c):
            self.entries.append(('cubic_to', a, b, c))

    d = Decomposer()

    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(ord('B'))

    glyph.outline.decompose(d)

    print(d.entries)

    assert d.entries == [
        ('move_to', (10.0, 17.0)),
        ('line_to', (10.0, 4.0)),
        ('line_to', (17.65625, 4.0)),
        ('conic_to', (21.90625, 4.0), (23.953125, 5.59375)),
        ('conic_to', (26.0, 7.203125), (26.0, 10.515625)),
        ('conic_to', (26.0, 13.84375), (23.953125, 15.421875)),
        ('conic_to', (21.90625, 17.0), (17.65625, 17.0)),
        ('line_to', (10.0, 17.0)),
        ('move_to', (10.0, 32.0)),
        ('line_to', (10.0, 21.0)),
        ('line_to', (17.078125, 21.0)),
        ('conic_to', (20.578125, 21.0), (22.28125, 22.359375)),
        ('conic_to', (24.0, 23.71875), (24.0, 26.5)),
        ('conic_to', (24.0, 29.265625), (22.28125, 30.625)),
        ('conic_to', (20.578125, 32.0), (17.078125, 32.0)),
        ('line_to', (10.0, 32.0)),
        ('move_to', (5.0, 36.0)),
        ('line_to', (17.421875, 36.0)),
        ('conic_to', (22.984375, 36.0), (25.984375, 33.65625)),
        ('conic_to', (29.0, 31.328125), (29.0, 27.015625)),
        ('conic_to', (29.0, 23.6875), (27.4375, 21.703125)),
        ('conic_to', (25.875, 19.734375), (22.84375, 19.25)),
        ('conic_to', (26.71875, 18.484375), (28.859375, 16.046875)),
        ('conic_to', (31.0, 13.625), (31.0, 10.0)),
        ('conic_to', (31.0, 5.21875), (27.59375, 2.609375)),
        ('conic_to', (24.1875, 0.0), (17.90625, 0.0)),
        ('line_to', (5.0, 0.0)),
        ('line_to', (5.0, 36.0))
    ]

def test_outline_to_string():
    face = ft.Face(vera_path())
    face.set_charmap(0)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(ord('B'))

    s = glyph.outline.to_string(' M ', ' L ', ' C ', ' Q ')

    print(s)

    assert s == b'10 17 M 10 4 L 17.65625 4 L 21.90625 4 23.953125 5.59375 Q 26 7.203125 26 10.515625 Q 26 13.84375 23.953125 15.421875 Q 21.90625 17 17.65625 17 Q 10 17 L 10 32 M 10 21 L 17.078125 21 L 20.578125 21 22.28125 22.359375 Q 24 23.71875 24 26.5 Q 24 29.265625 22.28125 30.625 Q 20.578125 32 17.078125 32 Q 10 32 L 5 36 M 17.421875 36 L 22.984375 36 25.984375 33.65625 Q 29 31.328125 29 27.015625 Q 29 23.6875 27.4375 21.703125 Q 25.875 19.734375 22.84375 19.25 Q 26.71875 18.484375 28.859375 16.046875 Q 31 13.625 31 10 Q 31 5.21875 27.59375 2.609375 Q 24.1875 0 17.90625 0 Q 5 0 L 5 36 L '

    s = glyph.outline.to_string(' M ', ' L ', ' C ', ' Q ', prefix=True)

    print(s)

    assert s == b' M 10 17 L 10 4 L 17.65625 4 Q 21.90625 4 23.953125 5.59375 Q 26 7.203125 26 10.515625 Q 26 13.84375 23.953125 15.421875 Q 21.90625 17 17.65625 17 L 10 17 M 10 32 L 10 21 L 17.078125 21 Q 20.578125 21 22.28125 22.359375 Q 24 23.71875 24 26.5 Q 24 29.265625 22.28125 30.625 Q 20.578125 32 17.078125 32 L 10 32 M 5 36 L 17.421875 36 Q 22.984375 36 25.984375 33.65625 Q 29 31.328125 29 27.015625 Q 29 23.6875 27.4375 21.703125 Q 25.875 19.734375 22.84375 19.25 Q 26.71875 18.484375 28.859375 16.046875 Q 31 13.625 31 10 Q 31 5.21875 27.59375 2.609375 Q 24.1875 0 17.90625 0 L 5 0 L 5 36'
