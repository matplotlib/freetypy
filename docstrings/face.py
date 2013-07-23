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

Face__init__ = """
A face object models a given typeface, in a given style.

Parameters
----------
file : path, or readable file-like object
    The file containing the face data.

face_index : int, optional
    The index of the face within the font.  The first face has index
    0.

Returns
-------
face : Face instance
"""

Face_ascender = """
The typographic ascender of the face, expressed in font units. For
font formats not having this information, it is set to
`BBox.yMax`. Only relevant for scalable formats.
"""

Face_attach = """
‘Attach’ data to a face object.

Normally, this is used to read additional information for the face
object. For example, you can attach an AFM file that comes with a Type
1 font to get the kerning values and other metrics.

Parameters
----------
file : path, or readable file-like object
    The file containing the data to attach.
"""

Face_available_sizes = """
An list of `Bitmap_Size` objects for all bitmap strikes in the face.
"""

Face_bbox = """
The font bounding box. Coordinates are expressed in font units (see
`~Face.units_per_EM`). The box is large enough to contain any glyph
from the font. Thus, `BBox.yMax` can be seen as the ‘maximum
ascender’, and `BBox.yMin` as the ‘minimum descender’. Only relevant
for scalable formats.

Note that the bounding box might be off by (at least) one pixel for
hinted fonts. See `Size_Metrics` for further discussion.
"""

Face_charmap = """
The current active `CharMap` for this face.
"""

Face_charmaps = """
A list of `CharMap` objects in the face.
"""

Face_descender = """
The typographic descender of the face, expressed in font units. For
font formats not having this information, it is set to
`BBox.yMin`. Note that this field is usually negative. Only relevant
for scalable formats.
"""

Face_face_flags = """
A set of `FACE_FLAG` flags that give important information about the face.
"""

Face_face_index = """
The index of the face in the font file. It is set to 0 if there is
only one face in the font file.
"""

Face_family_name = """
The face's family name. This is an ASCII string, usually in English,
which describes the typeface's family (like ‘Times New Roman’,
‘Bodoni’, ‘Garamond’, etc). This is a least common denominator used to
list fonts. Some formats (TrueType & OpenType) provide localized and
Unicode versions of this string. Applications should use the format
specific interface to access them. Can be `None` (e.g., in fonts
embedded in a PDF file).
"""

Face_filename = """
|freetypy| The argument used to initialize the font.  Is usually a
filename, but not always.
"""

Face_get_char_index_unicode = """
|freetypy| Return the glyph index of a given Unicode character.

Unlike `get_char_index`, if the selected `CharMap` is not Unicode, the
given unicode character will first be encoded to the correct character
map, if possible.

|freetypy| This is a freetypy-specific function.

Parameters
----------
charcode : length-1 unicode string
    The Unicode character.

Returns
-------
glyph_index : int
    The glyph index.  0 means ‘undefined character code’.
"""

Face_get_char_index = """
Return the glyph index of a given character code. This function uses a
charmap object to do the mapping.

Parameters
----------
charcode : int
    The character code.

Returns
-------
glyph_index : int
    The glyph index.  0 means ‘undefined character code’.

Notes
-----
If you use FreeType to manipulate the contents of font files directly,
be aware that the glyph index returned by this function doesn't always
correspond to the internal indices used within the file. This is done
to ensure that value 0 always corresponds to the ‘missing glyph’. If
the first glyph is not named ‘.notdef’, then for Type 1 and Type 42
fonts, ‘.notdef’ will be moved into the glyph ID 0 position, and
whatever was there will be moved to the position ‘.notdef’ had. For
Type 1 fonts, if there is no ‘.notdef’ glyph at all, then one will be
created at index 0 and whatever was there will be moved to the last
index -- Type 42 fonts are considered invalid under this condition.
"""

Face_get_char_name = """
|freetypy| Returns the glyph name of the given unicode code point.
Unlike `get_glyph_name`, this function will always return something,
even if the font contains no glyph names.

Parameters
----------
charcode : int
    The character code.

Returns
-------
glyph_name : unicode
    The name of the associated glyph.
"""

