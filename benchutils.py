import os, sys
from os.path import join as pjoin, basename, dirname
import subprocess as sp
import benchconfig as cfg
import benchpkgconfig as pc

def mkdir(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
        
run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]