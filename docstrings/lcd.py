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

set_lcd_filter = """
Apply color filtering to LCD decimated bitmaps.

Works when called `Glyph.render` with `RENDER_MODE.LCD` or `RENDER_MODE.LCD_V`.

Parameters
----------

filter : `LCD_FILTER` constant

Notes
-----
This feature is always disabled by default. Clients must make an
explicit call to this function with a `filter` value other than
`LCD_FILTER.NONE` in order to enable it.

Due to PATENTS covering subpixel rendering, this function doesn't do
anything except raising `NotImplementedError` if the configuration
macro ``FT_CONFIG_OPTION_SUBPIXEL_RENDERING`` is not defined in your
build of the library, which should correspond to all default builds of
FreeType.
"""

set_lcd_filter_weights = """
Enable LCD filter with custom weights.

Parameters
----------
a, b, c, d, e : int
    The filter weights

Notes
-----
Due to PATENTS covering subpixel rendering, this function doesn't do
anything except raising `NotImplementedError` if the configuration
macro ``FT_CONFIG_OPTION_SUBPIXEL_RENDERING`` is not defined in your
build of the library, which should correspond to all default builds of
FreeType.
"""


LCD_FILTER = """
Identifies various types of LCD filters.

- `NONE`: Do not perform filtering. When used with subpixel rendering,
  this results in sometimes severe color fringes.

- `DEFAULT`: The default filter reduces color fringes considerably, at
  the cost of a slight blurriness in the output.  It is a beveled,
  normalized, and color-balanced five-tap filter that is more
  forgiving to screens with non-ideal gamma curves and viewing
  angles. Note that while color-fringing is reduced, it can only be
  minimized by using linear alpha blending and gamma correction to
  render glyphs onto surfaces. The default filter weights are [0x08
  0x4D 0x56 0x4D 0x08].

- `LIGHT`: The light filter is a variant that is sharper at the cost
  of slightly more color fringes than the default one.  It is a boxy,
  normalized, and color-balanced three-tap filter that is less
  forgiving to screens with non-ideal gamma curves and viewing
  angles. This filter works best when the rendering system uses linear
  alpha blending and gamma correction to render glyphs onto
  surfaces. The light filter weights are [0x00 0x55 0x56 0x55 0x00].

- `LEGACY`: This filter corresponds to the original libXft color
  filter. It provides high contrast output but can exhibit really bad
  color fringes if glyphs are not extremely well hinted to the pixel
  grid. In other words, it only works well if the TrueType bytecode
  interpreter is enabled and high-quality hinted fonts are used.

  This filter is only provided for comparison purposes, and might be
  disabled or stay unsupported in the future.
"""
