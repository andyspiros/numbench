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
import numbench.utils.btl as btl
import numbench.utils.alternatives as alt
import btlBase
from os.path import join as pjoin


availableTests = ('general_solve', 'least_squares', 'lu_decomp', 'cholesky', \
                  'qr_decomp', 'svd_decomp', 'syev', 'stev', 'symm_ev')
defaultTests = ('lu_decomp', 'cholesky', 'qr_decomp', 'svd_decomp', 'syev')


def init(self, args):
    self.tests = btl.selectTests(availableTests, args)
    if len(self.tests) == 0:
        self.tests = defaultTests


def getImplementations(self, test):
    return alt.getImplementations(test['root'], self.libname)


def runTest(self, test, implementation):
    # Set up btlconfig
    btlconfig = dict (
      source = 'libs/LAPACK/main.cpp',
      exe = pjoin(test['testdir'], implementation, 'test'),
      logdir = pjoin(test['logdir'], implementation),
      testdir = pjoin(test['testdir'], implementation),
      btlincludes = ('libs/BLAS', 'libs/LAPACK'),
      defines = ('LAPACKNAME='+self.libname, self.libname.upper()+"_INTERFACE"),
      flags = alt.getFlags(test, self.libname, implementation),
      tests = self.tests
    )

    return btlBase.runTest(self, test, btlconfig)

getTests = btlBase.getTests
reportConf = btlBase.reportConf
