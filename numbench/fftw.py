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

class Module(btlbase.BTLBase):
    def _initialize(self):
        self.libname = "fftw"
        self.avail = (
          "FFTW_1D_Forward_Measure", "FFTW_1D_Forward_Estimate",
          "FFTW_1D_Backward_Measure", "FFTW_1D_Backward_Estimate",
          
          "FFTW_2D_Forward_Measure", "FFTW_2D_Forward_Estimate",
          "FFTW_2D_Backward_Measure", "FFTW_2D_Backward_Estimate",
          
          "FFTW_3D_Forward_Measure", "FFTW_3D_Forward_Estimate",
          "FFTW_3D_Backward_Measure", "FFTW_3D_Backward_Estimate"
        )
    
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
            self.tests = self.avail
        
        btlbase.BTLBase._parse_args(self, passargs)
        
    @staticmethod
    def get_impls(root):
        return ['fftw', 'fftw_threads']
        
    def instructionsFor(self, impl):
        Print("Use command 'pkg-config --cflags --libs " + impl + \
              "' when compiling")
    
    @staticmethod
    def _testClass():
        return FFTWTest
    
    
    
class FFTWTest(btlbase.BTLTest):
        
    def _get_flags(self):
        if self.impl == 'fftw':
            return ['-lfftw3', '-lm', '-g3']
        elif self.impl == 'fftw_threads':
            return ['-lfftw3_threads', '-lm']
        
    @staticmethod
    def _btl_source():
        return "libs/FFTW/main.cpp"
    
    @staticmethod
    def _btl_includes():
        return ["libs/FFTW"]
    
    @staticmethod
    def _btl_defines():
        return []