Face_get_char_variant_index = """
Return the glyph index of a given character code as modified by the
variation selector.

Parameters
----------
charcode : int
    The character code point in Unicode.

variantSelector : int
    The Unicode code point of the variation selector.

Returns
-------
glyph_index : int
    The glyph index. 0 means either ‘undefined character code’, or
    ‘undefined selector code’, or ‘no variation selector cmap
    subtable’, or ‘current CharMap is not Unicode’.

Notes
-----
If you use FreeType to manipulate the contents of font files directly,
be aware that the glyph index returned by this function doesn't always
correspond to the internal indices used within the file. This is done
to ensure that value 0 always corresponds to the ‘missing glyph’.

This function is only meaningful if a) the font has a variation
selector cmap sub table, and b) the current charmap has a Unicode
encoding.
"""

Face_get_chars = """
Returns an iterator over all of the character codes in the current
charmap.

Each result of the iterator is a tuple of the form (`charcode`,
`glyph_index`).
"""

Face_get_fstype_flags = """
Return the `FSTYPE` flags for a font, which indicate the licensing for
embedding and subsetting a font.

Returns
-------
flags : int
    A bitfield made up of the `FSTYPE` flags.
"""

Face_get_glyph_name = """
Retrieve the ASCII name of the given glyph in a face.  This only works
for those faces where `has_ps_glyph_names` returns `True`.

Parameters
----------
glyph_index : int
    The glyph index.

Returns
-------
glyph_name : str
    The glyph name.

Notes
-----
An exception is raised if the face doesn't provide glyph names or if the
glyph index is invalid.

Be aware that FreeType reorders glyph indices internally so that glyph
index 0 always corresponds to the ‘missing glyph’ (called ‘.notdef’).
"""

Face_get_kerning = """
Return the kerning vector between two glyphs of a same face.

Parameters
----------
left_glyph : int
    The index of the left glyph in the kern pair.

right_glyph : int
    The index of the right glyph in the kern pair.

kern_mode : int, optional
    A `KERNING` mode.

Returns
-------
vector : Vector
    The kerning between the two glyphs.

Notes
-----
Only horizontal layouts (left-to-right & right-to-left) are supported
by this method. Other layouts, or more sophisticated kernings, are out
of the scope of this API function -- they can be implemented through
format-specific interfaces.
"""

Face_get_name_index = """
Return the glyph index of a given glyph name. This function uses
driver specific objects to do the translation.

Parameters
----------
glyph_name : str
    The glyph name.

Returns
-------
glyph_index : int
    The glyph index.  0 means ‘undefined character code’.
"""

Face_get_postscript_name = """
Retrieve the ASCII PostScript name of a given face, if available.

This only works with PostScript and TrueType fonts.
"""

Face_get_track_kerning = """
Return the track kerning for a given face object at a given size.

Parameters
----------
point_size : float
    The point size in fractional points.

degree : int
    The degree of tightness.  Increasingly negative values represent
    tighter track kerning, while increasingly positive values
    represent looser track kerning. Value zero means no track kerning.

Returns
-------
kerning : float
    The track kerning between the glyphs.

Notes
-----
Currently, only the Type 1 font driver supports track kerning, using
data from AFM files (if attached with `attach`).

Only very few AFM files come with track kerning data; please refer to
the Adobe's AFM specification for more details.
"""

Face_get_tt_header = """
Get the TrueType header (`TT_Header`) from the face.
"""

Face_get_tt_horiheader = """
Get the TrueType horizontal header (`TT_HoriHeader`) from the face.
"""

Face_get_tt_os2 = """
Get the OS/2 TrueType header (`TT_OS2`) from the face.
"""

Face_get_tt_pclt = """
Get the PCLT header (`TT_Pclt`) from the face.
"""

Face_get_tt_postscript = """
Get the Postscript header (`TT_Postscript`) from the face.
"""

Face_get_tt_vertheader = """
Get the TrueType vertical header (`TT_VertHeader`) from the face.
"""

Face_glyph = """
The face's associated glyph slot.
"""

Face_has_ps_glyph_names = """
Return `True` if a given face provides reliable PostScript glyph names.

When this function returns `True`, the caller is sure that the glyph
names returned by `Face.get_glyph_name` are reliable.
"""

Face_height = """
This value is the vertical distance between two consecutive baselines,
expressed in font units. It is always positive. Only relevant for
scalable formats.

If you want the global glyph height, use `ascender` - `descender`.
"""

