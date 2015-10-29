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

    assert d.entries == [
        ('move_to', (640.0, 1088.0)),
        ('line_to', (640.0, 256.0)),
        ('line_to', (1130.0, 256.0)),
        ('conic_to', (1402.0, 256.0), (1533.0, 358.0)),
        ('conic_to', (1664.0, 461.0), (1664.0, 673.0)),
        ('conic_to', (1664.0, 886.0), (1533.0, 987.0)),
        ('conic_to', (1402.0, 1088.0), (1130.0, 1088.0)),
        ('line_to', (640.0, 1088.0)),
        ('move_to', (640.0, 2048.0)),
        ('line_to', (640.0, 1344.0)),
        ('line_to',(1093.0, 1344.0)),
        ('conic_to', (1317.0, 1344.0), (1426.0, 1431.0)),
        ('conic_to', (1536.0, 1518.0), (1536.0, 1696.0)),
        ('conic_to', (1536.0, 1873.0), (1426.0, 1960.0)),
        ('conic_to', (1317.0, 2048.0), (1093.0, 2048.0)),
        ('line_to', (640.0, 2048.0)),
        ('move_to', (320.0, 2304.0)),
        ('line_to', (1115.0, 2304.0)),
        ('conic_to', (1471.0, 2304.0), (1663.0, 2154.0)),
        ('conic_to', (1856.0, 2005.0), (1856.0, 1729.0)),
        ('conic_to', (1856.0, 1516.0), (1756.0, 1389.0)),
        ('conic_to', (1656.0, 1263.0), (1462.0, 1232.0)),
        ('conic_to', (1710.0, 1183.0), (1847.0, 1027.0)),
        ('conic_to', (1984.0, 872.0), (1984.0, 640.0)),
        ('conic_to', (1984.0, 334.0), (1766.0, 167.0)),
        ('conic_to', (1548.0, 0.0), (1146.0, 0.0)),
        ('line_to', (320.0, 0.0)),
        ('line_to', (320.0, 2304.0))
        ]
