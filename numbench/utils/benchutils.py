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
import os, shutil, string, random
import subprocess as sp

__all__ = ['mkdir', 'tmpfile', 'run_cmd']

def mkdir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)
        
def rmdir(directory):
    if os.path.isdir(directory):
        shutil.rmtree(directory, True)
        
    
def tmpfile(directory="/var/tmp"):
    """Returns the path of a free temporary file within the given directory."""
    chars = string.letters + string.digits
    while True:
        fname = os.path.join(directory, random.sample(chars, 10))
        if not os.path.exists(fname):
            return fname
    
run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]
