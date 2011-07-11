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