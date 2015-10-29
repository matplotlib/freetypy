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

Glyph__init__ = """
Contains information pertaining to a single glyph.

Notes
-----
If `Face.load_glyph` is called with default flags (see `LOAD.DEFAULT`)
the glyph image is loaded in the glyph slot in its native format
(e.g., an outline glyph for TrueType and Type 1 formats).

This image can later be converted into a bitmap by calling
`Glyph.render`.
"""

Glyph_advance = """
The transformed advance width for the glyph (in fractional
pixels). May not be transformed, depending on the value of
`LOAD.IGNORE_TRANSFORM`. As specified with `LOAD.VERTICAL_LAYOUT`, it
uses either the `horiAdvance` or the `vertAdvance` value of `metrics`
field.
"""

Glyph_bitmap = """
Get the `Bitmap` from this `Glyph` if it has been rendered.
"""

Glyph_bitmap_left = """
This is the bitmap's left bearing expressed in integer pixels. Of
course, this is only valid if the format is `GLYPH_FORMAT.BITMAP`.
"""

Glyph_bitmap_top = """
This is the bitmap's top bearing expressed in integer pixels. Remember
that this is the distance from the baseline to the top-most glyph
scanline, upwards y coordinates being positive. Of course, this is
only valid if the format is `GLYPH_FORMAT.BITMAP`.
"""

Glyph_face = """
The parent face object.
"""

Glyph_format = """
This field indicates the format of the image contained in the glyph
slot. Typically `GLYPH_FORMAT.BITMAP`, `GLYPH_FORMAT.OUTLINE`, or
`GLYPH_FORMAT.COMPOSITE`, but others are possible.

See `GLYPH_FORMAT` for the set of available formats.
"""

Glyph_get_cbox = """
Return a glyph's ‘control box’. The control box encloses all the
outline's points, including Bézier control points. Though it coincides
with the exact bounding box for most glyphs, it can be slightly larger
in some situations (like when rotating an outline which contains
Bézier outside arcs).

Computing the control box is very fast, while getting the bounding box
can take much more time as it needs to walk over all segments and arcs
in the outline.

Parameters
----------
mode : int, optional
    The mode which indicates how to interpret the returned bounding
    box values.  For the available options, see `GLYPH_BBOX`.

Returns
-------
bbox : BBox
    The glyph coordinate bounding box.

Notes
-----
Coordinates are relative to the glyph origin, using the *y* upwards
convention.

If the glyph has been loaded with `LOAD.NO_SCALE`, `mode` must be set
to `GLYPH_BBOX.UNSCALED` to get unscaled font units in 26.6
fixed-point pixel format.

If the font is tricky and the glyph has been loaded with
`LOAD.NO_SCALE`, the resulting `BBox` is meaningless. To get
reasonable values for the `BBox` it is necessary to load the glyph at
a large ppem value (so that the hinting instructions can properly
shift and scale the subglyphs), then extracting the `BBox` which can
be eventually converted back to font units.

Note that the maximum coordinates are exclusive, which means that one
can compute the width and height of the glyph image as::

  width  = bbox.xMax - bbox.xMin
  height = bbox.yMax - bbox.yMin

Note also that for 26.6 coordinates, if `mode` is set to
`GLYPH_BBOX.GRIDFIT`, the coordinates will also be grid-fitted, which
corresponds to::

  bbox.xMin = floor(bbox.xMin)
  bbox.yMin = floor(bbox.yMin)
  bbox.xMax = ceil(bbox.xMax)
  bbox.yMax = ceil(bbox.yMax)

To get the bbox in pixel coordinates, set `mode` to
`GLYPH_BBOX.TRUNCATE`.

To get the bbox in grid-fitted pixel coordinates, set `mode` to
`GLYPH_BBOX.PIXELS`.
"""

Glyph_linear_hori_advance = """
The advance width of the unhinted glyph. Its value is expressed in
fractional pixels, unless `LOAD.LINEAR_DESIGN` is set when loading the
glyph. This field can be important to perform correct WYSIWYG
layout. Only relevant for outline glyphs.
"""

Glyph_linear_vert_advance = """
The advance height of the unhinted glyph. Its value is expressed in
fractional pixels, unless `LOAD.LINEAR_DESIGN` is set when loading the
glyph. This field can be important to perform correct WYSIWYG
layout. Only relevant for outline glyphs.
"""

