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

import datetime

import freetypy as ft
from .util import *


def test_constant_magic():
    face = ft.Face(vera_path())

    sfntnames = face.sfnt_names

    assert repr(sfntnames[0].platform_id) == "freetypy.TT_PLATFORM.MACINTOSH"
    assert sfntnames[0].platform_id == ft.TT_PLATFORM.MACINTOSH
    assert 'freetypy.TT_PLATFORM.Constant' in str(type(sfntnames[0].platform_id))

    assert repr(sfntnames[0].encoding_id) == "freetypy.TT_MAC_ID.ROMAN"
    assert sfntnames[0].encoding_id == ft.TT_MAC_ID.ROMAN

    assert repr(sfntnames[0].name_id) == 'freetypy.TT_NAME_ID.COPYRIGHT'

    assert type(sfntnames[0].string) == type('')
    assert type(sfntnames[0].string_bytes) == type(b'')


def test_bitflag_magic():
    face = ft.Face(vera_path())
    assert repr(face.face_flags) == "freetypy.FACE_FLAG.EXTERNAL_STREAM | freetypy.FACE_FLAG.GLYPH_NAMES | freetypy.FACE_FLAG.HINTER | freetypy.FACE_FLAG.HORIZONTAL | freetypy.FACE_FLAG.KERNING | freetypy.FACE_FLAG.SCALABLE | freetypy.FACE_FLAG.SFNT"
    assert face.face_flags == (ft.FACE_FLAG.SCALABLE | ft.FACE_FLAG.EXTERNAL_STREAM | ft.FACE_FLAG.SFNT | ft.FACE_FLAG.KERNING | ft.FACE_FLAG.HORIZONTAL | ft.FACE_FLAG.GLYPH_NAMES | ft.FACE_FLAG.HINTER)
