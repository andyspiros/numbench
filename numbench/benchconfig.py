#=====================================================
# Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
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
import sys, os, time
from os.path import join as pjoin
import subprocess as sp
#from benchutils import mkdir
import benchutils as bu

try:
    needsinitialization = not initialized
except NameError:
    needsinitialization = True


if needsinitialization:
    isroot = os.getuid() == 0
    
    try:
        modname = sys.argv[1]
    except:
        modname = ''
    
    # Script directories
    curdir = os.path.abspath('.')
    scriptdir = os.path.dirname(os.path.realpath(__file__))
    if os.environ.has_key('BTLDIR'):
        btldir = os.environ['BTLDIR']
    else:
        btldir = '/usr/include/btl'
    
    # Library directory (lib32 vs. lib64)
    libdir = sp.Popen \
      ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/', \
      stdout=sp.PIPE, shell=True).communicate()[0].strip()
    
    # roots, tests, packages directories -- report, logs base directories
    if isroot:
        testsdir = "/var/tmp/benchmarks/tests/"
        rootsdir = "/var/tmp/benchmarks/roots/"
        pkgsdir = "/var/cache/benchmarks/packages/"
        
        reportdirb = "/var/cache/benchmarks/reports/"
        logdirb = "/var/log/benchmarks/"+modname+"_"+time.strftime('%Y-%m-%d')
    else:
        testsdir = os.environ['HOME'] + "/.benchmarks/tests/"
        rootsdir = os.environ['HOME'] + "/.benchmarks/roots/"
        pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
        reportdirb = os.environ['HOME'] + "/.benchmarks/reports/"
        logdirb = pjoin(os.environ['HOME'], ".benchmarks/log/")
    
    # Report directory
    reportdirb += modname + "_" + time.strftime('%Y-%m-%d')
    if os.path.exists(reportdirb):
        n = 1
        while True:
            reportdir = reportdirb + "_%i" % n
            if not os.path.exists(reportdir):
                break
            n += 1
    else:
        reportdir = reportdirb
    del reportdirb
    
    # Logs directory
    logdirb += modname + "_" + time.strftime('%Y-%m-%d')
    if os.path.exists(logdirb):
        n = 1
        while True:
            logdir = logdirb + "_%i"%n
            if not os.path.exists(logdir):
                break
            n += 1
    else:
        logdir = logdirb
    del logdirb
    
def makedirs():
    bu.mkdir(rootsdir)
    bu.mkdir(testsdir)
    bu.mkdir(pkgsdir)
    bu.mkdir(reportdir)
    bu.mkdir(logdir)
    
def purgedirs():
    bu.rmdir(rootsdir)
    bu.rmdir(testsdir)
    bu.rmdir(pkgsdir)
    bu.rmdir(pjoin(reportdir, '..'))
    bu.rmdir(pjoin(logdir, '..'))
    
    
    
inizialized = True
