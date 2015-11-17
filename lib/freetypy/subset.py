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

"""
A library to subset SFNT-style fonts.
"""

from __future__ import absolute_import, division, unicode_literals, print_function


from collections import OrderedDict
import struct


from freetypy import Face


UNDERSTOOD_VERSIONS = (0x00010000, 0x4f54544f)


class _BinaryStruct(object):
    """
    A wrapper around the Python stdlib struct module to define a
    binary struct more like a dictionary than a tuple.
    """
    def __init__(self, descr, endian='>'):
        """
        Parameters
        ----------
        descr : list of tuple
            Each entry is a pair ``(name, format)``, where ``format``
            is one of the format types understood by `struct`.
        endian : str, optional
            The endianness of the struct.  Must be ``>`` or ``<``.
        """
        self._fmt = [endian]
        self._offsets = {}
        self._names = []
        i = 0
        for name, fmt in descr:
            self._fmt.append(fmt)
            self._offsets[name] = (i, (endian + fmt).encode('ascii'))
            self._names.append(name)
            i += struct.calcsize(fmt.encode('ascii'))
        self._fmt = ''.join(self._fmt).encode('ascii')
        self._size = struct.calcsize(self._fmt)

    @property
    def size(self):
        """
        Return the size of the struct.
        """
        return self._size

    def pack(self, **kwargs):
        """
        Pack the given arguments, which are given as kwargs, and
        return the binary struct.
        """
        fields = [0] * len(self._names)
        for key, val in kwargs.items():
            if key not in self._offsets:
                raise KeyError("No header field '{0}'".format(key))
            i = self._names.index(key)
            fields[i] = val
        return struct.pack(self._fmt, *fields)

    def unpack(self, buff):
        """
        Unpack the given binary buffer into the fields.  The result
        is a dictionary mapping field names to values.
        """
        args = struct.unpack_from(self._fmt, buff[:self._size])
        return dict(zip(self._names, args))

    def read(self, fd):
        """
        Read a struct from the current location in the file.
        """
        buff = fd.read(self.size)
        return self.unpack(buff)

    def write(self, fd, data):
        """
        Write a struct to the current location in the file.
        """
        buff = self.pack(**data)
        fd.write(buff)


class Table(object):
    header_struct = _BinaryStruct([
        ('tag', '4s'),
        ('checkSum', 'I'),
        ('offset', 'I'),
        ('length', 'I')
    ])

    def __init__(self, header, content):
        self._header = header
        self._content = content

    def __repr__(self):
        return "<Table '{0}'>".format(
            self._header['tag'].decode('ascii'))

    def _calc_checksum(self, content):
        end = ((len(content) + 3) & ~3) - 4
        sum = 0
        for i in range(0, end, 4):
            sum += struct.unpack('I', content[i:i+4])[0]
            sum = sum & 0xffffffff
        return sum

    @classmethod
    def read(cls, fd):
        header = Table.header_struct.read(fd)
        content = fd.read(header['length'])

        return cls(header, content)

    @property
    def header(self):
        return self._header

    @property
    def content(self):
        return self._content
    @content.setter
    def content(self, new_content):
        checksum = self._calc_checksum(new_content)
        self._header['checkSum'] = checksum
        self._header['length'] = len(new_content)
        self._content = new_content


class HeadTable(Table):
    head_table_struct = _BinaryStruct([
        ('version', 'I'),
        ('fontRevision', 'I'),
        ('checkSumAdjustment', 'I'),
        ('magicNumber', 'I'),
        ('flags', 'H'),
        ('unitsPerEm', 'H'),
        ('created', 'q'),
        ('modified', 'q'),
        ('xMin', 'h'),
        ('yMin', 'h'),
        ('xMax', 'h'),
        ('yMax', 'h'),
        ('macStyle', 'H'),
        ('lowestRecPPEM', 'H'),
        ('fontDirectionHint', 'h'),
        ('indexToLocFormat', 'h'),
        ('glyphDataFormat', 'h')])

    def __init__(self, header, content):
        super(HeadTable, self).__init__(header, content)

        self.__dict__.update(self.head_table_struct.unpack(content))

        if self.version not in UNDERSTOOD_VERSIONS:
            raise ValueError("Not a TrueType or OpenType file")

        if self.magicNumber != 0x5F0F3CF5:
            raise ValueError("Bad magic number")


