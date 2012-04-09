#!/usr/bin/env python

#=====================================================
# Copyright (C) 2011-2012 Andrea Arteaga <andyspiros@gmail.com>
#=====================================================
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

import os
from distutils.core import setup

def recursiveGetPackage(pkg, directory):
    pkg += os.path.basename(directory)
    dirs = [os.path.join(directory, i) for i in os.listdir(directory)]
    dirs = [i for i in dirs if os.path.isdir(i)]
    res = [pkg]
    for i in dirs:
        res += recursiveGetPackage(pkg+'.', i)
    return res

setupdir = os.path.dirname(os.path.abspath(__file__))
numdir = os.path.join(setupdir, 'numbench')


setup(name='Numbench',
      version='1.0',
      description='Numerical benchmarks for Gentoo',
      author='Andrea Arteaga',
      author_email='andyspiros@gmail.com',
      url='http://soc.dev.gentoo.org/~spiros',
      packages=recursiveGetPackage('', numdir),
     )

