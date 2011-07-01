import sys, os, shlex
import commands as cmd
import subprocess as sp
from os.path import join as pjoin
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

def btlcompile(exe, source, btldir, includes, defines, libs, libdirs, other, \
  logfile=None):
    """
    Helper function that compiles a C++ source based on btl. The function
    sets the compiler flags that are needed by btl (include directives, link
    with rt,...). More options are accepted as arguments:
    
    exe: the generated executable
    
    source: the C++ source
    
    btldir: the base directory of the btl sources
    
    includes: an iterable containing the include directories (without -I)
    
    defines: an iterable of strings with define directives (without -D). In case
    of key-value pairs, the equal sign and the value have to be in the same
    string as the key: ['NDEBUG', 'FOO=BAR'] is transormed to
    '-DNDEBUD -DFOO=BAR'
    
    libs: the libraries to link against (without -l)
    
    libdirs: the directories where the libraries are seeked (without -L)
    
    other: an iterable with compiler flags
    
    logfile: the path of the file where the log is saved. The directory must
    exist. If None, no log is generated.
    """
    
    incs = (
      "%s/actions" % btldir,
      "%s/generic_bench" % btldir,
      "%s/generic_bench/utils" % btldir,
      "%s/libs/STL" % btldir
    ) + tuple(includes)
    incs = ' '.join(['-I'+i for i in incs])
    
    defs = ' '.join(['-D'+d for d in ["NDEBUG"] + defines])
    
    libs = ' '.join(['-l'+l for l in ["rt"] + libs])
    
    libdirs = ' '.join(['-L'+L for L in libdirs])
    
    cxxflags = run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()
    
    otherflags = ' '.join(other)
    
    # TODO: use CXX instead of g++
    cl = "g++ -o %s %s %s %s %s %s %s %s" \
        % (exe, source, incs, defs, libs, libdirs, cxxflags, otherflags)
        
    if logfile is None:
        fout = sp.PIPE
    else:
        fout = file(logfile, 'w')
        fout.write(cl + "\n" + 80*'-' + "\n")
        fout.flush()
    cl = shlex.split(cl)
    cp = sp.Popen(cl, stdout=fout, stderr=sp.STDOUT)
    cp.communicate()
    if logfile is not None:
        fout.close()
    return (cp.returncode, ' '.join(cl))


class BTLBase:
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
        
          
    def run_test(self, root, impl, testdir, env, logdir):
        # Convenient renames and definition of report files 
        Print = self.Print
        libdir = self.libdir
        name = self.libname
        files = [pjoin(testdir, 'bench_%s_%s.dat' % (op, name)) \
          for op in self.tests]
        
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
            results = {}
            for op in self.tests:
                results[op] = pjoin(testdir, 'bench_%s_%s.dat'%(op,name))
            return results
        
        for i in files:
            if os.path.exists(i): os.remove(i)
            
        # Prepare the environment
        if env.has_key('LIBRARY_PATH'):
            env['LIBRARY_PATH'] = root+libdir + ":" + env['LIBRARY_PATH']
        else:
            env['LIBRARY_PATH'] = root+libdir
            
        if env.has_key('INCLUDE_PATH'):
            env['INCLUDE_PATH'] = root+"/usr/include" +":"+ env['INCLUDE_PATH']
        else:
            env['INCLUDE_PATH'] = root+"/usr/include"
            
        if env.has_key('LD_LIBRARY_PATH'):
            env['LD_LIBRARY_PATH'] = root+libdir + ":" + env['LD_LIBRARY_PATH']
        else:
            env['LD_LIBRARY_PATH'] = root+libdir
        
        # Backup the environment
        oldenv = {}
        for k in env.keys():
            oldenv[k] = \
              (os.environ.has_key(k) and (os.environ[k],) or (None,))[0]
        
        # Set the environment
        for k,v in env.items():
            os.environ[k] = v
        
        # Compile
        # TODO: use CXX instead of g++
        btldir = 'btl/'
        logfile = os.path.join(logdir, name+"_comp.log")
        returncode, compilecl = btlcompile(
          exe = testdir + "/test",
          source = btldir + self._btl_source(),
          btldir = btldir,
          includes = [btldir+d for d in self._btl_includes()],
          defines = self._btl_defines(),
          libs = [],
          libdirs = [root+libdir],
          other = self._get_flags(root, impl, libdir),
          logfile = logfile
        )
        if returncode != 0:
            Print("Compilation failed")
            Print("See log: " + logfile)
            return
        Print("Compilation successful")
        
        # Run test
        logfile = file(os.path.join(logdir, name+"_run.log"), 'w')
        args = [os.path.join(testdir,"test")] + self.tests
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
          cwd = testdir)
        results = {}
        while True:
            errline = proc.stderr.readline()
            logfile.write(errline)
            if not errline:
                break
            resfile = errline.split()[-1]
            testname = resfile[6:-5-len(name)]
            results[testname] = pjoin(testdir, resfile)
            Print(resfile)
            Print.down()
            for i in xrange(100):
                outline = proc.stdout.readline()
                logfile.write(outline)
                Print(outline.rstrip())
            Print.up()
        Print.up()
        proc.wait()
        logfile.close()
        if proc.returncode != 0:
            Print('Test failed')
        else:
            Print('Test successful')
        
        # Restore the old environment
        for k in env.keys():
            if oldenv[k] != None:
                os.environ[k] = oldenv[k]
            elif os.environ.has_key(k):
                del os.environ[k]
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
                nameimplstr = "%s/%s" % nameimpl
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
                    plt.semilogx(x,y, label=impl, hold=True)
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
                    plt.semilogx(x,y, label=impl, hold=True)
                plt.legend(loc='best')
                plt.grid(True)
                fname = pjoin(figdir, test+".png")
                plt.savefig(fname, format='png')
                html.addFig(test, image=os.path.basename(fname))
                self.Print('Figure ' + fname + ' saved')
        
        html.close()
            
            
