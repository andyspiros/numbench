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

class BaseModule:
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
           
    # Alternatives-2 version with pkg-config
    def _get_flags(self, root, impl, libdir):
        while libdir[0] == '/':
            libdir = libdir[1:]
        # Retrieve pkgconfig settings and map the directories to the new root
        path = pjoin(root, "etc/env.d/alternatives", \
          self.libname,impl,libdir, "pkgconfig")
        cmd = ['pkg-config', '--libs', '--cflags', self.libname]
        env = {'PKG_CONFIG_PATH':path}
        pkgconf = sp.Popen(cmd, stdout=sp.PIPE, env=env).communicate()[0]
        pkgconf = pkgconf.replace('-L/', '-L'+root+'/')
        pkgconf = pkgconf.replace('-I/', '-I'+root+'/')
        return shlex.split(pkgconf)
        
    # Alternatives-2 version
    def get_impls(self, root):
        output = sp.Popen(
          ['eselect', '--no-color', '--brief', self.libname, 'list'],
          env={'ROOT' : root}, stdout=sp.PIPE).communicate()[0]
        return output.strip().split('\n')
        
    # Base version
    def _generateResults(self, files):
        return dict(zip(self.tests, files))
    
    def run_test(self, root, impl, testdir, env, logdir):
        # Convenient renames and definition of report files
        Print = self.Print
        name = self.libname
        files = [pjoin(testdir,f) for f in self.files]
        if self.libdir[0] == '/':
            libdir = root+self.libdir
        else:
            libdir = pjoin(root, self.libdir)
        
        # Create dir. If all results already exist use them and do not perform
        # the tests, otherwise remove every old results.
        runtests = False
        if os.path.exists(testdir):
            runtests = not all([os.path.exists(i) for i in files])
        else:
            os.makedirs(testdir)
            runtests = True
        
        if not runtests:
            Print("Not testing: results exist")
            return self._generateResults(files)
        
        for i in files:
            if os.path.exists(i): os.remove(i)
            
        # Prepare the environment
        if env.has_key('LIBRARY_PATH'):
            env['LIBRARY_PATH'] = libdir + ":" + env['LIBRARY_PATH']
        else:
            env['LIBRARY_PATH'] = libdir
            
        if env.has_key('INCLUDE_PATH'):
            env['INCLUDE_PATH'] = \
              pjoin(root, "usr/include") + ":" + env['INCLUDE_PATH']
        else:
            env['INCLUDE_PATH'] = pjoin(root, "usr/include")
            
        if env.has_key('LD_LIBRARY_PATH'):
            env['LD_LIBRARY_PATH'] = \
              libdir + ":" + env['LD_LIBRARY_PATH']
        else:
            env['LD_LIBRARY_PATH'] = libdir
        
        # Backup the environment
        oldenv = {}
        for k in env.keys():
            oldenv[k] = \
              (os.environ.has_key(k) and (os.environ[k],) or (None,))[0]
        
        # Set the new environment
        for k,v in env.items():
            os.environ[k] = v
        
        # Compile test suite
        logfile = os.path.join(logdir, name+"_comp.log")
        returncode, exe = self._compileTest(logfile=logfile, testdir=testdir, \
          root=root, impl=impl, libdir=libdir)
        if returncode != 0:
            Print("Compilation failed")
            Print("See log: " + logfile)
            return
        Print("Compilation successful")
        
        # Run test
        logfile = pjoin(logdir, name+"_run.log")
        retcode = self._executeTest(logfile=logfile, exe=exe, testdir=testdir)
        if returncode != 0:
            Print("Test failed")
            Print("See log: " + logfile)
            return
        Print("Test successful")
        
        # Restore the old environment
        for k in env.keys():
            if oldenv[k] != None:
                os.environ[k] = oldenv[k]
            elif os.environ.has_key(k):
                del os.environ[k]
                
        # Return
        return self._generateResults(files)
    
    
    def save_results(self, results, figdir, plottype='plot'):
        if not with_images:
            self.Print("Report generation skipped - missing libraries")
            return
        
        if plottype == 'plot': plotf = plt.plot
        elif plottype == 'semilogx': plotf = plt.semilogx
        elif plottype == 'semilogy': plotf = plt.semilogy
        elif plottype == 'loglog': plotf = plt.loglog
        else:
            raise Exception('Unrecognized plot type: "' + plottype + '"')
    
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
                    plotf(x,y, label=impl, hold=True)
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
                    plotf(x,y, label=impl, hold=True)
                plt.legend(loc='best')
                plt.grid(True)
                fname = pjoin(figdir, test+".png")
                plt.savefig(fname, format='png')
                html.addFig(test, image=os.path.basename(fname))
                self.Print('Figure ' + fname + ' saved')
        
        html.close()
