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

from os.path import join as pjoin

from internal import btlBase
from ..utils import btl, alternatives as alt

availableTests = ('axpy', 'matrix_vector', 'lu_decomp', 'cholesky',
                  'qr_decomp', 'svd_decomp', 'symm_ev')
defaultTests = availableTests

class Module:
    libname = 'scalapack'
    descr = 'Test module for ScaLAPACK implementations'

    def __init__(self, args):
        passargs = []
        skip = 0
        self.numproc = 4
        for i, a in enumerate(args):
            if skip > 0:
                skip -= 1
                continue
            elif a in ('-n', '--numproc'):
                self.numproc = int(args[i + 1])
            else:
                passargs.append(a)

        self.tests = btl.selectTests(availableTests, args)
        if len(self.tests) == 0:
            self.tests = defaultTests

    def getImplementations(self, test):
        return alt.getImplementations(test['root'], self.libname)

    def runTest(self, test, implementation):
        btlincludes = ['libs/' + i for i in \
                       ('BLAS', 'LAPACK', 'BLACS', 'PBLAS', 'STL')]

        # Set up btlconfig
        btlconfig = dict (
          CXX='mpic++',
          source='libs/PBLAS/main.cpp',
          preargs=('mpirun', '-np', str(self.numproc)),
          exe=pjoin(test['testdir'], implementation, 'test'),
          logdir=pjoin(test['logdir'], implementation),
          testdir=pjoin(test['testdir'], implementation),
          btlincludes=tuple(btlincludes),
          defines=('PBLASNAME=' + self.libname,),
          flags=alt.getFlags(test, self.libname, implementation),
          tests=self.tests
        )

        return btlBase.runTest(self, test, btlconfig)

    getTests = btlBase.getTests
    reportConf = btlBase.reportConf


