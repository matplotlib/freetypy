/*
Copyright (c) 2015, Michael Droettboom
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
*/

#include "encoding.h"

#include <endian.h>

#include FT_TRUETYPE_IDS_H


static int freetype_to_python_encoding(
        unsigned short platform_id, unsigned short encoding_id,
        const char **encoding, const char **fallback_encoding)
{
    *encoding = NULL;
    *fallback_encoding = NULL;

    switch(platform_id) {
    case TT_PLATFORM_APPLE_UNICODE:
        *encoding = "utf_16_be";
        break;

    case TT_PLATFORM_MACINTOSH:
        switch(encoding_id) {
        case TT_MAC_ID_ROMAN:
            *encoding = "mac_roman";
            break;

        case TT_MAC_ID_JAPANESE:
            *encoding = "mac_japanese";
            *fallback_encoding = "shift_jis";
            break;

        case TT_MAC_ID_TRADITIONAL_CHINESE:
            *encoding = "mac_chintrad";
            *fallback_encoding = "big5";
            break;

        case TT_MAC_ID_KOREAN:
            *encoding = "mac_korean";
            *fallback_encoding = "euc_kr";
            break;

        case TT_MAC_ID_ARABIC:
            *encoding = "mac_arabic";
            *fallback_encoding = "iso8859_6";
            break;

        case TT_MAC_ID_HEBREW:
            *encoding = "mac_hebrew";
            *fallback_encoding = "iso8859_8";
            break;

        case TT_MAC_ID_GREEK:
            *encoding = "mac_greek";
            break;

        case TT_MAC_ID_RUSSIAN:
            *encoding = "mac_cyrillic";
            break;

        case TT_MAC_ID_RSYMBOL:
            /* Not sure what this is */
            break;

        case TT_MAC_ID_DEVANAGARI:
            #ifdef FTPY_USE_LEGACY_MAC_ENCODINGS
            *encoding = "mac_devanaga";
            #else
            /* This is based on IS 13194:1991 (ISCII-91), which I
               don't think Python has */
            #endif
            break;

        case TT_MAC_ID_GURMUKHI:
            *encoding = "mac_gurmukhi";
            /* This is based on IS 13194:1991 (ISCII-91), which I
               don't think Python has */
            break;

        case TT_MAC_ID_GUJARATI:
            *encoding = "mac_gujarati";
            /* This is based on IS 13194:1991 (ISCII-91), which I
               don't think Python has */
            break;

        case TT_MAC_ID_ORIYA:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_BENGALI:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_TAMIL:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_TELUGU:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_KANNADA:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_MALAYALAM:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_SINHALESE:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_BURMESE:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_KHMER:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_THAI:
            *encoding = "mac_thai";
            /* This is based on TIS 620-2533, which I don't believe is
               in Python. */
            break;

        case TT_MAC_ID_LAOTIAN:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_GEORGIAN:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_ARMENIAN:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_SIMPLIFIED_CHINESE:
            *encoding = "mac_chinsimp";
            *fallback_encoding = "gb2312";
            break;

        case TT_MAC_ID_TIBETAN:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_MONGOLIAN:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_GEEZ:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_SLAVIC:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_VIETNAMESE:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_SINDHI:
            /* Not sure what this is. */
            break;

        case TT_MAC_ID_UNINTERP:
            /* Not sure what this is. */
            break;

        default:
            break;
        }
        break;

    case TT_PLATFORM_ISO:
        switch(encoding_id) {
        case TT_ISO_ID_7BIT_ASCII:
            *encoding = "ascii";
            break;

        case TT_ISO_ID_10646:
            /* TODO: This is probably wrong, but it's not clear what
             * it should be.  Probably need to find a font that uses
             * this and test. */
            *encoding = "utf_16_be";
            break;

        case TT_ISO_ID_8859_1:
            *encoding = "latin_1";
            break;

        default:
            break;
        }
        break;

    case TT_PLATFORM_MICROSOFT:
        switch (encoding_id) {
        case TT_MS_ID_SYMBOL_CS:
            *encoding = "mac_symbol";
            break;

        case TT_MS_ID_UNICODE_CS:
            *encoding = "utf_16_be";
            break;

        case TT_MS_ID_SJIS:
            *encoding = "shift_jis";
            break;

        case TT_MS_ID_GB2312:
            *encoding = "gb2312";
            break;

        case TT_MS_ID_BIG_5:
            *encoding = "big5";
            break;

        case TT_MS_ID_WANSUNG:
            *encoding = "euc_kr";
            break;

        case TT_MS_ID_JOHAB:
            *encoding = "johab";
            break;

        case TT_MS_ID_UCS_4:
            *encoding = "utf_32_be";
            break;

        default:
            break;
        }
        break;

    case TT_PLATFORM_ADOBE:
        switch (encoding_id) {
        case TT_ADOBE_ID_STANDARD:
            *encoding = "adobestdenc";
            break;

        case TT_ADOBE_ID_EXPERT:
            /* We'll have to make this one by hand. */
            break;

        case TT_ADOBE_ID_CUSTOM:
            /* Note sure what this is supposed to indicate. */
            break;

        case TT_ADOBE_ID_LATIN_1:
            *encoding = "latin-1";
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    if (encoding == NULL) {
        PyErr_Format(
            PyExc_ValueError,
            "Text is in an unknown encoding (%d/%d)",
            platform_id, encoding_id);

        return -1;
    }

    return 0;
}


int warn_fallback_message(const char *encoding, const char *fallback_encoding)
{
    char buff[120];

    if (PyErr_ExceptionMatches(PyExc_LookupError)) {
        PyOS_snprintf(
                buff, 120,
                "The exact encoding '%s' was not found on this Python, "
                "installation, so falling back to '%s'",
                encoding, fallback_encoding);

        if (PyErr_Warn(PyExc_UnicodeWarning, buff)) {
            return -1;
        }
    }

    return 0;
}


PyObject *ftpy_decode(
        unsigned short platform_id, unsigned short encoding_id,
        const char *string, Py_ssize_t string_len)
{
    const char *encoding;
    const char *fallback_encoding;
    PyObject *result;

    freetype_to_python_encoding(
            platform_id, encoding_id,
            &encoding, &fallback_encoding);

    if (encoding == NULL) {
        return NULL;
    }

    result = PyUnicode_Decode(string, string_len, encoding, "raise");
    if (result == NULL && fallback_encoding != NULL) {
        if (warn_fallback_message(encoding, fallback_encoding)) {
            return NULL;
        }

        result = PyUnicode_Decode(
                string, string_len, fallback_encoding, "raise");
    }

    return result;
}


int ftpy_get_charcode_from_unicode(
    PyObject *py_unicode, unsigned short platform_id,
    unsigned short encoding_id, unsigned long *charcode)
{
    const char *encoding;
    const char *fallback_encoding;
    PyObject *py_bytes = NULL;
    char *bytes;
    Py_ssize_t bytes_size;
    int result = -1;

    if (!PyUnicode_Check(py_unicode)) {
        PyErr_SetString(
            PyExc_TypeError,
            "Must be a single-character unicode string");
        goto exit;
    }

    if (PyUnicode_GetSize(py_unicode) < 1 ||
        PyUnicode_GetSize(py_unicode) > 2) {
        PyErr_SetString(
            PyExc_ValueError,
            "Must be a single-character unicode string");
        goto exit;
    }

    freetype_to_python_encoding(
            platform_id, encoding_id,
            &encoding, &fallback_encoding);
    if (encoding == NULL) {
        goto exit;
    }

    py_bytes = PyUnicode_AsEncodedString(py_unicode, encoding, "raise");
    if (py_bytes == NULL && fallback_encoding != NULL) {
        if (warn_fallback_message(encoding, fallback_encoding)) {
            goto exit;
        }

        py_bytes = PyUnicode_AsEncodedString(
                py_unicode, fallback_encoding, "raise");
    }

    if (py_bytes == NULL) {
        goto exit;
    }

    if (PyBytes_AsStringAndSize(py_bytes, &bytes, &bytes_size)) {
        goto exit;
    }

    if (bytes_size == 1) {
        *charcode = *(uint8_t *)bytes;
    } else if (bytes_size == 2) {
        *charcode = be16toh(*(uint16_t *)bytes);
    } else if (bytes_size == 4) {
        *charcode = be32toh(*(uint32_t *)bytes);
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Unknown charcode size");
        goto exit;
    }

    result = 0;
 exit:

    Py_XDECREF(py_bytes);
    return result;
}


int ftpy_is_unicode_encoding(
        unsigned short platform_id, unsigned short encoding_id)
{
    int result = 0;

    switch (platform_id) {
    case TT_PLATFORM_APPLE_UNICODE:
        result = 1;
        break;

    case TT_PLATFORM_MACINTOSH:
        break;

    case TT_PLATFORM_ISO:
        result = (encoding_id == TT_ISO_ID_10646);
        break;

    case TT_PLATFORM_MICROSOFT:
        result = (encoding_id == TT_MS_ID_UNICODE_CS ||
                  encoding_id == TT_MS_ID_UCS_4);

    case TT_PLATFORM_ADOBE:
        break;
    }

    return result;
}
