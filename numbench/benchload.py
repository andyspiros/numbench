import sys, shlex, threading, time, subprocess as sp

class PrintLoad(threading.Thread):
    
    _active = False
    _continue = True
    
    def run(self):
        while self._continue:
            if self._active:
                out = sp.check_output('uptime')
                printstr = ' '.join(shlex.split(out)[-5:])
                sys.stdout.flush()
                sys.stdout.write(printstr + '\r')
                sys.stdout.flush()
            time.sleep(.5)
            
    def setActive(self, active=True):
        self._active = active
        
    def stop(self):
        self._continue = False



def start():
    printLoadThread.setActive(True)

def stop():
    printLoadThread.setActive(False)
    
def close():
    printLoadThread.stop()
    
try:
    needsinitialization = not initialized
except:
    needsinitialization = True

if needsinitialization:
    printLoadThread = PrintLoad()
    printLoadThread.start()
    
initialized = True