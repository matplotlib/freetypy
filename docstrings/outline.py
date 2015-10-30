# -*- coding: utf-8 -*-

# Copyright (c) 2013, Michael Droettboom All rights reserved.

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

Outline__init__ = """
Represents an outline.
"""

Outline_n_contours = """
The number of contours in the outline.
"""

Outline_n_points = """
The number of points in the outline.
"""

Outline_points = """
A `memoryview` of points in the outline.  Each entry is an (x, y)
pair.
"""

Outline_tags = """
A `memoryview` of `n_points` values, giving each outline point's type.

- If bit 0 is unset, the point is ‘off’ the curve, i.e., a Bézier
  control point, while it is ‘on’ if set.

- Bit 1 is meaningful for ‘off’ points only. If set, it indicates a
  third-order Bézier arc control point; and a second-order control
  point if unset.

- If bit 2 is set, bits 5-7 contain the drop-out mode (as defined in
  the OpenType specification; the value is the same as the argument to
  the `SCANMODE` instruction).

- Bits 3 and 4 are reserved for internal purposes.
"""

Outline_contours = """
A `memoryview` giving the end point of each contour within the
outline. For example, the first contour is defined by the points `0`
to `contours[0]`, the second one is defined by the points `contours[0]
+ 1` to `contours[1]`, etc.

If using numpy, you may want to use this as a Numpy array as follows::

    >>> import numpy as np
    >>> a = np.asarray(outline.contours)
"""

Outline_flags = """
A set of `OUTLINE` flags used to characterize the outline.  Gives
hints to the scan-converter and hinter on how to convert/grid-fit it.
"""

Outline_check = """
Check the contents of an outline descriptor.

Raises an exception if the outline has problems.
"""

Outline_decompose = """
Decompose the outline into individual segments and Bézier arcs. This
function also emits ‘move to’ operations to indicate the start of new
contours in the outline.

Depending on the context, the values passed to the callback object
will either be in 26.6, 16.16 or native font units.

Parameters
----------
callback_object : object
    A Python object containing methods that will be called for
    each element of the outline.  The methods are:

        - `move_to(point)`

        - `line_to(point)`

        - `conic_to(control, endpoint)`

        - `cubic_to(control1, control2, endpoint)`

    |freetypy| If `callback_object` does not have a `conic_to` method,
    conic curves will be converted to cubic ones and `cubic_to` will
    be called.

shift : int, optional
    The number of bits to shift coordinates before they are sent to
    the emitter.

delta : int, optional
    The delta that is applied to coordinates before they are sent to
    the emitter, but after the shift.

    The transformation applied by shift and delta is::

        x' = (x << shift) - delta
        y' = (y << shift) - delta

Examples
--------

::

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

    face = ft.Face("Vera.ttf")
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(ord('B'))

    d = Decomposer()
    glyph.outline.decompose(d)
    print(d.entries)

Notes
-----
The point coordinates sent to the emitters are the transformed version
of the original coordinates (this is important for high accuracy
during scan-conversion). The transformation is simple::

    x = (x << shift) - delta
    y = (y << shift) - delta
"""

Outline_embolden = """
Embolden an outline. The new outline will be at most 4 times
`strength` pixels wider and higher. You may think of the left and
bottom borders as unchanged.

Negative `strength` values to reduce the outline thickness are
possible also.

Parameters
----------
strength : float
    How strong the glyph is emboldened.

Notes
-----
The used algorithm to increase or decrease the thickness of the glyph
doesn't change the number of points; this means that certain
situations like acute angles or intersections are sometimes handled
incorrectly.

If you need ‘better’ metrics values you should call `Outline.get_cbox`
or `Outline.get_bbox`.
"""

Outline_embolden_xy = """
Embolden an outline, with differing *x* and *y* amounts. The new
outline will be `xstrength` pixels wider and `ystrength` pixels
higher. Otherwise, it is similar to `Outline.embolden`, which uses the
same strength in both directions.

Parameters
----------
xstrength : float
    How strong the glyph is emboldened in the x direction.

ystrength : float
    How strong the glyph is emboldened in the y direction.
"""

Outline_get_bbox = """
Compute the exact bounding box of an outline.  This is slower than
computing the control box.  However, it uses an advanced algorithm
which returns very quickly with the two boxes coincide.  Otherwise,
the outline Bézier arcs are traversed to extract their extrema.

Returns
-------
bbox : BBox
    The outline's exact bounding box.

Notes
-----
If the font is tricky and the glyph has been loaded with
`LOAD.NO_SCALE`, the resulting `BBox` is meaningless. To get
reasonable values for the `BBox` it is necessary to load the glyph at
a large ppem value (so that the hinting instructions can properly
shift and scale the subglyphs), then extracting the `BBox` which can
be eventually converted back to font units.
"""

Outline_get_cbox = """
Get the control box. The control box encloses all the outline's
points, including Bézier control points. Though it coincides with the
exact bounding box for most glyphs, it can be slightly larger in some
situations (like when rotating an outline which contains Bézier
outside arcs).

Computing the control box is very fast, while getting the bounding box
can take much more time as it needs to walk over all segments and arcs
in the outline.

See `Glyph.get_cbox` for a discussion of tricky fonts.
"""

