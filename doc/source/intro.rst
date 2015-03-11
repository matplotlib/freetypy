Introduction to freetypy
========================

Since freetypy closely follows freetype's native C API, the `freetype
documentation <http://freetype.org/freetype2/documentation.html>`_
serves as a good introduction to freetypy as well.

Basic usage
-----------

The convention for importing ``freetypy`` is::

    >>> import freetypy as ft

First, load a font from a file.  `freetypy` also supports loading a
font from a Python file-like object::

    >>> face = ft.Face("Vera.ttf")

Then the size (in points) must be selected::

    >>> face.set_char_size(48)

Once that setup is performed, a glyph may be loaded.  We pass
``load_flags=ft.LOAD.RENDER`` to tell freetype to load the glyph
outline and then subsequently render it to a bitmap::

    >>> glyph = face.load_char_unicode(u"S", load_flags=ft.LOAD.RENDER)
    <freetypy.Glyph at 0x17afb00>
    >>> bitmap = glyph.bitmap

The bitmap data can be converted to a list::

    >>> bitmap.to_list()
    [[0L, 0L, 0L, 0L, 0L, 0L, 10L, 85L, 142L, 195L, 222L, 238L, 252L,
     243L, 230L, 207L, 170L, 132L, 76L, 17L, 0L, 0L, 0L, 0L, 0L, 0L] ...

However, to display it at the console, it's usually nicer to use the
`freetypy.util.bitmap_to_ascii` helper function::

    >>> ft.util.bitmap_to_ascii(bitmap)
           .+*******++.
         +*############**.
       .*##################
      .*###################
      *####*+.     .++*####
     *###*.            .*##
    .###*.               .*
    +###+
    *###
    *###
    *###
    *###
    *###+
    +####+
     #####*+.
     +#######**+.
      *###########**+.
       +##############*+
        .*##############*.
          .+*############*.
              .++*########*
                   .+*#####*
                      +*####.
                       .*###+
                        +###*
                         ###*
                         ###*
                         ###*
                         ###*
                        +###+
    *.                 .####.
    ##*+              +*###*
    #####*+..      .+*#####.
    #####################*.
    ####################*.
     .+*##############*+
         .++*******+.

This `~freetypy.Bitmap` is a Python buffer object.  If you have Numpy,
you can easily convert this data to an array::

    >>> import numpy as np
    >>> array = np.array(bitmap)

Getting the outline from a glyph
--------------------------------

To get the outline data from a glyph, for example to convert it to
another format, the easiest way is to use
`freetypy.Outline.decompose`::

    class Decomposer(object):
        def __init__(self):
            self.entries = []

        def move_to(self, point):
            self.entries.append(('move_to', point))

        def line_to(self, point):
            self.entries.append(('line_to', point))

        def conic_to(self, a, b):
            self.entries.append(('conic_to', a, b))

        def cubic_to(self, a, b, c):
            self.entries.append(('cubic_to', a, b, c))

    face = ft.Face("Vera.ttf")
    face.set_char_size(12, 12, 300, 300)
    glyph = face.load_char(ord('B'))

    d = Decomposer()
    glyph.outline.decompose(d)
    print(d.entries)

This outputs::

    [('move_to', (640.0, 1088.0)),
     ('line_to', (640.0, 256.0)),
     ('line_to', (1130.0, 256.0)),
     ('conic_to', (1402.0, 256.0), (1533.0, 358.0)),
     ('conic_to', (1664.0, 461.0), (1664.0, 673.0)),
     ('conic_to', (1664.0, 886.0), (1533.0, 987.0)),
     ('conic_to', (1402.0, 1088.0), (1130.0, 1088.0)),
     ('line_to', (640.0, 1088.0)),
     ('move_to', (640.0, 2048.0)),
     ('line_to', (640.0, 1344.0)),
     ('line_to', (1093.0, 1344.0)),
     ('conic_to', (1317.0, 1344.0), (1426.0, 1431.0)),
     ('conic_to', (1536.0, 1518.0), (1536.0, 1696.0)),
     ('conic_to', (1536.0, 1873.0), (1426.0, 1960.0)),
     ('conic_to', (1317.0, 2048.0), (1093.0, 2048.0)),
     ('line_to', (640.0, 2048.0)),
     ('move_to', (320.0, 2304.0)),
     ('line_to', (1115.0, 2304.0)),
     ('conic_to', (1471.0, 2304.0), (1663.0, 2154.0)),
     ('conic_to', (1856.0, 2005.0), (1856.0, 1729.0)),
     ('conic_to', (1856.0, 1516.0), (1756.0, 1389.0)),
     ('conic_to', (1656.0, 1263.0), (1462.0, 1232.0)),
     ('conic_to', (1710.0, 1183.0), (1847.0, 1027.0)),
     ('conic_to', (1984.0, 872.0), (1984.0, 640.0)),
     ('conic_to', (1984.0, 334.0), (1766.0, 167.0)),
     ('conic_to', (1548.0, 0.0), (1146.0, 0.0)),
     ('line_to', (320.0, 0.0)),
     ('line_to', (320.0, 2304.0))]
