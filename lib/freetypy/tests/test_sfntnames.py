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

import freetypy as ft
from .util import *


def test_sfntnames():
    face = ft.Face(vera_path())

    sfntnames = face.sfnt_names

    assert len(sfntnames) == 22

    values = set((x.name_id, x.string) for x in sfntnames)
    assert values == set([
        (ft.TT_NAME_ID.COPYRIGHT,
         'Copyright (c) 2003 by Bitstream, Inc. All Rights Reserved.'),
        (ft.TT_NAME_ID.FONT_FAMILY, 'Bitstream Vera Sans'),
        (ft.TT_NAME_ID.FONT_SUBFAMILY, 'Roman'),
        (ft.TT_NAME_ID.UNIQUE_ID, 'Bitstream Vera Sans'),
        (ft.TT_NAME_ID.FULL_NAME, 'Bitstream Vera Sans'),
        (ft.TT_NAME_ID.VERSION_STRING, 'Release 1.10'),
        (ft.TT_NAME_ID.PS_NAME, 'BitstreamVeraSans-Roman'),
        (ft.TT_NAME_ID.TRADEMARK,
         'Bitstream Vera is a trademark of Bitstream, Inc.'),
        (ft.TT_NAME_ID.MANUFACTURER, 'Bitstream Inc.'),
        (ft.TT_NAME_ID.VENDOR_URL, 'http://www.bitstream.com'),
        (ft.TT_NAME_ID.LICENSE, 'Copyright (c) 2003 by Bitstream, Inc.\r\nAll Rights Reserved.\r\nBitstream Vera is a trademark of Bitstream, Inc.\r\n\r\nPermission is hereby granted, free of charge, to any person obtaining a copy of the fonts accompanying this license ("Fonts") and associated documentation files (the "Font Software"), to reproduce and distribute the Font Software, including without limitation the rights to use, copy, merge, publish, distribute, and/or sell copies of the Font Software, and to permit persons to whom the Font Software is furnished to do so, subject to the following conditions:\r\n\r\nThe above copyright and trademark notices and this permission notice shall be included in all copies of one or more of the Font Software typefaces.\r\n\r\nThe Font Software may be modified, altered, or added to, and in particular the designs of glyphs or characters in the Fonts may be modified and additional glyphs or characters may be added to the Fonts, only if the fonts are renamed to names not containing either the words "Bitstream" or the word "Vera".\r\n\r\nThis License becomes null and void to the extent applicable to Fonts or Font Software that has been modified and is distributed under the "Bitstream Vera" names.\r\n\r\nThe Font Software may be sold as part of a larger software package but no copy of one or more of the Font Software typefaces may be sold by itself.\r\n\r\nTHE FONT SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF COPYRIGHT, PATENT, TRADEMARK, OR OTHER RIGHT. IN NO EVENT SHALL BITSTREAM OR THE GNOME FOUNDATION BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, INCLUDING ANY GENERAL, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF THE USE OR INABILITY TO USE THE FONT SOFTWARE OR FROM OTHER DEALINGS IN THE FONT SOFTWARE.\r\n\r\nExcept as contained in this notice, the names of Gnome, the Gnome Foundation, and Bitstream Inc., shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Font Software without prior written authorization from the Gnome Foundation or Bitstream Inc., respectively. For further information, contact: fonts at gnome dot org.'),
        (ft.TT_NAME_ID.COPYRIGHT,
         'Copyright (c) 2003 by Bitstream, Inc. All Rights Reserved.'),
        (ft.TT_NAME_ID.FONT_FAMILY, 'Bitstream Vera Sans'),
        (ft.TT_NAME_ID.FONT_SUBFAMILY, 'Roman'),
        (ft.TT_NAME_ID.UNIQUE_ID, 'Bitstream Vera Sans'),
        (ft.TT_NAME_ID.FULL_NAME, 'Bitstream Vera Sans'),
        (ft.TT_NAME_ID.VERSION_STRING, 'Release 1.10'),
        (ft.TT_NAME_ID.PS_NAME, 'BitstreamVeraSans-Roman'),
        (ft.TT_NAME_ID.TRADEMARK,
         'Bitstream Vera is a trademark of Bitstream, Inc.'),
        (ft.TT_NAME_ID.MANUFACTURER, 'Bitstream Inc.'),
        (ft.TT_NAME_ID.VENDOR_URL, 'http://www.bitstream.com'),
        (ft.TT_NAME_ID.LICENSE, 'Copyright (c) 2003 by Bitstream, Inc.\r\nAll Rights Reserved.\r\nBitstream Vera is a trademark of Bitstream, Inc.\r\n\r\nPermission is hereby granted, free of charge, to any person obtaining a copy of the fonts accompanying this license ("Fonts") and associated documentation files (the "Font Software"), to reproduce and distribute the Font Software, including without limitation the rights to use, copy, merge, publish, distribute, and/or sell copies of the Font Software, and to permit persons to whom the Font Software is furnished to do so, subject to the following conditions:\r\n\r\nThe above copyright and trademark notices and this permission notice shall be included in all copies of one or more of the Font Software typefaces.\r\n\r\nThe Font Software may be modified, altered, or added to, and in particular the designs of glyphs or characters in the Fonts may be modified and additional glyphs or characters may be added to the Fonts, only if the fonts are renamed to names not containing either the words "Bitstream" or the word "Vera".\r\n\r\nThis License becomes null and void to the extent applicable to Fonts or Font Software that has been modified and is distributed under the "Bitstream Vera" names.\r\n\r\nThe Font Software may be sold as part of a larger software package but no copy of one or more of the Font Software typefaces may be sold by itself.\r\n\r\nTHE FONT SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF COPYRIGHT, PATENT, TRADEMARK, OR OTHER RIGHT. IN NO EVENT SHALL BITSTREAM OR THE GNOME FOUNDATION BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, INCLUDING ANY GENERAL, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF THE USE OR INABILITY TO USE THE FONT SOFTWARE OR FROM OTHER DEALINGS IN THE FONT SOFTWARE.\r\n\r\nExcept as contained in this notice, the names of Gnome, the Gnome Foundation, and Bitstream Inc., shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Font Software without prior written authorization from the Gnome Foundation or Bitstream Inc., respectively. For further information, contact: fonts at gnome dot org.')
        ])


def test_get_name():
    face = ft.Face(vera_path())

    sfntname = face.sfnt_names.get_name(ft.TT_NAME_ID.COPYRIGHT)
    assert sfntname.platform_id == ft.TT_PLATFORM.MICROSOFT
    assert sfntname.encoding_id == ft.TT_MS_ID.UNICODE_CS
