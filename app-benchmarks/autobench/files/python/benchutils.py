import os
import subprocess as sp

def mkdir(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
        
run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]