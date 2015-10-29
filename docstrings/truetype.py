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


TT_PLATFORM = """
A set of platform identifier codes.

- `APPLE_UNICODE`: Used by Apple to indicate a Unicode character map
  and/or name entry. See `TT_APPLE_ID` for corresponding ‘encoding_id’
  values. Note that name entries in this format are coded as
  big-endian UCS-2 character codes only.

- `MACINTOSH`: Used by Apple to indicate a MacOS-specific charmap
  and/or name entry. See `TT_MAC_ID` for corresponding ‘encoding_id’
  values. Note that most TrueType fonts contain an Apple roman charmap
  to be usable on MacOS systems (even if they contain a Microsoft
  charmap as well).

- `ISO`: This value was used to specify ISO/IEC 10646 charmaps. It is
  however now deprecated. See `TT_ISO_ID` for a list of
  corresponding ‘encoding_id’ values.

- `MICROSOFT`: Used by Microsoft to indicate Windows-specific
  charmaps. See `TT_MS_ID` for a list of corresponding ‘encoding_id’
  values. Note that most fonts contain a Unicode charmap using
  (`TT_PLATFORM.MICROSOFT`, `TT_MS_ID.UNICODE_CS`).

- `CUSTOM`: Used to indicate application-specific charmaps.

- `ADOBE`: This value isn't part of any font format specification, but
  is used by FreeType to report Adobe-specific charmaps in an
  `CharMap` object. See `TT_ADOBE_ID`.
"""

TT_APPLE_ID = """
Apple-specific encoding values.

- `DEFAULT`: Unicode version 1.0.

- `UNICODE_1_1`: Unicode 1.1; specifies Hangul characters starting at
  U+34xx.

- `ISO_10646`: Deprecated (identical to preceding).

- `UNICODE_2_0`: Unicode 2.0 and beyond (UTF-16 BMP only).

- `UNICODE_32`: Unicode 3.1 and beyond, using UTF-32.

- `VARIANT_SELECTOR`: From Adobe, not Apple. Not a normal
  cmap. Specifies variations on a real cmap.
"""

TT_ADOBE_ID = """
Adobe-specific encoding values.

- `STANDARD`: Adobe standard encoding.

- `EXPERT`: Adobe expert encoding.

- `CUSTOM`: Adobe custom encoding.

- `LATIN_1`: Adobe Latin 1 encoding.
"""

TT_ISO_ID = """
Standard ISO encodings.

- `ISO_7BIT_ASCII`: ASCII.

- `ISO_10646`: ISO/10646.

- `ISO_8859_1`: Also known as Latin-1.
"""

TT_MAC_ID = """
Macintosh-specific encoding values.

- `ROMAN`
- `JAPANESE`
- `TRADITIONAL_CHINESE`
- `KOREAN`
- `ARABIC`
- `HEBREW`
- `GREEK`
- `RUSSIAN`
- `RSYMBOL`
- `DEVANAGARI`
- `GURMUKHI`
- `GUJARATI`
- `ORIYA`
- `BENGALI`
- `TAMIL`
- `TELUGU`
- `KANNADA`
- `MALAYALAM`
- `SINHALESE`
- `BURMESE`
- `KHMER`
- `THAI`
- `LAOTIAN`
- `GEORGIAN`
- `ARMENIAN`
- `MALDIVIAN`
- `SIMPLIFIED_CHINESE`
- `TIBETAN`
- `MONGOLIAN`
- `GEEZ`
- `SLAVIC`
- `VIETNAMESE`
- `SINDHI`
- `UNINTERP`
"""

TT_MAC_LANGID = """
Possible values of the language identifier field in the name records
of the TTF "name" table if the "platform" identifier code is
`TT_PLATFORM.MACINTOSH`.
"""

TT_MS_ID = """
Microsoft-specific encoding values.

- `SYMBOL_CS`: Corresponds to Microsoft symbol encoding. See
  `ENCODING.MS_SYMBOL`.

- `UNICODE_CS`: Corresponds to a Microsoft WGL4 charmap, matching
  Unicode. See `ENCODING.UNICODE`.

- `SJIS`: Corresponds to SJIS Japanese encoding. See `ENCODING.SJIS`.

- `GB2312`: Corresponds to Simplified Chinese as used in Mainland
  China. See `ENCODING.GB2312`.

- `BIG_5`: Corresponds to Traditional Chinese as used in Taiwan and
  Hong Kong. See `ENCODING.BIG5`.

- `WANSUNG`: Corresponds to Korean Wansung encoding. See
  `ENCODING.WANSUNG`.

- `JOHAB`: Corresponds to Johab encoding. See `ENCODING.JOHAB`.

- `UCS_4`: Corresponds to UCS-4 or UTF-32 charmaps. This has been
  added to the OpenType specification version 1.4 (mid-2001.)
"""

TT_MS_LANGID = """
Possible values of the language identifier field in the name records
of the TTF "name" table if the "platform" identifier code is
`TT_PLATFORM.MICROSOFT`.
"""

TT_NAME_ID = """
Indicates the type of value stored in a `SfntName` record.

- `COPYRIGHT`
- `FONT_FAMILY`
- `FONT_SUBFAMILY`
- `UNIQUE_ID`
- `FULL_NAME`
- `VERSION_STRING`
- `PS_NAME`
- `TRADEMARK`

The following values are from the OpenType spec:

- `MANUFACTURER`
- `DESIGNER`
- `DESCRIPTION`
- `VENDOR_URL`
- `DESIGNER_URL`
- `LICENSE`
- `LICENSE_URL`
- `PREFERRED_FAMILY`
- `PREFERRED_SUBFAMILY`
- `MAC_FULL_NAME`
- `SAMPLE_TEXT`

This is new in OpenType 1.3:

- `CID_FINDFONT_NAME`

This is new in OpenType 1.5:

- `WWS_FAMILY`
- `WWS_SUBFAMILY`
"""
