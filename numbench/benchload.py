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