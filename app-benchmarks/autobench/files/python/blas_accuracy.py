from pprint import pprint
from os.path import join as pjoin
import subprocess as sp
import shlex, os
from htmlreport import HTMLreport

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

class Module:
    def __init__(self, Print, libdir, args):
        self.Print = Print
        self.libdir = libdir
        self.summary = False
        self.summary_only = False
        
        self._initialize()
        
        passargs = []
        for i in args:
            if i == '-S':
                self.summary_only = True
                continue
            elif i == '-s':
                self.summary = True
                continue
            else:
                passargs += [i]
        
        self._parse_args(passargs)
    
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
           
    def _get_flags(self, root, impl, libdir):
        # Retrieve pkgconfig settings and map the directories to the new root
        path = pjoin(root, "etc/env.d/alternatives", \
          self.libname, impl, libdir, "pkgconfig")
        cmd = ['pkg-config', '--libs', '--cflags', self.libname]
        env = {'PKG_CONFIG_PATH':path}
        pkgconf = sp.Popen(cmd, stdout=sp.PIPE, env=env).communicate()[0]
        pkgconf = pkgconf.replace('-L/', '-L'+root+'/')
        pkgconf = pkgconf.replace('-I/', '-I'+root+'/')
        return shlex.split(pkgconf)
        
    def get_impls(self, root):
        output = sp.Popen(
          ['eselect', '--no-color', '--brief', 'blas', 'list'],
          env={'ROOT' : root}, stdout=sp.PIPE
        ).communicate()[0]
        return output.strip().split('\n')
    
    def run_test(self, root, impl, testdir, env, logdir):
        Print = self.Print
        libdir = self.libdir
        name = self.libname
        files = [pjoin(testdir, 'accuracy_%s_%s.dat' % (op, name)) \
          for op in self.tests]
        
        results = {}
        for op in self.tests:
            results[op] = pjoin(testdir, 'accuracy_%s_%s.dat' % (op, name))
            
        # Prepare the environment
        if env.has_key('LIBRARY_PATH'):
            env['LIBRARY_PATH'] = pjoin(root,libdir) + ":" + env['LIBRARY_PATH']
        else:
            env['LIBRARY_PATH'] = pjoin(root, libdir)
            
        if env.has_key('INCLUDE_PATH'):
            env['INCLUDE_PATH'] = \
              pjoin(root, "/usr/include") + ":" + env['INCLUDE_PATH']
        else:
            env['INCLUDE_PATH'] = pjoin(root, "/usr/include")
            
        if env.has_key('LD_LIBRARY_PATH'):
            env['LD_LIBRARY_PATH'] = \
              pjoin(root, libdir) + ":" + env['LD_LIBRARY_PATH']
        else:
            env['LD_LIBRARY_PATH'] = pjoin(root, libdir)
        
        # Backup the environment
        oldenv = {}
        for k in env.keys():
            oldenv[k] = \
              (os.environ.has_key(k) and (os.environ[k],) or (None,))[0]
        
        # Set the environment
        for k,v in env.items():
            os.environ[k] = v
        
        # Compile test suite
        exe = pjoin(testdir, 'test')
        source = "accuracy/main_blas.cpp"
        flags = self._get_flags(root, impl, libdir)
        cxxflags = run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()
        cxx = 'g++'
        cmd = [cxx, '-o', exe, source] + flags + shlex.split(cxxflags)
        logfile = pjoin(logdir, 'compile.log')
        p = sp.Popen(cmd, stdout=file(logfile, 'w'), stderr=sp.STDOUT)
        p.wait()
        if p.returncode != 0:
            Print("Compilation failed")
            Print("See log: " + logfile)
            return
        Print("Compilation successful")
        
        # Run test
        logfile = file(pjoin(logdir, name+"_run.log"), 'w')
        cmd = [pjoin(testdir,"test")] + self.tests
        proc = sp.Popen(cmd, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE,
          cwd=testdir)
        Print.down()
        while True:
            line = proc.stdout.readline()
            if not line:
                break
            logfile.write(line)
            if len(line.strip()) == 0:
                continue
            if line[0] != ' ':
                Print.up()
                Print(line.strip().split()[-1])
                Print.down()
            else:
                Print(line.strip())
        Print.up()          
        logfile.close()      
        proc.wait()
        if proc.returncode != 0:
            Print('Test failed')
        else:
            Print('Test successful')
        
        return results
        
    
    def save_results(self, results, figdir):
        if not with_images:
            self.Print("Report generation skipped - missing libraries")
            return
    
        # Re-order the result dictionary
        newresults = {}
        for test in self.tests:
            newresults[test] = {}
            for nameimpl in results:
                nameimplstr = pjoin(*nameimpl)
                resdat = results[nameimpl][test]
                newresults[test][nameimplstr] = resdat
        
        # Begin the HTML report
        htmlfname = pjoin(figdir, 'index.html')
        html = HTMLreport(htmlfname)
        
        # Generate summary - a single image with all plots
        if self.summary or self.summary_only:
            # Save summary figure
            sprows = (len(self.tests)+1)/2
            plt.figure(figsize=(16,6*sprows), dpi=300)
            for i, test in enumerate(self.tests, 1):
                plt.subplot(sprows, 2, i)
                plt.title(test)
                for impl in newresults[test]:
                    x,y = np.loadtxt(newresults[test][impl], unpack=True)
                    plt.loglog(x,y, label=impl, hold=True)
                plt.legend(loc='best')
                plt.grid(True)
            fname = pjoin(figdir, 'summary.png')
            plt.savefig(fname, format='png')
            html.addFig("Summary", image=os.path.basename(fname), width='95%')
            self.Print('Summary figure saved: ' + fname)
                
        # Generate plots
        if not self.summary_only:
            for test in self.tests:
                plt.figure(figsize=(12,9), dpi=300)
                for impl in newresults[test]:
                    x,y = np.loadtxt(newresults[test][impl], unpack=True)
                    plt.loglog(x,y, label=impl, hold=True)
                plt.legend(loc='best')
                plt.grid(True)
                fname = pjoin(figdir, test+".png")
                plt.savefig(fname, format='png')
                html.addFig(test, image=os.path.basename(fname))
                self.Print('Figure ' + fname + ' saved')
        
        html.close()
            
            
