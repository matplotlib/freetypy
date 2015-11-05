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


import hashlib
import os
import subprocess
import sys


# This is the version of FreeType to use when building a local
# version.  It must match the value in
# lib/matplotlib.__init__.py
LOCAL_FREETYPE_VERSION = '2.6.1'
# md5 hash of the freetype tarball
LOCAL_FREETYPE_HASH = '348e667d728c597360e4a87c16556597'


def get_file_hash(filename):
    """
    Get the MD5 hash of a given filename.
    """
    BLOCKSIZE = 1 << 16
    hasher = hashlib.md5()
    with open(filename, 'rb') as fd:
        buf = fd.read(BLOCKSIZE)
        while len(buf) > 0:
            hasher.update(buf)
            buf = fd.read(BLOCKSIZE)
    return hasher.hexdigest()


def build_local_freetype():
    src_path = os.path.join(
        'build', 'freetype-{0}'.format(LOCAL_FREETYPE_VERSION))

    # We've already built freetype
    if os.path.isfile(os.path.join(src_path, 'objs', '.libs', 'libfreetype.a')):
        return

    tarball = 'freetype-{0}.tar.gz'.format(LOCAL_FREETYPE_VERSION)
    tarball_path = os.path.join('build', tarball)
    if not os.path.isfile(tarball_path):
        tarball_url = 'http://download.savannah.gnu.org/releases/freetype/{0}'.format(tarball)

        print("Downloading {0}".format(tarball_url))
        if sys.version_info[0] == 2:
            from urllib import urlretrieve
        else:
            from urllib.request import urlretrieve

        if not os.path.exists('build'):
            os.makedirs('build')
        urlretrieve(tarball_url, tarball_path)

        if get_file_hash(tarball_path) != LOCAL_FREETYPE_HASH:
            raise IOError("{0} does not match expected hash.".format(tarball))

    print("Building {0}".format(tarball))
    cflags = 'CFLAGS="{0} -fPIC" '.format(os.environ.get('CFLAGS', ''))

    subprocess.check_call(
        ['tar', 'zxf', tarball], cwd='build')
    subprocess.check_call(
        [cflags + './configure --with-zlib=no --with-bzip2=no '
         '--with-png=no --with-harfbuzz=no'], shell=True, cwd=src_path)
    subprocess.check_call(
        [cflags + 'make'], shell=True, cwd=src_path)


def set_flags(ext):
    src_path = os.path.join(
                'build', 'freetype-{0}'.format(LOCAL_FREETYPE_VERSION))

    # Statically link to the locally-built freetype.
    # This is certainly broken on Windows.
    ext.include_dirs.insert(0, os.path.join(src_path, 'include'))
    ext.extra_objects.insert(
        0, os.path.join(src_path, 'objs', '.libs', 'libfreetype.a'))
    ext.define_macros.append(('FREETYPE_BUILD_TYPE', 'local'))
