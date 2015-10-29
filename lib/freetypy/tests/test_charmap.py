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


def test_charmap():
    face = ft.Face(vera_path())

    charmaps = face.charmaps
    assert len(charmaps) == 2

    assert charmaps[0].face is face
    assert charmaps[0].encoding == ft.ENCODING.APPLE_ROMAN
    assert charmaps[0].platform_id == ft.TT_PLATFORM.MACINTOSH
    assert charmaps[0].encoding_id == ft.TT_MAC_ID.ROMAN
    assert charmaps[1].face is face
    assert charmaps[1].encoding == ft.ENCODING.UNICODE
    assert charmaps[1].platform_id == ft.TT_PLATFORM.MICROSOFT
    assert charmaps[1].encoding_id == ft.TT_MS_ID.UNICODE_CS

    assert charmaps[0].get_format() == 0
    assert charmaps[1].get_format() == 4

    assert charmaps[0].get_language_id() == ft.TT_MAC_LANGID.ENGLISH
    assert charmaps[1].get_language_id() == 0


def test_different_charmap():
    face = ft.Face(vera_path())

    # The default charmap is in MacRoman ...
    face.set_charmap(0)
    macroman_id = face.get_char_index(0xCB)

    # ... which has a different encoding than the Unicode one
    face.set_charmap(1)
    unicode_id = face.get_char_index(0xC0)

    assert macroman_id == unicode_id


def test_select_charmap():
    face = ft.Face(vera_path())

    assert face.charmap.encoding == ft.ENCODING.UNICODE

    face.select_charmap(ft.ENCODING.APPLE_ROMAN)

    assert face.charmap.encoding == ft.ENCODING.APPLE_ROMAN

    try:
        face.select_charmap(ft.ENCODING.WANSUNG)
    except ValueError:
        pass
    else:
        assert False, "Expected an exception"


def test_instantiate_charmap():
    try:
        ft.CharMap()
    except RuntimeError:
        pass
    else:
        assert False, "Shouldn't be able to directly instantiate a CharMap"