Glyph_lsb_delta = """
The difference between hinted and unhinted left side bearing while
autohinting is active. Zero otherwise.
"""

Glyph_metrics = """
The `GlyphMetrics` of the glyph. The returned values depend on the
last load flags (see the `Face.load_glyph` method) and can be
expressed either in 26.6 fractional pixels or font units.

Note that even when the glyph image is transformed, the metrics are
not.
"""

Glyph_outline = """
The outline descriptor for the current glyph image if its format is
`GLYPH_FORMAT.OUTLINE`. Once a glyph is loaded, `outline` can be
transformed, distorted, embolded, etc.
"""

Glyph_render = """
Convert a given glyph image to a `Bitmap`. It does so by inspecting
the glyph image format, finding the relevant renderer, and invoking
it.

Parameters
----------
render_mode : int, optional
    This is the render mode used to render the glyph image into a
    bitmap.

    See `RENDER_MODE` for the available options.

origin : 2-sequence of floats
    The (x, y) origin to translate the glyph image before rendering.

Returns
-------
bitmap : Bitmap
    The generated bitmap.

Notes
-----
The selected render mode only affects vector glyphs of a
font. Embedded bitmaps often have a different pixel mode like
`PIXEL_MODE.MONO`. You can use `Bitmap.convert` to transform them
into 8-bit pixmaps.
"""

Glyph_rsb_delta = """
The difference between hinted and unhinted right side bearing while
autohinting is active. Zero otherwise.
"""

Glyph_subglyphs = """
A sequence of all of the subglyphs that make up the glyph.

Empty unless `format` is `GLYPH_FORMAT.COMPOSITE`.
"""

GLYPH_BBOX = """
The mode how the values of `Glyph.get_cbox` are returned.

- `UNSCALED`: Return unscaled font units.

- `SUBPIXELS`: Return unfitted fractional coordinates.

- `GRIDFIT`: Return grid-fitted factional coordinates.

- `TRUNCATE`: Return coordinates in integer pixels.

- `PIXELS`: Return grid-fitted pixel coordinates.
"""

GLYPH_FORMAT = """
An enumeration type used to describe the format of a given glyph
image. Note that this version of FreeType only supports two image
formats, even though future font drivers will be able to register
their own format.

- `NONE`: The value 0 is reserved.

- `COMPOSITE`: The glyph image is a composite of several other
  images. This format is only used with `LOAD.NO_RECURSE`, and is
  used to report compound glyphs (like accented characters).

- `BITMAP`: The glyph image is a bitmap, and can be described as an
  `Bitmap`.

- `OUTLINE`: The glyph image is a vectorial outline made of line
  segments and Bézier arcs; it can be described as an `Outline`.

- `PLOTTER`: The glyph image is a vectorial path with no inside and
  outside contours. Some Type 1 fonts, like those in the Hershey
  family, contain glyphs in this format. These are described as
  `Outline`, but FreeType isn't currently capable of rendering them
  correctly.
"""

RENDER_MODE = """
A set of enumerations to select a bitmap renderer.

- `NORMAL`: This is the default render mode; it corresponds to 8-bit
  anti-aliased bitmaps.

- `LIGHT`: This is equivalent to `RENDER_MODE.NORMAL`. It is only
  defined as a separate value because render modes are also used
  indirectly to define hinting algorithm selectors. See
  `LOAD.TARGET_XXX` for details.

- `MONO`: This mode corresponds to 1-bit bitmaps (with 2 levels of
  opacity).

- `LCD`: This mode corresponds to horizontal RGB and BGR sub-pixel
  displays like LCD screens. It produces 8-bit bitmaps that are 3
  times the width of the original glyph outline in pixels, and which
  use the `PIXEL_MODE.LCD` mode.  On many freetype builds, this
  functionality will be disabled due to patent restrictions, in which
  case the resulting bitmap will be grayscale.

- `LCD_V`: This mode corresponds to vertical RGB and BGR sub-pixel
  displays (like PDA screens, rotated LCD displays, etc.). It produces
  8-bit bitmaps that are 3 times the height of the original glyph
  outline in pixels and use the `PIXEL_MODE.LCD_V` mode. On many
  freetype builds, this functionality will be disabled due to patent
  restrictions, in which case the resulting bitmap will be grayscale.
"""
