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
from freetypy import subset
from .util import *

import io


def test_subset():
    face = ft.Face(vera_path())
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char_unicode('B')
    original_B = glyph.outline.to_string(' M ', ' L ', ' C ', ' Q ')

    with open(vera_path(), 'rb') as input_fd:
        output_fd = io.BytesIO()
        subset.subset_font(input_fd, output_fd, 'ABCD')

    output_fd.seek(0)

    face = ft.Face(output_fd)
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char_unicode('B')
    s = glyph.outline.to_string(' M ', ' L ', ' C ', ' Q ')
    assert original_B == s

    # Not here
    glyph = face.load_char_unicode('X')
    s = glyph.outline.to_string(' M ', ' L ', ' C ', ' Q ')
    assert len(s) == 0
