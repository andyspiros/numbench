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
import os, btlbase
import subprocess as sp
import shlex

class Module(btlbase.BTLBase):
    
    libname = "lapack"
    avail = ['general_solve', 'least_squares', 'lu_decomp', \
          'cholesky', 'qr_decomp', 'svd_decomp', 'syev', 'stev', 'symm_ev']
    
    def _initialize(self):
        pass
    
    def _parse_args(self, args):
        passargs = []
        
        # Parse arguments
        tests = []
        for i in args:
            if i in self.avail:
                tests.append(i)
                continue
            passargs.append(i)
        
        # Sort tests
        self.tests = [i for i in self.avail if i in tests]
        
        # If no test is specified, run everything
        if len(self.tests) == 0:
            self.tests = ['lu_decomp', 'cholesky', 'qr_decomp', 'svd_decomp',\
                          'syev', 'stev']
        
        btlbase.BTLBase._parse_args(self, passargs)
    
    @staticmethod
    def _testClass():
        return LapackTest
    
    
    
class LapackTest(btlbase.BTLTest):
    @staticmethod
    def _btl_source():
        return "libs/LAPACK/main.cpp"
    
    @staticmethod
    def _btl_includes():
        return ["libs/BLAS", "libs/LAPACK"]
    
    @staticmethod
    def _btl_defines():
        return ["LAPACKNAME=lapack"]
    