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

SfntName__init__ = """
An SFNT ‘name’ table entry.
"""

SfntName_encoding_id = """
The encoding ID.

Depending on the value of `platform_id`, this will be one of the
constants in `TT_APPLE_ID`, `TT_MAC_ID`, `TT_ISO_ID`, `TT_MS_ID` or
`TT_ADOBE_ID`.
"""

SfntName_language_id = """
The language ID.

Depending on the value of `platform_id`, this will be one of the
constants in `TT_MAC_LANGID` or `TT_MS_LANGID`.
"""

SfntName_name_id = """
The type of value stored in `string`.  See `TT_NAME_ID` for the
possible values.
"""

SfntName_platform_id = """
The platform ID.  One of `TT_PLATFORM`.
"""

SfntName_string = """
The value of the `SfntName` entry.

|freetypy|: Unlike the raw C interface, this value is always decoded
into a Unicode string, so users do not need to worry about its
encoding.  To get the undecoded bytes, use `string_bytes`.
"""

SfntName_string_bytes = """
The raw undecoded bytes of the value of the `SfntName` entry.

|freetypy|: Most of the time, you will want to use `string` instead,
which automatically handles the decoding of the bytes.
"""
