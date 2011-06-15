import os, blasbase
import subprocess as sp
import shlex

class Module(blasbase.BTLBase):
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
    
    @staticmethod
    def _btl_source():
        return "/libs/LAPACK/main.cpp"
    
    @staticmethod
    def _btl_includes():
        return ["/libs/BLAS", "libs/LAPACK"]
    
    def _btl_defines(self):
        return ["LAPACKNAME=" + self.libname]
           
    def _get_flags(self, root, impl, libdir):
        # Retrieve pkgconfig settings and map the directories to the new root
        path = "%s/etc/env.d/alternatives/%s/%s/%s/pkgconfig" % \
          (root, self.libname, impl, libdir)
        pkgconf = sp.Popen('pkg-config --libs --cflags lapack', shell=True, \
          stdout=sp.PIPE, env={'PKG_CONFIG_PATH':path}).communicate()[0]
        pkgconf = pkgconf.replace('-L/', '-L'+root+'/')
        pkgconf = pkgconf.replace('-I/', '-I'+root+'/')
        return shlex.split(pkgconf)
        
        
    def get_impls(self, root):
        output = sp.Popen(
          ['eselect', '--no-color', '--brief', self.libname, 'list'],
          env={'ROOT' : root}, stdout=sp.PIPE
        ).communicate()[0]
        return output.strip().split('\n')

del blasbase