Face_load_char = """
Load a single glyph, according to its character code.

Parameters
----------
char_code : int
    The glyph's character code, according to the current charmap used
    in the face.

load_flags : int, optional
    A flag indicating what to load for this glyph. The `LOAD`
    constants can be used to control the glyph loading process (e.g.,
    whether the outline should be scaled, whether to load bitmaps or
    not, whether to hint the outline, etc).

Returns
-------
glyph : Glyph
    The loaded `Glyph`.

Notes
------
See `load_glyph` for more information.
"""

Face_load_char_unicode = """
|freetypy| Load a single glyph, from a given unicode character.

Unlike `load_char`, if the selected `CharMap` is not Unicode, the
given unicode character will first be encoded to the correct character
map, if possible.

|freetypy| This is a freetypy-specific function.

Parameters
----------
char_code : unicode character (1-element unicode string)
    The unicode character.

load_flags : int, optional
    A flag indicating what to load for this glyph. The `LOAD`
    constants can be used to control the glyph loading process (e.g.,
    whether the outline should be scaled, whether to load bitmaps or
    not, whether to hint the outline, etc).

Returns
-------
glyph : Glyph
    The loaded `Glyph`.

Notes
------
See `load_glyph` for more information.
"""

Face_load_glyph = """
Load a single glyph.

Parameters
----------
glyph_index	: int
    The index of the glyph in the font file. For CID-keyed fonts
    (either in PS or in CFF format) this argument specifies the CID
    value.

load_flags : int, optional
    A flag indicating what to load for this glyph. The `LOAD`
    constants can be used to control the glyph loading process (e.g.,
    whether the outline should be scaled, whether to load bitmaps or
    not, whether to hint the outline, etc).

Returns
-------
glyph : Glyph
    The loaded `Glyph`.

Notes
-----
By default, hinting is enabled and the font's native hinter (see
`FACE_FLAG.HINTER`) is preferred over the auto-hinter. You can disable
hinting by setting `LOAD.NO_HINTING` or change the precedence by
setting `LOAD.FORCE_AUTOHINT`. You can also set `LOAD.NO_AUTOHINT` in
case you don't want the auto-hinter to be used at all.

See the description of `FACE_FLAG.TRICKY` for a special exception
(affecting only a handful of Asian fonts).

Besides deciding which hinter to use, you can also decide which
hinting algorithm to use. See `LOAD`'s ``TARGET_XXX`` constants for
details.

Note that the auto-hinter needs a valid Unicode cmap (either a native
one or synthesized by FreeType) for producing correct results. If a
font provides an incorrect mapping (for example, assigning the
character code U+005A, LATIN CAPITAL LETTER Z, to a glyph depicting a
mathematical integral sign), the auto-hinter might produce useless
results.

The loaded glyph may be transformed. See `set_transform` for the
details.

For subsetted CID-keyed fonts, a `ValueError` exception is thrown for
invalid CID values (this is, for CID values which don't have a
corresponding glyph in the font). See the discussion of the
`FACE_FLAG.CID_KEYED` flag for more details.

You should use only one of the `LOAD` ``TARGET_XXX`` values in your
`load_flags`. They can't be OR'ed.

If `LOAD.RENDER` is also set, the glyph is rendered in the
corresponding mode (i.e., the mode which matches the used algorithm
best). An exception is `LOAD.TARGET_MONO` since it implies
`LOAD.MONOCHROME`.
"""

Face_max_advance_height = """
The maximum advance height, in font units, for all glyphs in this
face. This is only relevant for vertical layouts, and is set to
`height` for fonts that do not provide vertical metrics. Only relevant
for scalable formats.
"""

Face_max_advance_width = """
The maximum advance width, in font units, for all glyphs in this
face. This can be used to make word wrapping computations faster. Only
relevant for scalable formats.
"""

Face_num_faces = """
The number of faces in the font file. Some font formats can have
multiple faces in a font file.
"""

Face_num_fixed_sizes = """
The number of bitmap strikes in the face. Even if the face is
scalable, there might still be bitmap strikes, which are called
‘sbits’ in that case.
"""

Face_num_glyphs = """
The number of glyphs in the face. If the face is scalable and has
sbits (see `num_fixed_sizes`), it is set to the number of outline
glyphs.

For CID-keyed fonts, this value gives the highest CID used in the
font.
"""

