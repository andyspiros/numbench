from os.path import join as pjoin, basename, dirname
import subprocess as sp
import shlex, os
import shutil as shu

import benchconfig as cfg
from htmlreport import HTMLreport
import basemodule
from benchutils import mkdir, run_cmd
from benchprint import Print
import benchpkgconfig as pc
from testdescr import testdescr

try:
    if not locals().has_key('initialized'):
        initialized = True
	import matplotlib
	matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        import numpy as np
        with_images = True
except ImportError:
    sys.stderr.write('Error: matplotlib and numpy are needed' + \
      'in order to generate the reports!\n')
    sys.stderr.write('Continue anyway.\n\n')    
    with_images = False


class BaseModule:
    def __init__(self, args):
        self.summary = False
        self.summary_only = False
        
        self._initialize()
        
        passargs = []
        for i in args:
            if i in ('-S', '--summary-only'):
                self.summary_only = True
                continue
            elif i in ('-s', '--summary'):
                self.summary = True
                continue
            else:
                passargs += [i]
        
        self._parse_args(passargs)
    
    @classmethod
    def printHelp(cls):
        print "Usage: numbench " + cfg.modulename + " [options] [tests]"
        print
        print "Generic options:"
        print "   -s -summary"
        print "      Generate a summary image that contains all tests results"
        print "      and show it on the HTML report page."
        print
        print "   -S -summary-only"
        print "      Only generate the summary image and skip all other plots."
        print
        
    # Alternatives-2 version
    def get_impls(self, root):
        output = sp.Popen(
          ['eselect', '--no-color', '--brief', self.libname, 'list'],
          env={'ROOT' : root}, stdout=sp.PIPE).communicate()[0]
        output = output.strip()
        if '(none found)' in output:
            return []
        else:
            return [i.split()[0] for i in output.split('\n')] 
           
    def getTest(self, root, impl, testdir, logdir):
        TestClass = self._testClass()
        t = TestClass(root, impl, testdir, logdir)
        t.libname = self.libname
        t.tests = self.tests
        t.files = self.files
        return t
    
    def save_results(self, results, plottype='plot', ylabel="MFlops"):
        if not with_images:
            Print("Report generation skipped - missing libraries")
            return
        
        try:
            plt.figure()
        except:
            Print("Unable to generate plots")
            Print("Please make sure that X is running and $DISPLAY is set")
            return
        
        # Copy inputfile and logs
        shu.copytree(cfg.logdir, pjoin(cfg.reportdir, 'log'))
        baseinfile = basename(cfg.inputfile)
        shu.copy2(cfg.inputfile, pjoin(cfg.reportdir, baseinfile))
        
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
                if results[nameimpl] == None:
                    continue
                resdat = results[nameimpl][test]
                newresults[test][nameimplstr] = resdat
        
        # Begin the HTML report
        htmlfname = pjoin(cfg.reportdir, 'index.html')
        html = HTMLreport(htmlfname)
        
        # Generate summary - a single image with all plots
        if self.summary or self.summary_only:
            # Save summary figure
            sprows = (len(self.tests)+1)/2
            plt.figure(figsize=(16,6*sprows), dpi=300)
            plt.subplots_adjust(wspace=.4, hspace=.4)
            for i, test in enumerate(self.tests, 1):
                plt.subplot(sprows, 2, i)
                plt.title(testdescr[test])
                for impl in newresults[test]:
                    x,y = np.loadtxt(newresults[test][impl], unpack=True)
                    plotf(x,y, label=impl, hold=True)
                if self.summary_only:
                    plt.legend(loc='best')
                plt.xlabel('size')
                plt.ylabel(ylabel)
                plt.grid(True)
            fname = pjoin(cfg.reportdir, 'summary.png')
            plt.savefig(fname, format='png', bbox_inches='tight', \
              transparent=True)
            html.addFig("Summary", image=os.path.basename(fname), width='90%')
            Print('Summary figure saved: ' + fname)
                
        # Generate plots
        if not self.summary_only:
            for test in self.tests:
                plt.figure(figsize=(12,9), dpi=300)
                for impl in newresults[test]:
                    x,y = np.loadtxt(newresults[test][impl], unpack=True)
                    plotf(x,y, label=impl, hold=True)
                plt.legend(loc='best')
                plt.xlabel('size')
                plt.ylabel(ylabel)
                plt.grid(True)
                fname = pjoin(cfg.reportdir, test+".png")
                plt.savefig(fname, format='png', bbox_inches='tight', \
                  transparent=True)
                html.addFig(testdescr[test], image=os.path.basename(fname))
                Print('Figure ' + fname + ' saved')
        
        html.close()
        Print('HTML report generated: ' + htmlfname)

class CompilationError(Exception):
    def __init__(self, logfile):
        self.logfile = logfile


class BaseTest:
    libname = None
    tests = None
    files = None
    
    def __init__(self, root, impl, testdir, logdir):
        self.root = root
        self.impl = impl
        self.testdir = testdir
        self.logdir = pjoin(logdir, impl)
        self.compileenv = {}
        self.runenv = {}
        
        mkdir(self.logdir)
        
        self.libdir = cfg.libdir
        while self.libdir[0] == '/':
            self.libdir = self.libdir[1:]
        self.libdir = pjoin(self.root, self.libdir)
        
    # Base version
    def _generateResults(self):
        return dict(zip(self.tests, self.files))
           
    # Alternatives-2 version with pkg-config
    def _get_flags(self):
        # 1. Run with no requires
        pfile = pc.GetFile(self.libname, self.impl, self.root)
        flags = pc.Run(pfile, self.root, False)
        logfile = file(pjoin(self.logdir, 'pkg-config.log'), 'w')
        print >> logfile, "File:", pfile
        print >> logfile, "Result:", flags
        
        # 2. Get requires
        requires = pc.Requires(pfile)
        print >> logfile, "Requires:", requires
        
        # 3.Substitute requires and add flags
        for r in requires:
            if r in self.changes.keys():
                pfile = pc.GetFile(r, self.changes[r])
                flags += ' ' + pc.Run(pfile)
            else:
                flags += ' ' + pc.Run(r)
        print >> logfile, "Third run:", flags
        logfile.close()
        
        return shlex.split(flags)
    
    def run_test(self, changes={}):
        self.changes = changes
        
        # Convenient renames and definition of report files
        name = self.libname
        root = self.root
        testdir = self.testdir
        self.files = [pjoin(testdir,f) for f in self.files]
        env = {} # TODO: remove this
        if cfg.libdir[0] == '/':
            libdir = root+cfg.libdir
        else:
            libdir = pjoin(root, cfg.libdir)
        
        # Create dir. If all results already exist use them and do not perform
        # the tests, otherwise remove every old results.
        runtests = False
        if os.path.exists(testdir):
            runtests = not all([os.path.exists(i) for i in self.files])
        else:
            os.makedirs(testdir)
            runtests = True
        if not runtests:
            Print("Not testing: results exist")
            return self._generateResults()
        for i in self.files:
            if os.path.exists(i): os.remove(i)
        
        # Compile test suite
        try:
            returncode, exe, logfile = self._compileTest()
            if returncode != 0:
                raise CompilationError(logfile)
        except CompilationError as e:
            Print("Compilation failed")
            Print("See log: " + e.logfile)
            return
        Print("Compilation successful")
        
        # Run test
        logfile = pjoin(self.logdir, name+"_run.log")
        retcode = self._executeTest(exe)
        if retcode != 0:
            Print("Test failed")
            Print("See log: " + logfile)
            return
        Print("Test successful")
                
        # Return
        return self._generateResults()
    
