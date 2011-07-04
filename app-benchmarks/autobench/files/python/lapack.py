import os, btlbase
import subprocess as sp
import shlex

class Module(btlbase.BTLBase):
    def _initialize(self):
        self.libname = "lapack"
        self.avail = ['general_solve', 'least_squares', 'lu_decomp', \
          'cholesky', 'symm_ev']
    
    def _parse_args(self, args):     
        # Parse arguments
        tests = []
        for i in args:
            if i in self.avail:
                tests.append(i)
                continue
            raise Exception("Argument not recognized: " + i)
        
        # Sort tests
        self.tests = [i for i in self.avail if i in tests]
        
        # If no test is specified, run everything
        if len(self.tests) == 0:
            self.tests = self.avail
        
        btlbase.BTLBase._parse_args(self, args)
    
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
    