Face_request_size = """
Resize the scale of the active `Size` object in a face.

Parameters
----------
type : size request constant
    The size request type.  See `SIZE_REQUEST_TYPE` for the available
    constants.

width : float, optional
    The desired width.

height : float, optional
    The desired height.

horiResolution : int, optional
    The horizontal resolution.  If set to zero, `width` is treated as a
    fractional pixel value.

vertResolution : int, optional
    The vertical resolution.  If set to zero, `height` is treated as a
    fractional pixel value.

Notes
-----
If `width` is zero, then the horizontal scaling value is set equal to
the vertical scaling value, and vice versa.

Although drivers may select the bitmap strike matching the request,
you should not rely on this if you intend to select a particular
bitmap strike. Use `select_size` instead in that case.

The relation between the requested size and the resulting glyph size
is dependent entirely on how the size is defined in the source
face. The font designer chooses the final size of each glyph relative
to this size. For more information refer to
`<http://www.freetype.org/freetype2/docs/glyphs/glyphs-2.html>`_.

See `Size_Metrics` for how size requesting relates to scaling values.
"""

Face_select_charmap = """
Select a given charmap by its encoding tag.

Parameters
----------
encoding : int
    An encoding constant.  See `ENCODING`.

Notes
-----
This function raises an exception if no charmap in the face corresponds
to the encoding queried here.

Because many fonts contain more than a single cmap for Unicode
encoding, this function has some special code to select the one which
covers Unicode best (‘best’ in the sense that a UCS-4 cmap is
preferred to a UCS-2 cmap). It is thus preferable to use `set_charmap`
in this case.
"""

Face_select_size = """
Select a bitmap strike.

Parameters
----------
strike_index : int
    The index of the bitmap strike in the `~Face.available_sizes`
    field.
"""

Face_set_char_size = """
Request a nominal font size (in points).

Parameters
----------
char_width : float, optional
    The nominal width, in fractional points.

char_height : float, optional
    The nominal height, in fractional points.

horz_resolution : int, optional
    The horizontal resolution, in dpi.

vert_resolution : int, optional
    The vertical resolution, in dpi.

Notes
-----
If either the character width or height is zero (or not provided), it
is set equal to the other value.  |freetypy|: If both width and height
are not provided, it defaults to 12.0.

If either the horizontal or vertical resolution is zero, it is set
equal to the other value. If both resolution values are zero, they are
set to 72dpi.
"""

Face_set_charmap = """
Select a charmap for character code to glyph index mapping.

Parameters
----------
charmap_index : int
    The index in `charmaps` to use.

Notes
-----
This fails if a type 14 charmap is selected.
"""

Face_set_transform = """
Set the transformation that is applied to glyph images.

Parameters
----------
matrix : 2x2 sequence of floats, optional
    A matrix of the form::

        [[xx, xy]
         [yx, yy]]

delta : 2-sequence of floats, optional

Notes
-----
The transformation is only applied to scalable image formats after the
glyph has been loaded. It means that hinting is unaltered by the
transformation and is performed on the character size given in the
last call to `set_char_size` or `set_pixel_sizes`.

Note that this also transforms the `Face.glyph.advance` field, but not
the values in `Face.glyph.metrics`.
"""

Face_sfnt_names = """
Get a `SfntNames` list of `SfntName` entries for the Face.
"""

Face_size = """
The current active size for this face.
"""

Face_style_flags = """
A set of `STYLE_FLAG` flags indicating the style of the face.
"""

Face_style_name = """
The face's style name. This is an ASCII string, usually in English,
which describes the typeface's style (like ‘Italic’, ‘Bold’,
‘Condensed’, etc). Not all font formats provide a style name, so this
field is optional, and can be set to `None`. As for
`~Face.family_name`, some formats provide localized and Unicode
versions of this string. Applications should use the format specific
interface to access them.
"""

Face_underline_position = """
The position, in font units, of the underline line for this face. It
is the center of the underlining stem. Only relevant for scalable
formats.
"""

Face_underline_thickness = """
The thickness, in font units, of the underline for this face. Only
relevant for scalable formats.
"""

Face_units_per_em = """
The number of font units per EM square for this face.

This is typically 2048 for TrueType fonts, and 1000 for Type 1
fonts. Only relevant for scalable formats.
"""

