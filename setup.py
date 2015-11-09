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

from __future__ import print_function, absolute_import

import versioneer

from distutils.core import setup, Extension
from distutils.command.build_ext import build_ext as BuildExtCommand

import glob
import os
import sys

PY3 = (sys.version_info[0] >= 3)

if PY3:
    import configparser
else:
    import ConfigParser as configparser

sys.path.insert(0, 'buildext')
sys.path.insert(0, 'doc')

import build_local_freetype
import convert_docstrings
import pkgconfig


if __name__ == '__main__':
    convert_docstrings.convert_all_docstrings('docstrings', 'src/doc')

    local_freetype = False
    setup_cfg = 'setup.cfg'
    if os.path.exists(setup_cfg):
        config = configparser.SafeConfigParser()
        config.read(setup_cfg)

        if config.has_option('test', 'local_freetype'):
            local_freetype = True

    local_freetype = (('FREETYPY_LOCAL_FREETYPE' in os.environ) or
                      local_freetype)

    cmdclass = versioneer.get_cmdclass()
    OriginalBuildExt = cmdclass.get('build_ext', BuildExtCommand)

    class BuildLocalFreetype(OriginalBuildExt):
        def run(self):
            if local_freetype:
                build_local_freetype.build_local_freetype()
            return OriginalBuildExt.run(self)

    cmdclass['build_ext'] = BuildLocalFreetype

    extension = Extension(
        'freetypy._freetypy',
        glob.glob('src/*.c') +
        glob.glob('src/doc/*.c'))

    if local_freetype:
        build_local_freetype.set_flags(extension)
    else:
        pkgconfig.pkgconfig.setup_extension(
            extension, 'freetype2', alt_exec='freetype-config')
        extension.define_macros.append(('FREETYPE_BUILD_TYPE', 'system'))

    setup(name="freetypy",
          version=versioneer.get_version(),
          description="Python wrapper for Freetype",
          author="Michael Droettboom",
          author_email="mike@droettboom.com",
          packages=['freetypy', 'freetypy.tests', 'freetypy.codecs'],
          package_dir={'': 'lib'},
          package_data={'freetypy': ['data/*.ttf']},
          ext_modules=[extension],
          cmdclass=cmdclass)
