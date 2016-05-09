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


# The general approach here is to not change any glyph ids, merely to
# remove content for unused glyphs.  This means the character map
# tables don't have to be rewritten.  Additionally, this doesn't break
# random third-party table formats that use glyph ids.  This does mean
# that some space savings are left on the table, but for large Unicode
# fonts, the glyph data itself is comprises the majority of the file
# size, and this approach tackles that handily.


__all__ = ['subset_font']


from collections import OrderedDict
import struct


from freetypy import Face, TT_PLATFORM, TT_ISO_ID, TT_MS_ID


UNDERSTOOD_VERSIONS = (0x00010000, 0x4f54544f)
MAGIC_NUMBER = 0x5F0F3CF5


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


class _Table(object):
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
        header = _Table.header_struct.read(fd)
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


class _HeadTable(_Table):
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
        super(_HeadTable, self).__init__(header, content)

        self.__dict__.update(self.head_table_struct.unpack(content))

        if self.version not in UNDERSTOOD_VERSIONS:
            raise ValueError("Not a TrueType or OpenType file")

        if self.magicNumber != MAGIC_NUMBER:
            raise ValueError("Bad magic number")


class _LocaTable(_Table):
    def _get_formats(self, fontfile):
        if fontfile[b'head'].indexToLocFormat == 0:  # short offsets
            return '>H', 2, 2
        else:
            return '>I', 4, 1

    def get_offsets(self, fontfile):
        entry_format, entry_size, scale = self._get_formats(fontfile)

        content = self.content
        offsets = [
            struct.unpack(
                entry_format, content[i:i+entry_size])[0] * scale
            for i in range(0, len(content), entry_size)
        ]

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
        self.content = b''.join(
            struct.pack(entry_format, value // scale)
            for value in new_offsets)


class _GlyfTable(_Table):
    def find_all_glyphs(self, glyphs, offsets):
        """
        Given a set of glyphs, find all glyphs, including the targets of
        compound glyphs, that are needed to render the glyphs.
        """
        ARG_1_AND_2_ARE_WORDS = 1 << 0
        WE_HAVE_A_SCALE = 1 << 3
        MORE_COMPONENTS = 1 << 5
        WE_HAVE_AN_X_AND_Y_SCALE = 1 << 6
        WE_HAVE_A_TWO_BY_TWO = 1 << 7

        def calculate_skip(flags):
            """
            Calculates the number of bytes to skip to get to the next
            component entry.
            """
            # Numbers can be in bytes or shorts, depending on
            # flag bit
            if flags & ARG_1_AND_2_ARE_WORDS:
                base_size = 2
            else:
                base_size = 1

            nbytes = 4 + base_size * 2
            if flags & WE_HAVE_A_SCALE:
                nbytes += base_size
            elif flags & WE_HAVE_AN_X_AND_Y_SCALE:
                nbytes += base_size * 2
            elif flags & WE_HAVE_A_TWO_BY_TWO:
                nbytes += base_size * 4

            return nbytes

        content = self.content
        all_glyphs = set()
        glyph_queue = glyphs[:]

        while len(glyph_queue):
            gind = glyph_queue.pop(0)
            if gind in all_glyphs:
                continue
            all_glyphs.add(gind)

            glyph = content[offsets[gind]:offsets[gind+1]]
            if len(glyph) == 0:
                continue

            num_contours, = struct.unpack('>h', glyph[0:2])
            if num_contours < 0:  # compound glyph
                # skip over glyph header
                i = 10
                while True:
                    flags, component_gind = struct.unpack('>HH', glyph[i:i+4])
                    glyph_queue.append(component_gind)

                    if not flags & MORE_COMPONENTS:
                        break

                    i += calculate_skip(flags)

        all_glyphs = list(all_glyphs)
        all_glyphs.sort()
        return all_glyphs

    def subset(self, glyphs, offsets):
        content = self.content

        self.content = b''.join(
            content[offsets[gind]:offsets[gind+1]]
            for gind in glyphs)


class _PostTable(_Table):
    post_table_struct = _BinaryStruct([
        ('format', 'I')])

    def __init__(self, header, content):
        super(_PostTable, self).__init__(header, content)

        self.__dict__.update(self.post_table_struct.unpack(content[:4]))

    def _subset_format2(self, glyphs):
        N_BASIC_NAMES = 258

        content = self._content
        i = 32

        numglyphs, = struct.unpack('>H', content[i:i+2])
        i += 2

        new_glyph_index = {}
        needed_indices = {}
        for gind in glyphs:
            if gind < numglyphs:
                offset = i + 2 * gind
                name_index, = struct.unpack('>H', content[offset:offset+2])
                if name_index < N_BASIC_NAMES:
                    new_glyph_index[gind] = name_index
                elif (name_index >= N_BASIC_NAMES and
                      name_index < numglyphs - N_BASIC_NAMES):
                    needed_indices[name_index - N_BASIC_NAMES] = gind

        names = [b'.removed']
        name_index = 0
        i += 2 * numglyphs
        while i < len(content):
            name_length, = struct.unpack('>B', content[i:i+1])
            i += 1
            if name_index in needed_indices:
                name = content[i:i+name_length]
                new_glyph_index[needed_indices[name_index]] = (
                    len(names) + N_BASIC_NAMES)
                names.append(name)
            i += name_length
            name_index += 1

        new_content = [content[0:36]]
        for i in range(1, numglyphs):
            val = new_glyph_index.get(i, N_BASIC_NAMES)
            new_content.append(struct.pack('>H', val))

        for name in names:
            new_content.append(struct.pack('>B', len(name)))
            new_content.append(name)

        return b''.join(new_content)

    def subset(self, glyphs):
        if self.format == 0x20000:
            self.content = self._subset_format2(glyphs)


class _HheaTable(_Table):
    hhea_table_struct = _BinaryStruct([
        ('version', 'I'),
        ('ascent', 'h'),
        ('descent', 'h'),
        ('lineGap', 'h'),
        ('advanceWidthMax', 'H'),
        ('minLeftSideBearing', 'h'),
        ('minRightSideBearing', 'h'),
        ('xMaxExtent', 'h'),
        ('caretSlopeRise', 'h'),
        ('caretSlopeRun', 'h'),
        ('caretOffset', 'h'),
        ('res0', 'h'),
        ('res1', 'h'),
        ('res2', 'h'),
        ('res3', 'h'),
        ('metricDataFormat', 'h'),
        ('numOfLongHorMetrics', 'H')])

    def __init__(self, header, content):
        super(_HheaTable, self).__init__(header, content)

        self.__dict__.update(self.hhea_table_struct.unpack(content))


class _HmtxTable(_Table):
    def subset(self, glyph_set, offsets, hhea):
        # In keeping with not changing glyph ids, we can't actually
        # remove entries here.  However, we can set unused entries to
        # 0 which should aid in compression.

        n_glyphs = len(offsets) - 1
        n_long_hor_metrics = hhea.numOfLongHorMetrics
        content = self.content

        h_metrics = content[:n_long_hor_metrics*4]
        new_values = []
        for i in range(n_long_hor_metrics):
            if i in glyph_set:
                new_values.append(h_metrics[i*4:i*4+4])
            else:
                new_values.append(b'\0\0\0\0')

        left_side_bearing = content[n_long_hor_metrics*4:]
        for i in range(n_glyphs - n_long_hor_metrics):
            if i + n_long_hor_metrics in glyph_set:
                new_values.append(left_side_bearing[i*2:i*2+2])
            else:
                new_values.append(b'\0\0')

        self.content = b''.join(new_values)


class _CmapTable(_Table):
    cmap_table_struct = _BinaryStruct([
        ('version', 'H'),
        ('numTables', 'H')])

    cmap_subtable_struct = _BinaryStruct([
        ('platformID', 'H'),
        ('encodingID', 'H'),
        ('offset', 'I')])

    class _CmapSubtable(object):
        format_12_struct = _BinaryStruct([
            ('format', 'H'),
            ('unused', 'H'),
            ('length', 'I'),
            ('language', 'I'),
            ('n_groups', 'I')])

        def __init__(self, content, offset, glyphs):
            self.offset = offset

            format, = struct.unpack('>H', content[offset:offset+2])
            if format in (0, 2, 4, 6):
                self.length, = struct.unpack(
                    '>H', content[offset+2:offset+4])
            elif format in (8, 10, 12, 13, 14):
                self.length, = struct.unpack(
                    '>I', content[offset+4:offset+8])
            else:
                raise ValueError("Unknown cmap table type")

            self.format = format
            self.content = content[self.offset:self.offset+self.length]

            if format == 12:
                self._subset_format12(glyphs)

        def _subset_format12(self, glyph_set):
            content = self.content
            header = self.format_12_struct.unpack(content[:16])
            chars = []
            for i in range(header['n_groups']):
                start_char, end_char, start_glyph = struct.unpack(
                    '>III', content[16+(i*12):28+(i*12)])

                for ccode in range(start_char, end_char + 1):
                    gind = start_glyph + (ccode - start_char)
                    if gind in glyph_set:
                        chars.append((ccode, gind))

            if len(chars) < 2:
                return

            new_groups = [[chars[0][0], chars[0][0], chars[0][1]]]
            last_ccode = chars[0][0]
            last_gind = chars[0][1]

            for ccode, gind in chars[1:]:
                if gind - ccode == last_gind - last_ccode:
                    new_groups[-1][1] = ccode
                else:
                    new_groups.append([ccode, ccode, gind])
                last_ccode = ccode
                last_gind = gind

            new_content = [
                self.format_12_struct.pack(
                    format=12, unused=0, length=16 + 12 * len(new_groups),
                    language=header['language'], n_groups=len(new_groups))]

            for start, end, gind in new_groups:
                new_content.append(struct.pack('>III', start, end, gind))

            self.content = b''.join(new_content)
            self.length = len(self.content)

    def is_unicode_table(self, header):
        if header['platformID'] == TT_PLATFORM.APPLE_UNICODE:
            return True
        elif (header['platformID'] == TT_PLATFORM.ISO and
              header['encodingID'] == TT_ISO_ID.ISO_10646):
            return True
        elif (header['platformID'] == TT_PLATFORM.MICROSOFT and
              header['encodingID'] in (TT_MS_ID.UNICODE_CS, TT_MS_ID.UCS_4)):
            return True
        return False

    def subset(self, glyph_set):
        # This removes all but the master unicode table.  We could
        # probably do more by shrinking that table, but this is good
        # as a first pass
        content = self.content

        header = self.cmap_table_struct.unpack(content[:4])

        i = 4
        tables = {}
        entries = []
        for table_num in range(header['numTables']):
            subheader = self.cmap_subtable_struct.unpack(content[i:i+8])
            if self.is_unicode_table(subheader):
                if subheader['offset'] in tables:
                    table = tables[subheader['offset']]
                else:
                    try:
                        table = self._CmapSubtable(content, subheader['offset'], glyph_set)
                    except ValueError:
                        # If unknown cmap table types, just abort on subsetting
                        return
                    tables[subheader['offset']] = table
                entries.append((subheader, table))
            i += 8

        # If we don't have a Unicode table, just leave everything intact
        if len(entries) == 0:
            return

        tables = list(tables.values())
        offset = 4 + len(entries) * 8
        for table in tables:
            table.offset = offset
            offset += table.length

        new_content = [
            self.cmap_table_struct.pack(
                version=header['version'],
                numTables=len(entries))]

        for subheader, table in entries:
            new_content.append(self.cmap_subtable_struct.pack(
                platformID=subheader['platformID'],
                encodingID=subheader['encodingID'],
                offset=table.offset))

        for table in tables:
            new_content.append(table.content)

        self.content = b''.join(new_content)


SPECIAL_TABLES = {
    b'cmap': _CmapTable,
    b'head': _HeadTable,
    b'hhea': _HheaTable,
    b'hmtx': _HmtxTable,
    b'loca': _LocaTable,
    b'glyf': _GlyfTable,
    b'post': _PostTable
}


class _FontFile(object):
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

    def __hasitem__(self, tag):
        return tag in self._tables

    @classmethod
    def read(cls, fd, tables_to_remove=[]):
        header = cls.header_struct.read(fd)

        if header['version'] not in UNDERSTOOD_VERSIONS:
            raise ValueError("Not a TrueType or OpenType file")

        table_dir = []
        for i in range(header['numTables']):
            table_dir.append(_Table.header_struct.read(fd))

        tables = OrderedDict()
        for table_header in table_dir:
            fd.seek(table_header['offset'])
            content = fd.read(table_header['length'])
            if table_header['tag'] in tables_to_remove:
                continue
            table_cls = SPECIAL_TABLES.get(table_header['tag'], _Table)
            tables[table_header['tag']] = table_cls(table_header, content)

        fd.seek(0)
        face = Face(fd)

        return cls(face, header, tables)

    def subset(self, ccodes):
        if (b'loca' not in self._tables or
                b'glyf' not in self._tables):
            # No outlines found, so can not subset
            return

        # Always include glyph 0
        glyphs = [0]
        for ccode in ccodes:
            glyphs.append(self._face.get_char_index_unicode(ccode))

        offsets = self[b'loca'].get_offsets(self)
        # Find all glyphs used, including components of compound
        # glyphs
        glyphs = self[b'glyf'].find_all_glyphs(glyphs, offsets)

        glyph_set = set(glyphs)

        self[b'glyf'].subset(glyphs, offsets)
        self[b'loca'].subset(self, glyphs, offsets)
        if b'post' in self._tables:
            self[b'post'].subset(glyphs)
        if b'hmtx' in self._tables and b'hhea' in self._tables:
            self[b'hmtx'].subset(glyph_set, offsets, self[b'hhea'])
        if b'cmap' in self._tables:
            self[b'cmap'].subset(glyph_set)

    def write(self, fd):
        self._header['numTables'] = len(self._tables)

        self.header_struct.write(fd, self._header)

        offset = (self.header_struct.size +
                  _Table.header_struct.size * len(self._tables))

        for table in self._tables.values():
            table.header['offset'] = offset
            offset += len(table.content)

        for table in self._tables.values():
            _Table.header_struct.write(fd, table.header)

        for table in self._tables.values():
            fd.write(table._content)


def subset_font(input_fd, output_fd, charcodes, tables_to_remove=None):
    """
    Subset a SFNT-style (TrueType or OpenType) font.

    If the font is not one of these types, a ValueError is raised.

    Parameters
    ----------
    input_fd : readable file-like object, for bytes
        The font file to read.

    output_fd : writable file-like object, for bytes
        The file to write a subsetted font file to.

    charcodes : list of int or unicode string
        The character codes to include in the output font file.

    tables_to_remove : list of bytes, optional
        The tags of tables to remove completely.  If not provided,
        this defaults to:

           [b'GPOS', b'GSUB']
    """
    if tables_to_remove is None:
        tables_to_remove = [b'GPOS', b'GSUB']

    fontfile = _FontFile.read(input_fd, tables_to_remove)
    fontfile.subset(charcodes)
    fontfile.write(output_fd)
