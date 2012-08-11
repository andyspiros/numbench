#=====================================================
# Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
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
import os
from os.path import basename, dirname, realpath


class ModuleNotFoundException(RuntimeError):
    pass


def getModulesNames():
    files = os.listdir(dirname(realpath(__file__)))
    me = basename(__file__)
    modnames = []
    for f in files:
        if f[-3:] == '.py' and f != me:
            modnames.append(f[:-3])
    return modnames


def loadModule(modname, args=None):
    if not modname in getModulesNames():
        raise ModuleNotFoundException("module " + modname + " not found")

    # Get the arguments string
    args = "" if args is None else args
    args = args if type(args) == type('') else ' '.join(args)

    # Load the module
    tmp = __import__(modname, fromlist=["Module"])
    return tmp.Module(args)
