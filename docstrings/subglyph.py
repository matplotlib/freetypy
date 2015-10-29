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


SubGlyph__init__ = """
A description of a given subglyph.
"""

SubGlyph_index = """
The glyph index of the subglyph.
"""

SubGlyph_flags = """
The `SUBGLYPH_FLAG` bitfield.
"""

SubGlyph_arg1 = """
The subglyph's first argument (if any).
"""

SubGlyph_arg2 = """
The subglyph's second argument (if any).
"""

SubGlyph_transform = """
The subglyph transformation (if any).
"""

SUBGLYPH_FLAG = """
A list of constants used to describe subglyphs.

- `ARGS_ARE_WORDS`: If set, `SubGlyph.arg1` and `SubGlyph.arg2`
  are words, else bytes.

- `ARGS_ARE_XY_VALUES`: If set, `SubGlyph.arg1` and `SubGlyph.arg2`
  are xy values.

- `ROUND_XY_TO_GRID`: If set, round the xy values to grid.

- `SCALE`: If set, there is a simple scale for the component.

- `XY_SCALE`: If set, the x direction will use a different scale than
  the y direction.

- `TWOXTWO`: If set, there is a 2-by-2 transformation that will be
  used to scale the component. (|freetypy| This is named `2X2` in the
  underlying freetype library, but was renamed here so it doesn't
  begin with a digit.)

- `USE_MY_METRICS`: Use metrics from this component for the compound
  glyph.
"""
