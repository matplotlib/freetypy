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

Bitmap__init__ = """
A structure used to describe a bitmap or pixmap to the raster.

`Bitmap` supports the Python buffer interface, so it is easy to
convert it to a Numpy array.  For example::

    >>> import numpy as np
    >>> a = np.asarray(bitmap)
"""

Bitmap_buffer = """
Get the bitmap's contents as a buffer.

In most cases, the preferred method to get the data is to cast the
`Bitmap` object to a memoryview, since that will also have size and
type information.
"""

Bitmap_convert = """
Convert a `Bitmap` to 8 bits per pixel.  Given a `Bitmap` with depth
1bpp, 2bpp, 4bpp, or 8bpp converts it to one with depth 8bpp, making
the number of used bytes per line (a.k.a. the ‘pitch’) a multiple of
`alignment`.

Parameters
----------
alignment : int, optional
    The pitch of the bitmap is a multiple of this parameter. Common
    values are 1, 2, or 4.

Returns
-------
target : Bitmap
    The bitmap, converted to 8bpp.
"""

Bitmap_num_grays = """
The number of gray levels used in the bitmap. This field is only used
with `PIXEL_MODE.GRAY`.
"""

Bitmap_pitch = """
The number of bytes taken by one bitmap row.

Includes padding.

The pitch is positive when the bitmap has a ‘down’ flow, and negative
when it has an ‘up’ flow. In all cases, the pitch is an offset to add
to a bitmap pointer in order to go down one row.

Note that ‘padding’ means the alignment of a bitmap to a byte border,
and FreeType functions normally align to the smallest possible integer
value.

For the B/W rasterizer, `pitch` is always an even number.

To change the pitch of a bitmap (say, to make it a multiple of 4), use
`Bitmap.convert`. Alternatively, you might use callback functions to
directly render to the application's surface.
"""

Bitmap_pixel_mode = """
The `PIXEL_MODE`, i.e., how pixel bits are stored.
"""

Bitmap_rows = """
The number of bitmap rows.
"""

Bitmap_to_list = """
|freetypy| Convert the bitmap to a nested list.
"""

Bitmap_width = """
The number of pixels in bitmap row.
"""

PIXEL_MODE = """
Constants related to the pixel mode of bitmaps.

- `MONO`: A monochrome bitmap, using 1 bit per pixel. Note that pixels
  are stored in most-significant order (MSB), which means that the
  left-most pixel in a byte has value 128.

- `GRAY`: An 8-bit bitmap, generally used to represent anti-aliased
  glyph images. Each pixel is stored in one byte. Note that the number
  of ‘gray’ levels is stored in the ‘num_grays’ field of the Bitmap
  structure (it generally is 256).

- `GRAY2`: A 2-bit per pixel bitmap, used to represent embedded
  anti-aliased bitmaps in font files according to the OpenType
  specification. We haven't found a single font using this format,
  however.

- `GRAY4`: A 4-bit per pixel bitmap, representing embedded
  anti-aliased bitmaps in font files according to the OpenType
  specification. We haven't found a single font using this format,
  however.

- `LCD`: An 8-bit bitmap, representing RGB or BGR decimated glyph
  images used for display on LCD displays; the bitmap is three times
  wider than the original glyph image. See also `RENDER_MODE.LCD`.  On
  many freetype builds, this functionality will be disabled due to
  patent restrictions, in which case the resulting bitmap will be
  grayscale.

- `LCD_V`: An 8-bit bitmap, representing RGB or BGR decimated glyph
  images used for display on rotated LCD displays; the bitmap is three
  times taller than the original glyph image. See also
  `RENDER_MODE.LCD_V`. On many freetype builds, this functionality
  will be disabled due to patent restrictions, in which case the
  resulting bitmap will be grayscale.
"""
