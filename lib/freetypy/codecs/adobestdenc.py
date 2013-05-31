""" Python Character Mapping Codec adobestdenc generated from 'MAPPINGS/VENDORS/ADOBE/stdenc.txt' with gencodec.py.

"""#"

import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):
        return codecs.charmap_encode(input,errors,encoding_map)

    def decode(self,input,errors='strict'):
        return codecs.charmap_decode(input,errors,decoding_map)

class IncrementalEncoder(codecs.IncrementalEncoder):
    def encode(self, input, final=False):
        return codecs.charmap_encode(input,self.errors,encoding_map)[0]

class IncrementalDecoder(codecs.IncrementalDecoder):
    def decode(self, input, final=False):
        return codecs.charmap_decode(input,self.errors,decoding_map)[0]

class StreamWriter(Codec,codecs.StreamWriter):
    pass

class StreamReader(Codec,codecs.StreamReader):
    pass

### encodings module API

def getregentry():
    return codecs.CodecInfo(
        name='adobestdenc',
        encode=Codec().encode,
        decode=Codec().decode,
        incrementalencoder=IncrementalEncoder,
        incrementaldecoder=IncrementalDecoder,
        streamreader=StreamReader,
        streamwriter=StreamWriter,
    )


### Decoding Map

decoding_map = codecs.make_identity_dict(range(256))
decoding_map.update({
    0x27: 0x00A9,       #  APOSTROPHE   # quotesingle
    0x60: 0x00C1,       #  GRAVE ACCENT # grave
    0x80: None,
    0x81: None,
    0x82: None,
    0x83: None,
    0x84: None,
    0x85: None,
    0x86: None,
    0x87: None,
    0x88: None,
    0x89: None,
    0x8A: None,
    0x8B: None,
    0x8C: None,
    0x8D: None,
    0x8E: None,
    0x8F: None,
    0x90: None,
    0x91: None,
    0x92: None,
    0x93: None,
    0x94: None,
    0x95: None,
    0x96: None,
    0x97: None,
    0x98: None,
    0x99: None,
    0x9A: None,
    0x9B: None,
    0x9C: None,
    0x9D: None,
    0x9E: None,
    0x9F: None,
    0xA0: 0x0020,       #  NO-BREAK SPACE       # space
    0xA4: 0x00A8,       #  CURRENCY SIGN        # currency
    0xA6: None,
    0xA8: 0x00C8,       #  DIAERESIS    # dieresis
    0xA9: None,
    0xAA: 0x00E3,       #  FEMININE ORDINAL INDICATOR   # ordfeminine
    0xAC: None,
    0xAD: 0x002D,       #  SOFT HYPHEN  # hyphen
    0xAE: None,
    0xAF: 0x00C5,       #  MACRON       # macron
    0xB0: None,
    0xB1: None,
    0xB2: None,
    0xB3: None,
    0xB4: 0x00C2,       #  ACUTE ACCENT # acute
    0xB5: None,
    0xB7: 0x00B4,       #  MIDDLE DOT   # periodcentered
    0xB8: 0x00CB,       #  CEDILLA      # cedilla
    0xB9: None,
    0xBA: 0x00EB,       #  MASCULINE ORDINAL INDICATOR  # ordmasculine
    0xBC: None,
    0xBD: None,
    0xBE: None,
    0xC0: None,
    0xC1: None,
    0xC2: None,
    0xC3: None,
    0xC4: None,
    0xC5: None,
    0xC6: 0x00E1,       #  LATIN CAPITAL LETTER AE      # AE
    0xC7: None,
    0xC8: None,
    0xC9: None,
    0xCA: None,
    0xCB: None,
    0xCC: None,
    0xCD: None,
    0xCE: None,
    0xCF: None,
    0xD0: None,
    0xD1: None,
    0xD2: None,
    0xD3: None,
    0xD4: None,
    0xD5: None,
    0xD6: None,
    0xD7: None,
    0xD8: 0x00E9,       #  LATIN CAPITAL LETTER O WITH STROKE   # Oslash
    0xD9: None,
    0xDA: None,
    0xDB: None,
    0xDC: None,
    0xDD: None,
    0xDE: None,
    0xDF: 0x00FB,       #  LATIN SMALL LETTER SHARP S   # germandbls
    0xE0: None,
    0xE1: None,
    0xE2: None,
    0xE3: None,
    0xE4: None,
    0xE5: None,
    0xE6: 0x00F1,       #  LATIN SMALL LETTER AE        # ae
    0xE7: None,
    0xE8: None,
    0xE9: None,
    0xEA: None,
    0xEB: None,
    0xEC: None,
    0xED: None,
    0xEE: None,
    0xEF: None,
    0xF0: None,
    0xF1: None,
    0xF2: None,
    0xF3: None,
    0xF4: None,
    0xF5: None,
    0xF6: None,
    0xF7: None,
    0xF8: 0x00F9,       #  LATIN SMALL LETTER O WITH STROKE     # oslash
    0xF9: None,
    0xFA: None,
    0xFB: None,
    0xFC: None,
    0xFD: None,
    0xFE: None,
    0xFF: None,
    0x131: 0x00F5,      #  LATIN SMALL LETTER DOTLESS I # dotlessi
    0x141: 0x00E8,      #  LATIN CAPITAL LETTER L WITH STROKE   # Lslash
    0x142: 0x00F8,      #  LATIN SMALL LETTER L WITH STROKE     # lslash
    0x152: 0x00EA,      #  LATIN CAPITAL LIGATURE OE    # OE
    0x153: 0x00FA,      #  LATIN SMALL LIGATURE OE      # oe
    0x192: 0x00A6,      #  LATIN SMALL LETTER F WITH HOOK       # florin
    0x2C6: 0x00C3,      #  MODIFIER LETTER CIRCUMFLEX ACCENT    # circumflex
    0x2C7: 0x00CF,      #  CARON        # caron
    0x2C9: 0x00C5,      #  MODIFIER LETTER MACRON       # macron
    0x2D8: 0x00C6,      #  BREVE        # breve
    0x2D9: 0x00C7,      #  DOT ABOVE    # dotaccent
    0x2DA: 0x00CA,      #  RING ABOVE   # ring
    0x2DB: 0x00CE,      #  OGONEK       # ogonek
    0x2DC: 0x00C4,      #  SMALL TILDE  # tilde
    0x2DD: 0x00CD,      #  DOUBLE ACUTE ACCENT  # hungarumlaut
    0x2013: 0x00B1,     #  EN DASH      # endash
    0x2014: 0x00D0,     #  EM DASH      # emdash
    0x2018: 0x0060,     #  LEFT SINGLE QUOTATION MARK   # quoteleft
    0x2019: 0x0027,     #  RIGHT SINGLE QUOTATION MARK  # quoteright
    0x201A: 0x00B8,     #  SINGLE LOW-9 QUOTATION MARK  # quotesinglbase
    0x201C: 0x00AA,     #  LEFT DOUBLE QUOTATION MARK   # quotedblleft
    0x201D: 0x00BA,     #  RIGHT DOUBLE QUOTATION MARK  # quotedblright
    0x201E: 0x00B9,     #  DOUBLE LOW-9 QUOTATION MARK  # quotedblbase
    0x2020: 0x00B2,     #  DAGGER       # dagger
    0x2021: 0x00B3,     #  DOUBLE DAGGER        # daggerdbl
    0x2022: 0x00B7,     #  BULLET       # bullet
    0x2026: 0x00BC,     #  HORIZONTAL ELLIPSIS  # ellipsis
    0x2030: 0x00BD,     #  PER MILLE SIGN       # perthousand
    0x2039: 0x00AC,     #  SINGLE LEFT-POINTING ANGLE QUOTATION MARK    # guilsinglleft
    0x203A: 0x00AD,     #  SINGLE RIGHT-POINTING ANGLE QUOTATION MARK   # guilsinglright
    0x2044: 0x00A4,     #  FRACTION SLASH       # fraction
    0x2215: 0x00A4,     #  DIVISION SLASH       # fraction
    0x2219: 0x00B4,     #  BULLET OPERATOR      # periodcentered
    0xFB01: 0x00AE,     #  LATIN SMALL LIGATURE FI      # fi
    0xFB02: 0x00AF,     #  LATIN SMALL LIGATURE FL      # fl
})

