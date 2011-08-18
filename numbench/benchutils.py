import os, sys, string, random
import subprocess as sp

__all__ = ['mkdir', 'tmpfile', 'run_cmd']

def mkdir(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
    
def tmpfile(dir="/var/tmp"):
    """Returns the path of a free temporary file within the given directory."""
    chars = string.letters + string.digits
    while True:
        fname = os.path.join(dir, random.sample(chars, 10))
        if not os.path.exists(fname):
            return fname
    
run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]