class LocaTable(Table):
    def _get_formats(self, fontfile):
        if fontfile[b'head'].indexToLocFormat == 0:  # short offsets
            return '>H', 2, 2
        else:
            return '>I', 4, 1

    def get_offsets(self, fontfile):
        entry_format, entry_size, scale = self._get_formats(fontfile)

        content = self._content
        offsets = []
        for i in range(0, len(content), entry_size):
            value = struct.unpack(
                entry_format, content[i:i+entry_size])[0] * scale
            offsets.append(value)

        return offsets

    def subset(self, fontfile, glyphs, offsets):
        new_offsets = []
        offset = 0
        j = 0
        for i in range(len(offsets) - 1):
            new_offsets.append(offset)
            if j < len(glyphs) and i == glyphs[j]:
                offset += offsets[i+1] - offsets[i]
                j += 1
        new_offsets.append(offset)

        entry_format, entry_size, scale = self._get_formats(fontfile)
        new_content = []
        for value in new_offsets:
            new_content.append(struct.pack(entry_format, value // scale))
        self.content = b''.join(new_content)


class GlyfTable(Table):
    def subset(self, glyphs, offsets):
        content = self.content
        new_content = []
        for gind in glyphs:
            new_content.append(content[offsets[gind]:offsets[gind+1]])
        self.content = b''.join(new_content)


SPECIAL_TABLES = {
    b'head': HeadTable,
    b'loca': LocaTable,
    b'glyf': GlyfTable
}


class FontFile(object):
    """
    A class to subset SFNT-style fonts (TrueType and OpenType).
    """

    header_struct = _BinaryStruct([
        ('version', 'I'),
        ('numTables', 'H'),
        ('searchRange', 'H'),
        ('entrySelector', 'H'),
        ('rangeShift', 'H')
    ])

    def __init__(self, face, header, tables):
        self._face = face
        self._header = header
        self._tables = tables

    def __getitem__(self, tag):
        return self._tables[tag]

    @classmethod
    def read(cls, fd):
        header = cls.header_struct.read(fd)

        if header['version'] not in UNDERSTOOD_VERSIONS:
            raise ValueError("Not a TrueType or OpenType file")

        table_dir = []
        for i in range(header['numTables']):
            table_dir.append(Table.header_struct.read(fd))

        tables = OrderedDict()
        for table_header in table_dir:
            fd.seek(table_header['offset'])
            content = fd.read(table_header['length'])
            table_cls = SPECIAL_TABLES.get(table_header['tag'], Table)
            tables[table_header['tag']] = table_cls(table_header, content)

        fd.seek(0)
        face = Face(fd)

        return cls(face, header, tables)

    def subset(self, ccodes):
        glyphs = []
        for ccode in ccodes:
            glyphs.append(self._face.get_char_index_unicode(ccode))
        glyphs.sort()

        offsets = self[b'loca'].get_offsets(self)
        self[b'glyf'].subset(glyphs, offsets)
        self[b'loca'].subset(self, glyphs, offsets)

    def write(self, fd):
        self.header_struct.write(fd, self._header)

        offset = (self.header_struct.size +
                  Table.header_struct.size * len(self._tables))

        for table in self._tables.values():
            table.header['offset'] = offset
            offset += len(table.content)

        for table in self._tables.values():
            Table.header_struct.write(fd, table.header)

        for table in self._tables.values():
            fd.write(table._content)


def subset_font(input_fd, output_fd, charcodes):
    """
    Subset a SFNT-style (TrueType or OpenType) font.

    Parameters
    ----------
    input_fd : readable file-like object, for bytes
        The font file to read.

    output_fd : writable file-like object, for bytes
        The file to write a subsetted font file to.

    charcodes : list of int or unicode string
        The character codes to include in the output font file.
    """
    fontfile = FontFile.read(input_fd)
    fontfile.subset(charcodes)
    fontfile.write(output_fd)
