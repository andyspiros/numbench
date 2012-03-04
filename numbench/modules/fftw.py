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
from os.path import exists, join as pjoin
from ..utils import btl
from internal import btlBase
from .. import benchconfig as cfg

availableTests = (
    'FFTW_1D_Forward_Measure', 'FFTW_1D_Forward_Estimate',
    'FFTW_1D_Backward_Measure', 'FFTW_1D_Backward_Estimate',

    'FFTW_2D_Forward_Measure', 'FFTW_2D_Forward_Estimate',
    'FFTW_2D_Backward_Measure', 'FFTW_2D_Backward_Estimate',

    'FFTW_3D_Forward_Measure', 'FFTW_3D_Forward_Estimate',
    'FFTW_3D_Backward_Measure', 'FFTW_3D_Backward_Estimate',
)
defaultTests = availableTests


class Module:
    libname = 'fftw'
    descr = 'Test module for FFTW library'

    def __init__(self, args):
        self.tests = btl.selectTests(availableTests, args)
        if len(self.tests) == 0:
            self.tests = defaultTests

    def getImplementations(self, test):
        ret = ['fftw3']
        for impl in ('fftw3_threads', 'fftw3_omp'):
            l = pjoin(test['root'], cfg.libdir, 'lib'+impl+'.so')
            if exists(l):
                ret.append(impl)
        return ret

    def runTest(self, test, implementation):
        # Set up btlconfig
        btlconfig = dict (
            source = 'libs/FFTW/main.cpp',
            exe = pjoin(test['testdir'], implementation, 'test'),
            libraries = [implementation, 'm'],
            logdir = pjoin(test['logdir'], implementation),
            testdir = pjoin(test['testdir'], implementation),
            tests = self.tests
        )

        if implementation != 'fftw3':
            btlconfig['libraries'].append('fftw3')

        return btlBase.runTest(self, test, btlconfig)

    getTests = btlBase.getTests
    reportConf = btlBase.reportConf