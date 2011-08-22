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
try:
    needsinitialization = not initialized
except NameError:
    needsinitialization = True


if needsinitialization:
    import benchconfig as cfg, benchutils as bu
    from os.path import dirname, join as pjoin

    class _Print:
        def __init__(self, logfile, maxlevel=10):
            self._level = 0
            self._maxlevel = maxlevel
            self._logfile = logfile
        
        def __call__(self, arg='', end='\n'):
            printstr = str(arg) + end
            if self._level > 0:
                printstr = (self._level-1)*"  " + "-- " + printstr
            
            # Print to logfile
            bu.mkdir(dirname(self._logfile))
            logfile = file(self._logfile, 'a')
            print >> logfile, printstr,
            logfile.close()
            
            # Print to terminal
            if self._level <= self._maxlevel:
                print printstr,
            
        def up(self, n=1):
            self._level = max(self._level-n, 0)
        
        def down(self, n=1):
            self._level = max(self._level+n, 0)
            
    # Initialize main Print object ("static")
    Print = _Print(pjoin(cfg.logdir, 'main.log'), 3)


initialized = True
