import os, sys
from os.path import join as pjoin, basename, dirname
import subprocess as sp
import benchconfig as cfg
import benchpkgconfig as pc

def mkdir(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
        
run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]

    
if __name__ == '__main__':
    cfg.libdir = '/usr/lib64'
    root = '/var/tmp/benchmarks/roots/atlas'
    pfile = pc.GetFile('lapack', 'atlas', root)
    print pc.Run(pfile, root, False)
    
    print pfile
    req = pc.Requires(pfile)
    print req
    change = sys.argv[1:]
    print change
    changes = dict([k.split(':',1) for k in change])
    deps = []
    for r in req:
        if r in changes.keys():
            impl = changes[r]
            pfile = pc.GetFile(r, changes[r])
            print pc.Run(pfile)
        else:
            print pc.Run(r)
        