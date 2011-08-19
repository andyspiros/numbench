import os, btlbase, shlex, subprocess as sp

numproc = 4

class Module(btlbase.BTLBase):
    def _initialize(self):
        self.libname = "scalapack"
        self.avail = ['axpy', 'matrix_vector', 'lu_decomp', 'cholesky',
          'qr_decomp', 'svd_decomp', 'symm_ev']
    
    def _parse_args(self, args):
        passargs = []
        
        # Parse arguments
        tests = []
        skip = 0
        
        for i, a in enumerate(args):
            if skip != 0:
                skip -= 1
                continue
            if a == '-n':
                skip += 1
                numproc = args[i+1]
                continue
            if a in self.avail:
                tests.append(a)
                continue
            passargs.append(a)
        
        # Sort tests
        self.tests = [i for i in self.avail if i in tests]
        
        # If no test is specified, then choose four standard tests
        if len(self.tests) == 0:
            self.tests = self.avail
        
        btlbase.BTLBase._parse_args(self, passargs)
        
    @staticmethod
    def _testClass():
        return ScaLAPACKTest


class ScaLAPACKTest(btlbase.BTLTest):    
#    def __init__(self, *args, **kwargs):
#        os.environ['CXX'] = 'mpic++'
#        btlbase.BTLTest.__init__(self, *args, **kwargs)
        
    def _get_flags(self):
        proc = sp.Popen(['mpic++', '-showme'], stdout=sp.PIPE)
        out = proc.communicate()[0]
        if proc.returncode != 0:
            raise Exception("An MPI implementation is needed in order to" \
              + " run the scalapack tests.")
        return shlex.split(out)[1:] + btlbase.BTLTest._get_flags(self)
    
    def _executeTest(self, exe, logfile):
        btlbase.BTLTest._executeTest(self, exe, logfile, \
          ['mpirun', '-n', str(numproc)])
    
    @staticmethod
    def _btl_source():
        return "libs/PBLAS/main.cpp"
    
    @staticmethod
    def _btl_includes():
        return ["libs/"+i for i in ("BLAS", "LAPACK", "BLACS", "PBLAS", "STL")]
    
    def _btl_defines(self):
        return ["PBLASNAME="+self.libname]