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

BBox__init__ = """
An outline's bounding box.

The bounding box is specified with the coordinates of the lower left
and the upper right corner. In PostScript, those values are often
called ``(llx, lly)`` and ``(urx, ury)``, respectively.

If `y_min` is negative, this value gives the glyph's
descender. Otherwise, the glyph doesn't descend below the
baseline. Similarly, if `y_max` is positive, this value gives the
glyph's ascender.

`x_min` gives the horizontal distance from the glyph's origin to the
left edge of the glyph's bounding box. If `x_min` is negative, the
glyph extends to the left of the origin.

`BBox` also works as a Python sequence, so it is easy to do::

    x_min, y_min, x_max, y_max = bbox
"""

BBox_ascent = """
The height of the bounding box above the baseline.  This is an alias
for `y_max`.
"""

BBox_depth = """
The depth of the bounding box below the baseline.  This is an alias
for `y_min`.
"""

BBox_height = """
The height of the bounding box (``y_max - y_min``).
"""

BBox_width = """
The width of the bounding box (``x_max - x_min``).
"""

BBox_x_max = """
The horizontal maximum (right-most).
"""

BBox_x_min = """
The horizontal minimum (left-most).
"""

BBox_y_max = """
The vertical maximum (top-most).
"""

BBox_y_min = """
The vertical minimum (bottom-most).
"""