FACE_FLAG = """
A set of bit flags that give important information about the face.

- `SCALABLE`: Indicates that the face contains outline glyphs. This
  doesn't prevent bitmap strikes, i.e., a face can have both this and
  and `FACE_FLAG.FIXED_SIZES` set.

- `FIXED_SIZES`: Indicates that the face contains bitmap strikes. See
  also `Face.available_sizes`.

- `FIXED_WIDTH`: Indicates that the face contains fixed-width
  characters (like Courier, Lucido, MonoType, etc.).

- `SFNT`: Indicates that the face uses the ‘sfnt’ storage scheme. For
  now, this means TrueType and OpenType.

- `HORIZONTAL`: Indicates that the face contains horizontal glyph
  metrics. This should be set for all common formats.

- `VERTICAL`: Indicates that the face contains vertical glyph
  metrics. This is only available in some formats, not all of them.

- `KERNING`: Indicates that the face contains kerning information. If
  set, the kerning distance can be retrieved through
  `Face.get_kerning`. Otherwise the function always returns the vector
  (0, 0). Note that FreeType doesn't handle kerning data from the
  ‘GPOS’ table (as present in some OpenType fonts).

- `MULTIPLE_MASTERS`: Indicates that the font contains multiple
  masters and is capable of interpolating between them. See the
  multiple-masters specific API for details.

- `GLYPH_NAMES`: Indicates that the font contains glyph names that can
  be retrieved through `Face.get_glyph_name`. Note that some TrueType
  fonts contain broken glyph name tables. Use the function
  `Face.has_ps_glyph_names` when needed.

- `EXTERNAL_STREAM`: Used internally by FreeType to indicate that a
  face's stream was provided by the client application and should not
  be destroyed when the face is destructed. Don't read or test this
  flag.

- `HINTER`: Set if the font driver has a hinting machine of its
  own. For example, with TrueType fonts, it makes sense to use data
  from the SFNT ‘gasp’ table only if the native TrueType hinting
  engine (with the bytecode interpreter) is available and active.

- `CID_KEYED`: Set if the font is CID-keyed. In that case, the font is
  not accessed by glyph indices but by CID values. For subsetted
  CID-keyed fonts this has the consequence that not all index values
  are a valid argument to `Face.load_glyph`. Only the CID values for
  which corresponding glyphs in the subsetted font exist make
  `Face.load_glyph` return successfully; in all other cases you get a
  `ValueError` exception.

  Note that CID-keyed fonts which are in an SFNT wrapper don't have
  this flag set since the glyphs are accessed in the normal way (using
  contiguous indices); the ‘CID-ness’ isn't visible to the
  application.

- `TRICKY`: Set if the font is ‘tricky’, this is, it always needs the
  font format's native hinting engine to get a reasonable result. A
  typical example is the Chinese font ‘mingli.ttf’ which uses TrueType
  bytecode instructions to move and scale all of its subglyphs.

  It is not possible to autohint such fonts using
  `LOAD.FORCE_AUTOHINT`; it will also ignore `LOAD.NO_HINTING`. You
  have to set both `LOAD.NO_HINTING` and `LOAD.NO_AUTOHINT` to really
  disable hinting; however, you probably never want this except for
  demonstration purposes.

  Currently, there are about a dozen TrueType fonts in the list of
  tricky fonts; they are hard-coded.
"""

STYLE_FLAG = """
Bit flags to indicate the style of a given face.

- `ITALIC`: Indicates that a given face style is italic
  or oblique.

- `BOLD`: Indicates that a given face is bold.
"""

