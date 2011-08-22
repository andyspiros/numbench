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