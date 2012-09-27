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
import sys, os, time, subprocess as sp
from os.path import join as pjoin


# Arguments
arguments = None
inputfile = None
clean = None
imageformat = None

# Directories
curdir = None
scriptdir = None
btldir = None
libdir = None

# Storage directories
basedir = None
testsdir = None
rootsdir = None
pkgsdir = None
reportdir = None
logdir = None
copyreport = None

# Module
module = None
modulename = None
moduleargs = None
reportTitle = None

# Other
isroot = not os.getuid()
tests = None


def parseArguments():
    global arguments, inputfile, clean, imageformat, basedir, copyreport

    arguments = []
    clean = False
    imageformat = 'svg'

    # Directories have not yet been set up
    curdir = os.path.abspath('.')

    skipargs = 0
    for i, a in enumerate(sys.argv[1:], 1):
        if skipargs > 0:
            skipargs -= 1
            continue

        if a[0] != '-':
            inputfile = os.path.realpath(a)
            continue

        if a in ('-d', '--directory'):
            basedir = pjoin(curdir, sys.argv[i + 1])
            skipargs += 1
            continue

        if a in ('-c', '--clean'):
            clean = True
            continue

        if a in ('-i', '--imageformat'):
            imageformat = sys.argv[i + 1]
            skipargs += 1
            continue
        
        if a in ('-r', '--copy-report'):
            copyreport = sys.argv[i + 1]
            skipargs += 1
            continue

        arguments.append(a)



def setDirs():
    global curdir, scriptdir, btldir, libdir, basedir
    global testsdir, rootsdir, pkgsdir, reportdir, logdir


    # Script directories
    curdir = os.path.abspath('.')
    scriptdir = os.path.dirname(os.path.realpath(__file__))
    if os.environ.has_key('BTLDIR'):
        btldir = os.environ['BTLDIR']
    else:
        btldir = '/usr/include/numbench/btl'

    # Library directory (lib vs. lib32 vs. lib64)
    libdir = sp.Popen \
      ('ABI=$(portageq envvar ABI); echo `portageq envvar LIBDIR_$ABI`', \
      stdout=sp.PIPE, shell=True).communicate()[0].strip()
    if not libdir:
        libdir = 'usr/lib'
    else:
        libdir = 'usr/' + libdir

    # Storage directories
    if basedir is None:
        if modulename is None:
            raise RuntimeError("Module is not defined")

        basedirb = pjoin(os.environ['HOME'], '.numbench') \
                + '/numbench_' + modulename + '_' + time.strftime('%Y-%m-%d')

        # Find suitable base directory
        if os.path.exists(basedirb):
            n = 1
            while True:
                basedir = basedirb + "_%i" % n
                if not os.path.exists(basedir):
                    break
                n += 1
        else:
            basedir = basedirb
        del basedirb

    testsdir, rootsdir, pkgsdir, reportdir, logdir = tuple([pjoin(basedir, i) \
      for i in ('tests', 'roots', 'packages', 'report', 'log')])
