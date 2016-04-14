#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -----------------------------------------------------------------------------
#
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

# Original example from freetype-py:

#  FreeType high-level python API - Copyright 2011 Nicolas P. Rougier
#  Distributed under the terms of the new BSD license.
#
# -----------------------------------------------------------------------------
'''
Demonstrates glyph bitmap rendering modes.
'''
from __future__ import print_function, unicode_literals, absolute_import

import argparse

import freetypy as ft
import freetypy.util as ft_util

import numpy as np

import matplotlib.pyplot as plt


def rendering_modes(char):
    face = ft.Face(ft_util.vera_path())
    face.select_charmap(ft.ENCODING.UNICODE)
    face.set_char_size(48)

    for i, mode in enumerate([ft.LOAD.TARGET_MONO,
                              ft.LOAD.TARGET_NORMAL,
                              ft.LOAD.TARGET_LCD,
                              ft.LOAD.TARGET_LCD_V]):
        glyph = face.load_char_unicode(char, ft.LOAD.RENDER | mode)
        bitmap = glyph.bitmap
        if mode == ft.LOAD.TARGET_MONO:
            bitmap = bitmap.convert()

        array = np.array(bitmap)

        plt.subplot(1, 4, i+1)
        plt.xticks([])
        plt.yticks([])

        if i < 2:
            plt.imshow(array, interpolation='nearest', cmap=plt.cm.gray)
        else:
            plt.imshow(array, interpolation='nearest')

        plt.xlabel(repr(bitmap.pixel_mode))

    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Displays a character using different rendering modes.')
    parser.add_argument(
        '-c', type=str, default='S',
        help='The character to display')
    args = parser.parse_args()
    char = args.c
    if isinstance(char, bytes):
        char = char.decode('utf-8')

    try:
        ft.set_lcd_filter(ft.LCD_FILTER.LIGHT)
    except NotImplementedError:
        print("LCD filtering is not available in your freetype build")

    rendering_modes(char)
