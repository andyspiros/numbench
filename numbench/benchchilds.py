try:
    copy = procs
    del copy
except:
    procs = []
    
def terminate():
    for p in procs:
        if p.poll() is None:
            p.kill()

def append(proc):
    procs.append(proc)