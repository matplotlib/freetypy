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

Size_Metrics__init__ = """
The metrics of a size object.

The scaling values, if relevant, are determined first during a size
changing operation. The remaining fields are then set by the
driver. For scalable formats, they are usually set to scaled values of
the corresponding fields in `Face`.

Note that due to glyph hinting, these values might not be exact for
certain fonts. Thus they must be treated as unreliable with an error
margin of at least one pixel!

Indeed, the only way to get the exact metrics is to render all
glyphs. As this would be a definite performance hit, it is up to
client applications to perform such computations.

The `Size_Metrics` object is valid for bitmap fonts also.
"""

Size_Metrics_ascender = """
The ascender in fractional pixels.
"""

Size_Metrics_descender = """
The descender in fractional pixels.
"""

Size_Metrics_height = """
The height in fractional pixels.
"""

Size_Metrics_max_advance = """
The maximum advance width in fractional pixels.
"""

Size_Metrics_x_ppem = """
The width of the scaled EM square in pixels.

It is also referred to as ‘nominal width’.
"""

Size_Metrics_x_scale = """
A fractional scaling value used to convert horizontal metrics from
font units to 26.6 fractional pixels. Only relevant for scalable font
formats.
"""

Size_Metrics_y_ppem = """
The height of the scaled EM square in pixels.

It is also referred to as ‘nominal height’.
"""

Size_Metrics_y_scale = """
A fractional scaling value used to convert horizontal metrics from
font units to 26.6 fractional pixels. Only relevant for scalable font
formats.
"""
