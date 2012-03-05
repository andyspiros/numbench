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
import internal.blasBase as base

class Module:
    libname = 'cblas'
    descr = 'Test module for CBLAS implementations'

    __init__ = base.init
    getImplementations = base.getImplementations
    runTest = base.runTest
    getTests = base.getTests
    reportConf = base.reportConf



if __name__ == '__main__':
    import os
    import benchconfig as cfg
    
    
#    m = Module(('1', 'matrix_matrix'))
    m = Module(('axpy',))
    mytest = dict(
      root='/home/spiros/packages/sci-libs/cblasroot',
      testdir='/home/spiros/tests/cblas-reference',
      logdir='/home/spiros/tests/log/cblas-reference',
      compileenv = {},
      runenv = {'PATH':os.environ['PATH']}
    )
    cfg.libdir = 'usr/lib'
    cfg.btldir = '/home/spiros/hg/btl'
    print m.runTest(mytest, 'reference')