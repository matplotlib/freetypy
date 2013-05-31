#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -----------------------------------------------------------------------------
#
# Copyright (c) 2013, Michael Droettboom
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

# Original example from freetype-py:

#  FreeType high-level python API - Copyright 2011 Nicolas P. Rougier
#  Distributed under the terms of the new BSD license.
#
# -----------------------------------------------------------------------------
'''
Demonstrates getting Face information
'''
from __future__ import print_function, unicode_literals, absolute_import

import argparse

import freetypy as ft

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Displays information from a given font.')
    parser.add_argument(
        'filename', type=str, nargs=1,
        help='The font to display')
    args = parser.parse_args()
    filename = args.filename[0]

    face = ft.Face(filename)

    for attr in [
            'family_name',
            'style_name',
            'num_faces',
            'face_index',
            'face_flags',
            'style_flags',
            'num_glyphs',
            'charmaps',
            'bbox',
            'units_per_em',
            'ascender',
            'descender',
            'height',
            'max_advance_width',
            'max_advance_height',
            'underline_position',
            'underline_thickness',
            'has_horizontal',
            'has_vertical',
            'has_kerning',
            'is_scalable',
            'is_sfnt',
            'is_fixed_width']:
        val = getattr(face, attr)

        print("{0:20s}: {1!r}".format(attr, val))

    print()
    print("SFNT name entries:")
    for sfnt in face.sfnt_names:
        s = sfnt.string
        if len(s) > 50:
            s = s[:47] + '...'
        name_id = repr(sfnt.name_id)
        if '.' in name_id:
            name_id = name_id.split('.')[2]
        print("{0:20s}: {1}".format(name_id, s))