### Encoding Map

encoding_map = {
    0x0000: 0x00,       #  CONTROL CHARACTER
    0x0001: 0x01,       #  CONTROL CHARACTER
    0x0002: 0x02,       #  CONTROL CHARACTER
    0x0003: 0x03,       #  CONTROL CHARACTER
    0x0004: 0x04,       #  CONTROL CHARACTER
    0x0005: 0x05,       #  CONTROL CHARACTER
    0x0006: 0x06,       #  CONTROL CHARACTER
    0x0007: 0x07,       #  CONTROL CHARACTER
    0x0008: 0x08,       #  CONTROL CHARACTER
    0x0009: 0x09,       #  CONTROL CHARACTER
    0x000A: 0x0A,       #  CONTROL CHARACTER
    0x000B: 0x0B,       #  CONTROL CHARACTER
    0x000C: 0x0C,       #  CONTROL CHARACTER
    0x000D: 0x0D,       #  CONTROL CHARACTER
    0x000E: 0x0E,       #  CONTROL CHARACTER
    0x000F: 0x0F,       #  CONTROL CHARACTER
    0x0010: 0x10,       #  CONTROL CHARACTER
    0x0011: 0x11,       #  CONTROL CHARACTER
    0x0012: 0x12,       #  CONTROL CHARACTER
    0x0013: 0x13,       #  CONTROL CHARACTER
    0x0014: 0x14,       #  CONTROL CHARACTER
    0x0015: 0x15,       #  CONTROL CHARACTER
    0x0016: 0x16,       #  CONTROL CHARACTER
    0x0017: 0x17,       #  CONTROL CHARACTER
    0x0018: 0x18,       #  CONTROL CHARACTER
    0x0019: 0x19,       #  CONTROL CHARACTER
    0x001A: 0x1A,       #  CONTROL CHARACTER
    0x001B: 0x1B,       #  CONTROL CHARACTER
    0x001C: 0x1C,       #  CONTROL CHARACTER
    0x001D: 0x1D,       #  CONTROL CHARACTER
    0x001E: 0x1E,       #  CONTROL CHARACTER
    0x001F: 0x1F,       #  CONTROL CHARACTER
    0x0020: 0xA0,       #  NO-BREAK SPACE       # space
    0x0020: 0x20,       #  SPACE        # space
    0x0021: 0x21,       #  EXCLAMATION MARK     # exclam
    0x0022: 0x22,       #  QUOTATION MARK       # quotedbl
    0x0023: 0x23,       #  NUMBER SIGN  # numbersign
    0x0024: 0x24,       #  DOLLAR SIGN  # dollar
    0x0025: 0x25,       #  PERCENT SIGN # percent
    0x0026: 0x26,       #  AMPERSAND    # ampersand
    0x0027: 0x2019,     #  RIGHT SINGLE QUOTATION MARK  # quoteright
    0x0028: 0x28,       #  LEFT PARENTHESIS     # parenleft
    0x0029: 0x29,       #  RIGHT PARENTHESIS    # parenright
    0x002A: 0x2A,       #  ASTERISK     # asterisk
    0x002B: 0x2B,       #  PLUS SIGN    # plus
    0x002C: 0x2C,       #  COMMA        # comma
    0x002D: 0x2D,       #  HYPHEN-MINUS # hyphen
    0x002D: 0xAD,       #  SOFT HYPHEN  # hyphen
    0x002E: 0x2E,       #  FULL STOP    # period
    0x002F: 0x2F,       #  SOLIDUS      # slash
    0x0030: 0x30,       #  DIGIT ZERO   # zero
    0x0031: 0x31,       #  DIGIT ONE    # one
    0x0032: 0x32,       #  DIGIT TWO    # two
    0x0033: 0x33,       #  DIGIT THREE  # three
    0x0034: 0x34,       #  DIGIT FOUR   # four
    0x0035: 0x35,       #  DIGIT FIVE   # five
    0x0036: 0x36,       #  DIGIT SIX    # six
    0x0037: 0x37,       #  DIGIT SEVEN  # seven
    0x0038: 0x38,       #  DIGIT EIGHT  # eight
    0x0039: 0x39,       #  DIGIT NINE   # nine
    0x003A: 0x3A,       #  COLON        # colon
    0x003B: 0x3B,       #  SEMICOLON    # semicolon
    0x003C: 0x3C,       #  LESS-THAN SIGN       # less
    0x003D: 0x3D,       #  EQUALS SIGN  # equal
    0x003E: 0x3E,       #  GREATER-THAN SIGN    # greater
    0x003F: 0x3F,       #  QUESTION MARK        # question
    0x0040: 0x40,       #  COMMERCIAL AT        # at
    0x0041: 0x41,       #  LATIN CAPITAL LETTER A       # A
    0x0042: 0x42,       #  LATIN CAPITAL LETTER B       # B
    0x0043: 0x43,       #  LATIN CAPITAL LETTER C       # C
    0x0044: 0x44,       #  LATIN CAPITAL LETTER D       # D
    0x0045: 0x45,       #  LATIN CAPITAL LETTER E       # E
    0x0046: 0x46,       #  LATIN CAPITAL LETTER F       # F
    0x0047: 0x47,       #  LATIN CAPITAL LETTER G       # G
    0x0048: 0x48,       #  LATIN CAPITAL LETTER H       # H
    0x0049: 0x49,       #  LATIN CAPITAL LETTER I       # I
    0x004A: 0x4A,       #  LATIN CAPITAL LETTER J       # J
    0x004B: 0x4B,       #  LATIN CAPITAL LETTER K       # K
    0x004C: 0x4C,       #  LATIN CAPITAL LETTER L       # L
    0x004D: 0x4D,       #  LATIN CAPITAL LETTER M       # M
    0x004E: 0x4E,       #  LATIN CAPITAL LETTER N       # N
    0x004F: 0x4F,       #  LATIN CAPITAL LETTER O       # O
    0x0050: 0x50,       #  LATIN CAPITAL LETTER P       # P
    0x0051: 0x51,       #  LATIN CAPITAL LETTER Q       # Q
    0x0052: 0x52,       #  LATIN CAPITAL LETTER R       # R
    0x0053: 0x53,       #  LATIN CAPITAL LETTER S       # S
    0x0054: 0x54,       #  LATIN CAPITAL LETTER T       # T
    0x0055: 0x55,       #  LATIN CAPITAL LETTER U       # U
    0x0056: 0x56,       #  LATIN CAPITAL LETTER V       # V
    0x0057: 0x57,       #  LATIN CAPITAL LETTER W       # W
    0x0058: 0x58,       #  LATIN CAPITAL LETTER X       # X
    0x0059: 0x59,       #  LATIN CAPITAL LETTER Y       # Y
    0x005A: 0x5A,       #  LATIN CAPITAL LETTER Z       # Z
    0x005B: 0x5B,       #  LEFT SQUARE BRACKET  # bracketleft
    0x005C: 0x5C,       #  REVERSE SOLIDUS      # backslash
    0x005D: 0x5D,       #  RIGHT SQUARE BRACKET # bracketright
    0x005E: 0x5E,       #  CIRCUMFLEX ACCENT    # asciicircum
    0x005F: 0x5F,       #  LOW LINE     # underscore
    0x0060: 0x2018,     #  LEFT SINGLE QUOTATION MARK   # quoteleft
    0x0061: 0x61,       #  LATIN SMALL LETTER A # a
    0x0062: 0x62,       #  LATIN SMALL LETTER B # b
    0x0063: 0x63,       #  LATIN SMALL LETTER C # c
    0x0064: 0x64,       #  LATIN SMALL LETTER D # d
    0x0065: 0x65,       #  LATIN SMALL LETTER E # e
    0x0066: 0x66,       #  LATIN SMALL LETTER F # f
    0x0067: 0x67,       #  LATIN SMALL LETTER G # g
    0x0068: 0x68,       #  LATIN SMALL LETTER H # h
    0x0069: 0x69,       #  LATIN SMALL LETTER I # i
    0x006A: 0x6A,       #  LATIN SMALL LETTER J # j
    0x006B: 0x6B,       #  LATIN SMALL LETTER K # k
    0x006C: 0x6C,       #  LATIN SMALL LETTER L # l
    0x006D: 0x6D,       #  LATIN SMALL LETTER M # m
    0x006E: 0x6E,       #  LATIN SMALL LETTER N # n
    0x006F: 0x6F,       #  LATIN SMALL LETTER O # o
    0x0070: 0x70,       #  LATIN SMALL LETTER P # p
    0x0071: 0x71,       #  LATIN SMALL LETTER Q # q
    0x0072: 0x72,       #  LATIN SMALL LETTER R # r
    0x0073: 0x73,       #  LATIN SMALL LETTER S # s
    0x0074: 0x74,       #  LATIN SMALL LETTER T # t
    0x0075: 0x75,       #  LATIN SMALL LETTER U # u
    0x0076: 0x76,       #  LATIN SMALL LETTER V # v
    0x0077: 0x77,       #  LATIN SMALL LETTER W # w
    0x0078: 0x78,       #  LATIN SMALL LETTER X # x
    0x0079: 0x79,       #  LATIN SMALL LETTER Y # y
    0x007A: 0x7A,       #  LATIN SMALL LETTER Z # z
    0x007B: 0x7B,       #  LEFT CURLY BRACKET   # braceleft
    0x007C: 0x7C,       #  VERTICAL LINE        # bar
    0x007D: 0x7D,       #  RIGHT CURLY BRACKET  # braceright
    0x007E: 0x7E,       #  TILDE        # asciitilde
    0x007F: 0x7F,       #  CONTROL CHARACTER
    0x00A1: 0xA1,       #  INVERTED EXCLAMATION MARK    # exclamdown
    0x00A2: 0xA2,       #  CENT SIGN    # cent
    0x00A3: 0xA3,       #  POUND SIGN   # sterling
    0x00A4: 0x2215,     #  DIVISION SLASH       # fraction
    0x00A4: 0x2044,     #  FRACTION SLASH       # fraction
    0x00A5: 0xA5,       #  YEN SIGN     # yen
    0x00A6: 0x192,      #  LATIN SMALL LETTER F WITH HOOK       # florin
    0x00A7: 0xA7,       #  SECTION SIGN # section
    0x00A8: 0xA4,       #  CURRENCY SIGN        # currency
    0x00A9: 0x27,       #  APOSTROPHE   # quotesingle
    0x00AA: 0x201C,     #  LEFT DOUBLE QUOTATION MARK   # quotedblleft
    0x00AB: 0xAB,       #  LEFT-POINTING DOUBLE ANGLE QUOTATION MARK    # guillemotleft
    0x00AC: 0x2039,     #  SINGLE LEFT-POINTING ANGLE QUOTATION MARK    # guilsinglleft
    0x00AD: 0x203A,     #  SINGLE RIGHT-POINTING ANGLE QUOTATION MARK   # guilsinglright
    0x00AE: 0xFB01,     #  LATIN SMALL LIGATURE FI      # fi
    0x00AF: 0xFB02,     #  LATIN SMALL LIGATURE FL      # fl
    0x00B1: 0x2013,     #  EN DASH      # endash
    0x00B2: 0x2020,     #  DAGGER       # dagger
    0x00B3: 0x2021,     #  DOUBLE DAGGER        # daggerdbl
    0x00B4: 0x2219,     #  BULLET OPERATOR      # periodcentered
    0x00B4: 0xB7,       #  MIDDLE DOT   # periodcentered
    0x00B6: 0xB6,       #  PILCROW SIGN # paragraph
    0x00B7: 0x2022,     #  BULLET       # bullet
    0x00B8: 0x201A,     #  SINGLE LOW-9 QUOTATION MARK  # quotesinglbase
    0x00B9: 0x201E,     #  DOUBLE LOW-9 QUOTATION MARK  # quotedblbase
    0x00BA: 0x201D,     #  RIGHT DOUBLE QUOTATION MARK  # quotedblright
    0x00BB: 0xBB,       #  RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK   # guillemotright
    0x00BC: 0x2026,     #  HORIZONTAL ELLIPSIS  # ellipsis
    0x00BD: 0x2030,     #  PER MILLE SIGN       # perthousand
    0x00BF: 0xBF,       #  INVERTED QUESTION MARK       # questiondown
    0x00C1: 0x60,       #  GRAVE ACCENT # grave
    0x00C2: 0xB4,       #  ACUTE ACCENT # acute
    0x00C3: 0x2C6,      #  MODIFIER LETTER CIRCUMFLEX ACCENT    # circumflex
    0x00C4: 0x2DC,      #  SMALL TILDE  # tilde
    0x00C5: 0xAF,       #  MACRON       # macron
    0x00C5: 0x2C9,      #  MODIFIER LETTER MACRON       # macron
    0x00C6: 0x2D8,      #  BREVE        # breve
    0x00C7: 0x2D9,      #  DOT ABOVE    # dotaccent
    0x00C8: 0xA8,       #  DIAERESIS    # dieresis
    0x00CA: 0x2DA,      #  RING ABOVE   # ring
    0x00CB: 0xB8,       #  CEDILLA      # cedilla
    0x00CD: 0x2DD,      #  DOUBLE ACUTE ACCENT  # hungarumlaut
    0x00CE: 0x2DB,      #  OGONEK       # ogonek
    0x00CF: 0x2C7,      #  CARON        # caron
    0x00D0: 0x2014,     #  EM DASH      # emdash
    0x00E1: 0xC6,       #  LATIN CAPITAL LETTER AE      # AE
    0x00E3: 0xAA,       #  FEMININE ORDINAL INDICATOR   # ordfeminine
    0x00E8: 0x141,      #  LATIN CAPITAL LETTER L WITH STROKE   # Lslash
    0x00E9: 0xD8,       #  LATIN CAPITAL LETTER O WITH STROKE   # Oslash
    0x00EA: 0x152,      #  LATIN CAPITAL LIGATURE OE    # OE
    0x00EB: 0xBA,       #  MASCULINE ORDINAL INDICATOR  # ordmasculine
    0x00F1: 0xE6,       #  LATIN SMALL LETTER AE        # ae
    0x00F5: 0x131,      #  LATIN SMALL LETTER DOTLESS I # dotlessi
    0x00F8: 0x142,      #  LATIN SMALL LETTER L WITH STROKE     # lslash
    0x00F9: 0xF8,       #  LATIN SMALL LETTER O WITH STROKE     # oslash
    0x00FA: 0x153,      #  LATIN SMALL LIGATURE OE      # oe
    0x00FB: 0xDF,       #  LATIN SMALL LETTER SHARP S   # germandbls
}
