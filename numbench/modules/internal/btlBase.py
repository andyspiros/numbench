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
from os.path import join as pjoin, exists
import numbench.utils.btl as btl
from numbench.benchprint import Print

def reportConf(*args):
    return {'type':'semilogx', 'xlabel':'size', 'ylabel':'MFlops'}


def runTest(self, test, btlconfig):
    # Check if results already exist
    tmpres = dict( \
      [(i, pjoin(btlconfig['testdir'], 'bench_'+i+'_'+self.libname+'.dat')) \
      for i in btlconfig['tests']])

    if all([exists(i) for i in tmpres.values()]):
        Print("Results exist - skipping run")
        return tmpres

    # Compile test suite
    ret = btl.compileTest(test, btlconfig)
    if ret != 0:
        Print("Compilation failed with code: %i" % ret)
        return None
    else:
        Print("Compilation successful")

    # Run test suite
    ret, result = btl.runTest(test, btlconfig)
    if ret != 0:
        Print("Execution failed with code: %i" % ret)
        Print("The results will be incomplete")
        # TODO: remove this if possible (return incomplete results)
        return None
    else:
        Print("Execution successful")

    return result


def getTests(self):
    return self.tests
