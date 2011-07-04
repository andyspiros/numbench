from os.path import join as pjoin
import subprocess as sp
import shlex, os
from htmlreport import HTMLreport
import basemodule

try:
    import matplotlib.pyplot as plt
    import numpy as np
    with_images = True
except ImportError:
    sys.stderr.write('Error: matplotlib and numpy are needed' + \
      'in order to generate the reports!\n')
    sys.stderr.write('Continue anyway.\n\n')    
    with_images = False

run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]

class Module(basemodule.BaseModule):
    
    def _initialize(self):
        self.libname = 'blas'
        self.avail=['axpy', 'matrix_vector', 'trisolve_vector', 'matrix_matrix']
    
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
        
        # If no test is specified, then do everything
        if len(self.tests) == 0:
            self.tests = self.avail
         
        # Generate list of dat (result) files, relative to the testdir
        self.files = [pjoin('accuracy_%s_%s.dat' % (op, name)) \
          for op in self.tests]
        
    def _compileTest(self, logfile, testdir, root, impl, *args, **kwargs):
        exe = pjoin(testdir, 'test')
        source = "accuracy/main_blas.cpp"
        flags = self._get_flags(root, impl, self.libdir)
        cxxflags = run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()
        cxx = 'g++'
        cmd = [cxx, '-o', exe, source] + flags + shlex.split(cxxflags)
        proc = sp.Popen(cmd, stdout=file(logfile, 'w'), stderr=sp.STDOUT)
        proc.wait()
        return proc.returncode, exe
    
    def _executeTest(self, logfile, exe, testdir):
        # TODO: control objdump and nm
        logfile = file(logfile, 'w')
        cmd = [exe] + self.tests
        proc = sp.Popen(cmd, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE,
          cwd=testdir)
        self.Print.down()
        while True:
            line = proc.stdout.readline()
            if not line:
                break
            logfile.write(line)
            if len(line.strip()) == 0:
                continue
            if line[0] != ' ':
                self.Print.up()
                self.Print(line.strip().split()[-1])
                self.Print.down()
            else:
                self.Print(line.strip())
        self.Print.up()          
        logfile.close()      
        proc.wait()
        return proc.returncode

    
    def save_results(self, results, figdir):
        basemodule.BaseModule.save_results(self, results,figdir, 'loglog')
            
            
