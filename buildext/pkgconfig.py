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

from distutils import sysconfig
import os
import subprocess
import sys

if sys.platform != 'win32':
    if sys.version_info[0] < 3:
        from commands import getstatusoutput
    else:
        from subprocess import getstatusoutput

try:
    from subprocess import check_output
except ImportError:
    # check_output is not available in Python 2.6
    def check_output(*popenargs, **kwargs):
        """
        Run command with arguments and return its output as a byte
        string.

        Backported from Python 2.7 as it's implemented as pure python
        on stdlib.
        """
        process = subprocess.Popen(
            stdout=subprocess.PIPE, *popenargs, **kwargs)
        output, unused_err = process.communicate()
        retcode = process.poll()
        if retcode:
            cmd = kwargs.get("args")
            if cmd is None:
                cmd = popenargs[0]
            error = subprocess.CalledProcessError(retcode, cmd)
            error.output = output
            raise error
        return output


def get_base_dirs():
    """
    Returns a list of standard base directories on this platform.
    """
    basedir_map = {
        'win32': ['win32_static'],
        'darwin': ['/usr/local/', '/usr', '/usr/X11', '/opt/local'],
        'gnu0': ['/usr'],
        'aix5': ['/usr/local'],
    }
    return basedir_map.get(sys.platform, ['/usr/local', '/usr'])


class PkgConfig(object):
    """
    This is a class for communicating with pkg-config.
    """
    def __init__(self):
        """
        Determines whether pkg-config exists on this machine.
        """
        if sys.platform == 'win32':
            self.has_pkgconfig = False
        else:
            self.set_pkgconfig_path()
            status, output = getstatusoutput("pkg-config --help")
            self.has_pkgconfig = (status == 0)

    def set_pkgconfig_path(self):
        pkgconfig_path = sysconfig.get_config_var('LIBDIR')
        if pkgconfig_path is None:
            return

        pkgconfig_path = os.path.join(pkgconfig_path, 'pkgconfig')
        if not os.path.isdir(pkgconfig_path):
            return

        try:
            os.environ['PKG_CONFIG_PATH'] += ':' + pkgconfig_path
        except KeyError:
            os.environ['PKG_CONFIG_PATH'] = pkgconfig_path

    def setup_extension(self, ext, package, default_include_dirs=[],
                        default_library_dirs=[], default_libraries=[],
                        alt_exec=None):
        """
        Add parameters to the given `ext` for the given `package`.
        """
        flag_map = {
            '-I': 'include_dirs', '-L': 'library_dirs', '-l': 'libraries'}

        executable = alt_exec
        if self.has_pkgconfig:
            executable = 'pkg-config {0}'.format(package)

        use_defaults = True

        if executable is not None:
            command = "{0} --libs --cflags ".format(executable)

            try:
                output = check_output(command, shell=True,
                                      stderr=subprocess.STDOUT)
            except subprocess.CalledProcessError:
                pass
            else:
                output = output.decode(sys.getfilesystemencoding())
                use_defaults = False
                for token in output.split():
                    attr = flag_map.get(token[:2])
                    if attr is not None:
                        getattr(ext, attr).append(token[2:])

        if use_defaults:
            basedirs = get_base_dirs()
            for base in basedirs:
                for include in default_include_dirs:
                    dir = os.path.join(base, include)
                    if os.path.exists(dir):
                        ext.include_dirs.append(dir)
                for lib in default_library_dirs:
                    dir = os.path.join(base, lib)
                    if os.path.exists(dir):
                        ext.library_dirs.append(dir)
            ext.libraries.extend(default_libraries)
            return True

        return False

    def get_version(self, package):
        """
        Get the version of the package from pkg-config.
        """
        if not self.has_pkgconfig:
            return None

        status, output = getstatusoutput(
            "pkg-config %s --modversion" % (package))
        if status == 0:
            return output
        return None


pkgconfig = PkgConfig()
