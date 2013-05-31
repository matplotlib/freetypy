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

CharMap__init__ = """
A mapping from code points to glyph identifiers. Some font formats may
provide several `CharMap` instances per font.

Each face object owns zero or more charmaps, but only one of them can
be ‘active’ and used by `Face.get_char_index` or `Face.load_char`.

The list of available charmaps in a face is available through
`Face.charmaps`.

The currently active charmap is available as `Face.charmap`.
"""

CharMap_encoding = """
An `ENCODING` tag identifying the charmap.

Despite the name, this value specifies a specific character
repertory (i.e., charset), and not a text encoding method (e.g.,
UTF-8, UTF-16, etc.).
"""

CharMap_encoding_id = """
A platform-specific encoding number. This also comes from the TrueType
specification and should be emulated similarly.

Depending on `platform_id`, this will be from `TT_APPLE_ID`,
`TT_MAC_ID`, `TT_MS_ID` or `TT_ADOBE_ID`.
"""

CharMap_face = """
The parent `Face` object.
"""

CharMap_get_format = """
Return TrueType/sfnt specific cmap format.

Returns
-------
format : int
    The format of ‘charmap’. If ‘charmap’ doesn't belong to a
    TrueType/sfnt face, raises a `ValueError`.
"""

CharMap_get_language_id = """
Return TrueType/sfnt specific cmap language ID.

It will be one of the constants in `TT_MAC_LANGID` or `TT_MS_LANGID`.

Returns
-------
language_id : int
    The language ID of the `CharMap`. If it doesn't belong to a
    TrueType/sfnt face, raises a `ValueError`.

    For a format 14 cmap (to access Unicode IVS), the return value is
    0xFFFFFFFF.
"""

CharMap_platform_id = """
An `TT_PLATFORM` id describing the platform for the following encoding
ID. This comes directly from the TrueType specification and should be
emulated for other formats.
"""

ENCODING = """
A tag identifying a `CharMap` type.

- `NONE`: The encoding value 0 is reserved.

- `UNICODE`: Corresponds to the Unicode character set. This value
  covers all versions of the Unicode repertoire, including ASCII and
  Latin-1. Most fonts include a Unicode charmap, but not all of them.

  For example, if you want to access Unicode value U+1F028 (and the
  font contains it), use value 0x1F028 as the input value for
  `Face.get_char_index`.

- `MS_SYMBOL`: Corresponds to the Microsoft Symbol encoding, used to
  encode mathematical symbols in the 32..255 character code range. For
  more information, see `<http://www.ceviz.net/symbol.htm>`_.

- `SJIS`: Corresponds to Japanese SJIS encoding. More info at at
  `<http://langsupport.japanreference.com/encoding.shtml>`_. See note
  on multi-byte encodings below.

- `GB2312`: Corresponds to an encoding system for Simplified Chinese
  as used used in mainland China.

- `BIG5`: Corresponds to an encoding system for Traditional Chinese as
  used in Taiwan and Hong Kong.

- `WANSUNG`: Corresponds to the Korean encoding system known as
  Wansung. For more information see
  `<http://www.microsoft.com/typography/unicode/949.txt>`_.

- `JOHAB`: The Korean standard character set (KS C 5601-1992), which
  corresponds to MS Windows code page 1361. This character set
  includes all possible Hangeul character combinations.

- `ADOBE_LATIN_1`: Corresponds to a Latin-1 encoding as defined in a
  Type 1 PostScript font. It is limited to 256 character codes.

- `ADOBE_STANDARD`: Corresponds to the Adobe Standard encoding, as
  found in Type 1, CFF, and OpenType/CFF fonts. It is limited to 256
  character codes.

- `ADOBE_EXPERT`: Corresponds to the Adobe Expert encoding, as found
  in Type 1, CFF, and OpenType/CFF fonts. It is limited to 256
  character codes.

- `ADOBE_CUSTOM`: Corresponds to a custom encoding, as found in Type
  1, CFF, and OpenType/CFF fonts. It is limited to 256 character
  codes.

- `APPLE_ROMAN`: Corresponds to the 8-bit Apple roman encoding. Many
  TrueType and OpenType fonts contain a charmap for this encoding,
  since older versions of Mac OS are able to use it.
"""
