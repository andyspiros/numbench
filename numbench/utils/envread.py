#=====================================================
# Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
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
import os, shlex
from os.path import isfile, join as pjoin
from .. import benchconfig as cfg

def transformPaths(root, value):
    paths = value.split(':')
    newpaths = ''
    for p in paths:
        if p[0] == '/':
            newpaths += ':' + (root + p)
        else:
            newpaths += ':' + (p)
    return newpaths
        

def envread(test):
    # Set default paths
    path = pjoin(test['root'], 'bin') + ':' +  pjoin(test['root'], 'usr/bin')
    libpath = pjoin(test['root'], cfg.libdir)
    addenv = dict( PATH=path, LIBRARY_PATH=libpath, LD_LIBRARY_PATH=libpath )
    
    # Merge environment
    for k,v in addenv.items():
        if test['compileenv'].has_key(k):
            test['compileenv'][k] += ':' + v
        else:
            test['compileenv'][k] = v
            
        if test['runenv'].has_key(k):
            test['runenv'][k] += ':' + v
        else:
            test['runenv'][k] = v


    # Read environment files in (root)/etc/env.d
    envdir = pjoin(test['root'], 'etc', 'env.d')
    files = [pjoin(envdir, i) for i in os.listdir(envdir)]
    files = [i for i in files if isfile(i)]
    files.sort()
    
    for envf in files:
        fs = file(envf, 'r')
        for l in fs.readlines():
            l = l.strip().split('=', 1)
            vname = l[0]
            vvalue = shlex.split(l[1])[0]
            
            if vname[-4:] == 'PATH':
                # Append paths in colon-separated list
                newpaths = transformPaths(test['root'], vvalue)
                
                # Compile environment
                if test['compileenv'].has_key(vname):
                    newvar = test['compileenv'][vname] + newpaths
                else:
                    newvar = newpaths[1:]
                test['compileenv'][vname] = newvar
                
                # Run-time environment
                if test['runenv'].has_key(vname):
                    newvar = test['runenv'][vname] + newpaths
                else:
                    newvar = newpaths[1:]
                test['runenv'][vname] = newvar
                
            else:
                # Substitute other variables
                test['compileenv'][vname] = vvalue
                test['runenv'][vname] = vvalue
        
    # Add LDPATH to LIBRARY_PATH and LD_LIBRARY_PATH
    if test['compileenv'].has_key('LDPATH'):
        test['compileenv']['LIBRARY_PATH'] += ':' + test['compileenv']['LDPATH']
        test['compileenv']['LD_LIBRARY_PATH']+= ':'+test['compileenv']['LDPATH']
    if test['runenv'].has_key('LDPATH'):
        test['runenv']['LIBRARY_PATH'] += ':' + test['runenv']['LDPATH']
        test['runenv']['LD_LIBRARY_PATH'] += ':' + test['runenv']['LDPATH']