Outline_get_orientation = """
This function analyzes a glyph outline and tries to compute its fill
orientation. This is done by computing the direction of each global
horizontal and/or vertical extrema within the outline.

See `ORIENTATION` for a list of the result values.

Note that this will return `ORIENTATION.TRUETYPE` for empty outlines.
"""

Outline_reverse = """
Reverse the drawing direction of an outline.  This is used to ensure
consistent fill conventions for mirrored glyphs.

Notes
-----
The function toggles the `OUTLINE.REVERSE_FILL` bit flag in
`Outline.flags`.

It shouldn't be used by a normal client application, unless it knows
what it is doing.
"""

Outline_to_string = """
|freetypy| Convert the outline to a text format string of commands.
This function is flexible enough to create path commands for PDF,
Postcript and SVG.

Parameters
----------
move_command : bytes
    The character or command to use for "move to" commands.

line_command : bytes
    The character or command to use for "line to" commands.

cubic_command : bytes
    The character or command to use for "cubic curve" commands.

conic_command : bytes, optional
    The character or command to use for "conic curve" commands.  If
    one is not provided, conic curves will be implicitly converted to
    cubic curves.

Returns
-------
string : bytes
    A text-based string of commands to render the character.

Examples
--------
To generate a PDF-compatible path::

    outline.to_string(" m ", " l ", " c ")
"""

Outline_transform = """
Apply a simple 2x2 matrix to all of an outline's points. Useful for
applying rotations, slanting, flipping, etc.

Parameters
----------
matrix : 2x2 sequence of floats, optional
    A matrix of the form::

        . [[xx, xy]
        .  [yx, yy]]
"""

Outline_translate = """
Apply a simple translation to the points of an outline.

Parameters
----------
xOffset : int
    The horizontal offset.

yOffset : int
    The vertical offset.
"""

OUTLINE = """
A set of bit flags to characterize an `Outline`.

- `NONE`: Value 0 is reserved.

- `OWNER`: If set, this flag indicates that the outline's field arrays
  (i.e., `~Outline.points`, `~Outline.flags`, and `~Outline.contours`)
  are ‘owned’ by the outline object, and should thus be freed when it
  is destroyed.

- `EVEN_ODD_FILL`: By default, outlines are filled using the non-zero
  winding rule. If set to 1, the outline will be filled using the
  even-odd fill rule (only works with the smooth rasterizer).

- `REVERSE_FILL`: By default, outside contours of an outline are
  oriented in clock-wise direction, as defined in the TrueType
  specification. This flag is set if the outline uses the opposite
  direction (typically for Type 1 fonts). This flag is ignored by the
  scan converter.

- `IGNORE_DROPOUTS`: By default, the scan converter will try to detect
  drop-outs in an outline and correct the glyph bitmap to ensure
  consistent shape continuity. If set, this flag hints the scan-line
  converter to ignore such cases. See below for more information.

- `SMART_DROPOUTS`: Select smart dropout control. If unset, use simple
  dropout control. Ignored if `OUTLINE.IGNORE_DROPOUTS` is set. See
  below for more information.

- `INCLUDE_STUBS`: If set, turn pixels on for ‘stubs’, otherwise
  exclude them. Ignored if `OUTLINE.IGNORE_DROPOUTS` is set. See
  below for more information.

- `HIGH_PRECISION`: This flag indicates that the scan-line converter
  should try to convert this outline to bitmaps with the highest
  possible quality. It is typically set for small character
  sizes. Note that this is only a hint that might be completely
  ignored by a given scan-converter.

- `SINGLE_PASS`: This flag is set to force a given scan-converter to
  only use a single pass over the outline to render a bitmap glyph
  image. Normally, it is set for very large character sizes. It is
  only a hint that might be completely ignored by a given
  scan-converter.

Notes
-----
The flags `OUTLINE.IGNORE_DROPOUTS`, `OUTLINE.SMART_DROPOUTS`, and
`OUTLINE.INCLUDE_STUBS` are ignored by the smooth rasterizer.

There exists a second mechanism to pass the drop-out mode to the B/W
rasterizer; see the `Outline.tags`.

Please refer to the description of the ‘SCANTYPE’ instruction in the
OpenType specification (in file ‘ttinst1.doc’) how simple drop-outs,
smart drop-outs, and stubs are defined.
"""

ORIENTATION = """
Values to specify how an outline is oriented.

- `TRUETYPE`: According to the TrueType specification, clockwise
  contours must be filled, and counter-clockwise ones must be
  unfilled.

- `POSTSCRIPT`: According to the PostScript specification,
  counter-clockwise contours must be filled, and clockwise ones must
  be unfilled.

- `FILL_RIGHT`: This is identical to `ORIENTATION.TRUETYPE`, but is
  used to remember that in TrueType, everything that is to the right
  of the drawing direction of a contour must be filled.

- `FILL_LEFT`: This is identical to `ORIENTATION.POSTSCRIPT`, but is
  used to remember that in PostScript, everything that is to the left
  of the drawing direction of a contour must be filled.

- `NONE`: The orientation cannot be determined. That is, different
  parts of the glyph have different orientation.
"""