LOAD = """
A flag indicating what to load for this glyph. The `LOAD` constants
can be used to control the glyph loading process (e.g., whether the
outline should be scaled, whether to load bitmaps or not, whether to
hint the outline, etc).

- `DEFAULT`: Corresponding to 0, this value is used as the default
  glyph load operation. In this case, the following happens:

    1. FreeType looks for a bitmap for the glyph corresponding to the
       face's current size. If one is found, the function returns. The
       bitmap data can be accessed from the glyph slot (see note
       below).

    2. If no embedded bitmap is searched or found, FreeType looks for
       a scalable outline. If one is found, it is loaded from the font
       file, scaled to device pixels, then ‘hinted’ to the pixel grid
       in order to optimize it. The outline data can be accessed from
       the glyph slot (see note below).

  Note that by default, the glyph loader doesn't render outlines into
  bitmaps. The following flags are used to modify this default
  behaviour to more specific and useful cases.

- `NO_SCALE`: Don't scale the loaded outline glyph but keep it in font
  units.

  This flag implies `LOAD.NO_HINTING` and `LOAD.NO_BITMAP`, and unsets
  `LOAD.RENDER`.

  If the font is ‘tricky’ (see `FACE_FLAG.TRICKY` for more), using
  `LOAD.NO_SCALE` usually yields meaningless outlines because the
  subglyphs must be scaled and positioned with hinting
  instructions. This can be solved by loading the font without
  `LOAD.NO_SCALE` and setting the character size to
  `~Face.units_per_em`.

- `NO_HINTING`: Disable hinting. This generally generates ‘blurrier’
  bitmap glyphs when the glyph are rendered in any of the anti-aliased
  modes. See also the note below.

  This flag is implied by `LOAD.NO_SCALE`.

- `RENDER`: Render after the glyph is loaded. By default, the glyph is
  rendered in `RENDER_MODE.NORMAL` mode. This can be overridden by
  `LOAD.TARGET_XXX` or `LOAD.MONOCHROME`.

  This flag is unset by `LOAD.NO_SCALE`.

- `NO_BITMAP`: Ignore bitmap strikes when loading. Bitmap-only fonts
  ignore this flag.

  `LOAD.NO_SCALE` always sets this flag.

- `VERTICAL_LAYOUT`: Load the glyph for vertical text layout. In
  particular, the `advance` value in the `Glyph` object is set to the
  `vert_advance` value of the `metrics` field.

  In case `Face.has_vertical` doesn't return `True`, you shouldn't use
  this flag currently. Reason is that in this case vertical metrics
  get synthesized, and those values are not always consistent across
  various font formats.

- `FORCE_AUTOHINT`: Indicates that the auto-hinter is preferred over
  the font's native hinter. See also the note below.

- `CROP_BITMAP`: Indicates that the font driver should crop the loaded
  bitmap glyph (i.e., remove all space around its black bits). Not all
  drivers implement this.

- `PEDANTIC`: Indicates that the font driver should perform pedantic
  verifications during glyph loading. This is mostly used to detect
  broken glyphs in fonts. By default, FreeType tries to handle broken
  fonts also.

  In particular, errors from the TrueType bytecode engine are not
  passed to the application if this flag is not set; this might result
  in partially hinted or distorted glyphs in case a glyph's bytecode
  is buggy.

- `NO_RECURSE`: This flag is only used internally. It merely indicates
  that the font driver should not load composite glyphs
  recursively. Instead, it should set the `num_subglyph` and
  `subglyphs` values of the `Glyph`, and set `Glyph.format`
  to `GLYPH_FORMAT.COMPOSITE`.

  The description of sub-glyphs is not available to client
  applications for now.

  This flag implies `LOAD.NO_SCALE` and `LOAD.IGNORE_TRANSFORM`.

- `IGNORE_TRANSFORM`: Indicates that the transform matrix set by
  `Face.set_transform` should be ignored.

- `MONOCHROME`: This flag is used with `LOAD.RENDER` to indicate that
  you want to render an outline glyph to a 1-bit monochrome bitmap
  glyph, with 8 pixels packed into each byte of the bitmap data.

  Note that this has no effect on the hinting algorithm used. You
  should rather use `LOAD.TARGET_MONO` so that the
  monochrome-optimized hinting algorithm is used.

- `LINEAR_DESIGN`: Indicates that the `linear_hori_advance` and
  `linear_vert_advance` fields of `Glyph` should be kept in font
  units. See `Glyph` for details.

- `NO_AUTOHINT`: Disable auto-hinter. See also the note below.

The following flags select a specific hinting algorithm to use by the
hinter.

- `TARGET_NORMAL`: This corresponds to the default hinting algorithm,
  optimized for standard gray-level rendering. For monochrome output,
  use `LOAD.TARGET_MONO` instead.

- `TARGET_LIGHT`: A lighter hinting algorithm for non-monochrome
  modes. Many generated glyphs are more fuzzy but better resemble its
  original shape. A bit like rendering on Mac OS X.

  As a special exception, this target implies `LOAD.FORCE_AUTOHINT`.

- `TARGET_MONO`: Strong hinting algorithm that should only be used for
  monochrome output. The result is probably unpleasant if the glyph is
  rendered in non-monochrome modes.

- `TARGET_LCD`: A variant of `LOAD.TARGET_NORMAL` optimized for
  horizontally decimated LCD displays.  On many freetype builds, this
  functionality will be disabled due to patent restrictions, in which
  case the resulting bitmap will be grayscale.

- `TARGET_LCD_V`: Strong hinting algorithm that should only be used
  for monochrome output. The result is probably unpleasant if the
  glyph is rendered in non-monochrome modes. On many freetype builds,
  this functionality will be disabled due to patent restrictions, in
  which case the resulting bitmap will be grayscale.
"""

