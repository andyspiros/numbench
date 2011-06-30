import os, btlbase
import subprocess as sp
import shlex

class Module(btlbase.BTLBase):
    def _initialize(self):
        self.libname = "cblas"
        self.avail1 = ['axpy', 'axpby', 'rot']
        self.avail2 = ['matrix_vector','atv','symv','syr2','ger',
          'trisolve_vector']
        self.avail3 = ['matrix_matrix', 'aat', 'trisolve_matrix', 'trmm']
        self.avail = self.avail1 + self.avail2 + self.avail3
    
    def _parse_args(self, args):     
        # Parse arguments
        tests = []
        for i in args:
            if i == '1':
                tests += avail1
                continue
            if i == '2':
                tests += avail2
                continue
            if i == '3':
                tests += avail3
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
    
    @staticmethod
    def _btl_source():
        return "/libs/BLAS/main.cpp"
    
    @staticmethod
    def _btl_includes():
        return ["/libs/BLAS"]
    
    def _btl_defines(self):
        return ["CBLASNAME=" + self.libname, "CBLAS_INTERFACE"]
           
    def _get_flags(self, root, impl, libdir):
        # Retrieve pkgconfig settings and map the directories to the new root
        path = "%s/etc/env.d/alternatives/%s/%s/%s/pkgconfig" % \
          (root, self.libname, impl, libdir)
        pkgconf = sp.Popen('pkg-config --libs --cflags cblas', shell=True, \
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

del btlbase
