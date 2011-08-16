import os, btlbase
import subprocess as sp
import shlex
from os.path import join as pjoin

avail1 = ['axpy', 'axpby', 'rot']
avail2 = ['matrix_vector','atv','symv', 'ger', 'syr2',
          'trisolve_vector']
avail3 = ['matrix_matrix', 'aat', 'trisolve_matrix', 'trmm']

class BLASBase(btlbase.BTLBase):
    
    avail1 = avail1
    avail2 = avail2
    avail3 = avail3
    avail = avail1 + avail2 + avail3
        
    def _initialize(self):
        pass
    
    def _parse_args(self, args):     
        # Parse arguments
        tests = []
        for i in args:
            if i == '1':
                tests += self.avail1
                continue
            if i == '2':
                tests += self.avail2
                continue
            if i == '3':
                tests += self.avail3
                continue
            if i in self.avail:
                tests.append(i)
                continue
            raise Exception("Argument not recognized: " + i)
        
        # Sort tests
        self.tests = [i for i in self.avail if i in tests]
        
        # If no test is specified, then choose four standard tests
        if len(self.tests) == 0:
            self.tests = ['axpy', 'matrix_vector', \
              'trisolve_vector', 'matrix_matrix']
        
        btlbase.BTLBase._parse_args(self, args)
        
    @staticmethod
    def _testClass():
        return BLASTest


class BLASTest(btlbase.BTLTest):
    @staticmethod
    def _btl_source():
        return "libs/BLAS/main.cpp"
    
    @staticmethod
    def _btl_includes():
        return ["libs/BLAS"]
    
    def _btl_defines(self):
        return ["CBLASNAME="+self.libname, self.libname.upper()+"_INTERFACE"]