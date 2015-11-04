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

Glyph_Metrics__init__ = """
Models the metrics of a single `Glyph`.

Notes
-----
If not disabled with `LOAD.NO_HINTING`, the values represent
dimensions of the hinted glyph (in case hinting is applicable).

Stroking a glyph with an outside border does not increase
`horiAdvance` or `vertAdvance`; you have to manually adjust these
values to account for the added width and height.
"""

Glyph_Metrics_height = """
The glyph's height.
"""

Glyph_Metrics_hori_advance = """
Advance width for horizontal layout.
"""

Glyph_Metrics_hori_bearing_x = """
Left side bearing for horizontal layout.
"""

Glyph_Metrics_hori_bearing_y = """
Top side bearing for horizontal layout.
"""

Glyph_Metrics_vert_advance = """
Advance height for vertical layout. Positive values mean the glyph has
a positive advance downward.
"""

Glyph_Metrics_vert_bearing_x = """
Left side bearing for vertical layout.
"""

Glyph_Metrics_vert_bearing_y = """
Top side bearing for vertical layout. Larger positive values mean
further below the vertical glyph origin.
"""

Glyph_Metrics_width = """
The glyph's width.
"""
