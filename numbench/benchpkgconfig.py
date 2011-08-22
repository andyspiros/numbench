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
import os
from os.path import join as pjoin, basename, dirname, exists
import subprocess as sp

import benchconfig as cfg

def GetFile(file, impl, roots='/'):
    if file[-3:] != '.pc':
        fname = file + '.pc'
    else:
        fname = file
    
    libdir = cfg.libdir
    while libdir[0] == '/':
        libdir = libdir[1:]
    
    # Check alternatives
    if type(roots) == type(''):
        roots = roots,
    pkgcfgpath = ':'.join([pjoin(r,'etc/env.d/alternatives', file, impl, \
      libdir, 'pkgconfig', fname) for r in roots])
     
    if os.path.exists(pkgcfgpath):
        return pkgcfgpath
    else:
        raise Exception('pkg-config fname ' + file + ' not found', pkgcfgpath)
    
def Requires(fname):
    env = {'PKG_CONFIG_PATH' : dirname(fname)}
    cmd = ['pkg-config', '--print-requires', basename(fname)[:-3]]
    proc = sp.Popen(cmd, env=env, stdout=sp.PIPE)
    return proc.communicate()[0].split()
       
    
def Run(fname, root='/', requires=True):
    if not requires:
        lines = file(fname, 'r').readlines()
        newlines = [l for l in lines if l[:10] != 'Requires: ']
        file(fname, 'w').writelines(newlines)
    
    bname = basename(fname)
    if bname[-3:] == '.pc':
        bname = bname[:-3]
        
    env = {'PKG_CONFIG_PATH' : dirname(fname), 'PKG_CONFIG_SYSROOT_DIR' : root}
    cmd = ['pkg-config', '--libs', '--cflags', bname]
    proc = sp.Popen(cmd, env=env, stdout=sp.PIPE)
    out = proc.communicate()[0].strip()
    
    if not requires:
        file(fname, 'w').writelines(lines)
        
    return out