SIZE_REQUEST_TYPE = """
Modes for selecting the size of a bitmap font.

- `NOMINAL`: The nominal size. The `Face.units_per_EM` is used to
  determine both scaling values.

- `REAL_DIM`: The real dimension. The sum of the the `~Face.ascender`
  and (minus of) the `~Face.descender` are used to determine both
  scaling values.

- `BBOX`: The font bounding box. The width and height of the
  `~Face.bbox` are used to determine the horizontal and vertical
  scaling value, respectively.

- `CELL`: The `~Face.max_advance_width` is used to determine the
  horizontal scaling value; the vertical scaling value is determined
  the same way as `SIZE_REQUEST_TYPE.REAL_DIM` does. Finally, both
  scaling values are set to the smaller one. This type is useful if
  you want to specify the font size for, say, a window of a given
  dimension and 80x24 cells.

- `SCALES`: Specify the scaling values directly.

The above descriptions only apply to scalable formats. For bitmap
formats, the behaviour is up to the driver.
"""

FSTYPE = """
A list of bit flags indicating the embedding and subsetting
restrictions with a font.

- `INSTALLABLE_EMBEDDING`: Fonts with no fsType bit set may be
  embedded and permanently installed on the remote system by an
  application.

- `RESTRICTED_LICENSE_EMBEDDING`: Fonts that have only this bit set
  must not be modified, embedded or exchanged in any manner without
  first obtaining permission of the font software copyright owner.

- `PREVIEW_AND_PRINT_EMBEDDING`: If this bit is set, the font may be
  embedded and temporarily loaded on the remote system. Documents
  containing Preview & Print fonts must be opened ‘read-only’; no
  edits can be applied to the document.

- `EDITABLE_EMBEDDING`: If this bit is set, the font may be embedded
  but must only be installed temporarily on other systems. In contrast
  to Preview & Print fonts, documents containing editable fonts may be
  opened for reading, editing is permitted, and changes may be saved.

- `NO_SUBSETTING`: If this bit is set, the font may not be subsetted
  prior to embedding.

- `BITMAP_EMBEDDING_ONLY`: If this bit is set, only bitmaps contained
  in the font may be embedded; no outline data may be embedded. If
  there are no bitmaps available in the font, then the font is
  unembeddable.
"""

KERNING = """
Kerning modes.

- `DEFAULT`: Return scaled and grid-fitted kerning distances (value is
  0).

- `UNFITTED`: Return scaled but un-grid-fitted kerning distances.

- `UNSCALED`: Return the kerning vector in original font units.
"""

Face_has_horizontal = """
`True` whenever a face object contains horizontal metrics.

This is `True` for all font formats though.
"""

Face_has_vertical = """
`True` whenever a face object contains real vertical metrics.

`False` when the face contains only synthesized vertical metrics.
"""

Face_has_kerning = """
`True` whenever a face object contains kerning data.

The kerning data can be accessed with `Face.get_kerning`.
"""

Face_is_scalable = """
`True` whenever a face object contains a scalable font face.

This includes TrueType, Type 1, Type 42, CID, OpenType/CFF, and PFR
font formats.
"""

Face_is_sfnt = """
`True` whenever a face object contains a font whose format is based on
the SFNT storage scheme.

This usually means: TrueType fonts, OpenType fonts, as well as
SFNT-based embedded bitmap fonts.
"""

Face_is_fixed_width = """
`True` whenever a face object contains some embedded bitmaps.

See `Face.available_sizes`.
"""
