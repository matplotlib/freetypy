Freetypy API documentation
==========================

.. currentmodule:: freetypy

The API closely follows the `FreeType API
<http://freetype.org/freetype2/docs/reference/ft2-toc.html>`_.  It is
not 100% complete, but it does contain the functionality that most
would need.

Names have been modified to follow Python's `PEP8 style guidelines
<http://www.python.org/dev/peps/pep-0008/>`_.

Where appropriate, extra API functions have been added to make working
with FreeType more Pythonic.  These methods are marked with the
|freetypy| in the documentation.

Face
----

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   Face
   Size
   Size_Metrics
   FACE_FLAG
   STYLE_FLAG
   LOAD
   SIZE_REQUEST_TYPE
   FSTYPE

Layout
------

|freetypy| Freetypy includes a very basic layout algorithm for
left-to-right text.  For more serious usage, one should use a real
layout engine, such as `Pango <http://www.pango.org/>`_.

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   Layout

Subset
------

|freetypy| Freetypy includes support for subsetting TrueType and
OpenType fonts.

.. autosummary::
   :toctree: _generated
   :template: autosummary/module.rst

   subset

Glyph
-----

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   Glyph
   Glyph_Metrics
   GLYPH_BBOX
   GLYPH_FORMAT
   SubGlyph
   SUBGLYPH_FLAG

Bitmap
------

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   Bitmap
   Bitmap_Size
   PIXEL_MODE
   RENDER_MODE

Outline
-------

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   Outline
   OUTLINE
   ORIENTATION
   CODES

CharMap
-------

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   CharMap
   ENCODING

TrueType information
--------------------

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   SfntName
   SfntNames
   TT_Header
   TT_HoriHeader
   TT_VertHeader
   TT_OS2
   TT_Pclt
   TT_Postscript
   TT_PLATFORM
   TT_APPLE_ID
   TT_MAC_ID
   TT_MAC_LANGID
   TT_MS_ID
   TT_MS_LANGID
   TT_ADOBE_ID
   TT_NAME_ID
   TT_MAC_STYLE
   TT_HEADER_FLAGS
   TT_WIDTH_CLASS
   TT_WEIGHT_CLASS
   TT_FS_SELECTION

Basic Types
-----------

.. autosummary::
   :toctree: _generated
   :template: autosummary/class.rst

   BBox
   Matrix
   Vector

Miscellaneous Utilities
-----------------------

.. autosummary::
   :toctree: _generated
   :template: autosummary/module.rst

   util
