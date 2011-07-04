import sys, os, shlex
import commands as cmd
import subprocess as sp
from os.path import join as pjoin
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
    
    # Compile flags
    incs = (
      "%s/actions" % btldir,
      "%s/generic_bench" % btldir,
      "%s/generic_bench/utils" % btldir,
      "%s/libs/STL" % btldir
    ) + tuple(includes)
    incs = ['-I'+i for i in incs]
    
    defs = ['-D'+d for d in ["NDEBUG"] + defines]
    
    libs = ['-l'+l for l in ["rt"] + libs]
    
    libdirs = ['-L'+L for L in libdirs]
    
    cxxflags = shlex.split(run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip())
    
    otherfl = other
    
    # Retrieve compiler
    cxx = 'g++'
    cxx_portage = run_cmd(['portageq', 'envvar', 'CXX']).strip()
    if cxx_portage != '':
        cxx = cxx_portage
    if os.environ.has_key('CXX'):
        cxx = os.environ['CXX']
    
    # Compile command
    cl = [cxx, '-o', exe, source]+incs+defs+libs+libdirs+cxxflags+other
    
    # Open logfile or redirect to PIPE 
    if logfile is None:
        fout = sp.PIPE
    else:
        fout = file(logfile, 'w')
        fout.write(str(cl) + "\n" + 80*'-' + "\n")
        fout.flush()
    
    # Execute command
    cp = sp.Popen(cl, stdout=fout, stderr=sp.STDOUT)
    cp.wait()
    
    # Close the log file (if any)
    if logfile is not None:
        fout.close()
    
    return cp.returncode


class BTLBase(basemodule.BaseModule):
    
    def _parse_args(self, args):
        # Generate list of dat (result) files, relative to the testdir
        self.files = [pjoin('bench_%s_%s.dat' % (op, self.libname)) \
          for op in self.tests]
    
    def _compileTest(self, logfile, testdir, root, impl, libdir, \
      *args, **kwargs):
        btldir = 'btl/'
        exe = pjoin(testdir, "test")
        returncode = btlcompile(
          exe = exe,
          source = pjoin(btldir, self._btl_source()),
          btldir = btldir,
          includes = [pjoin(btldir, d) for d in self._btl_includes()],
          defines = self._btl_defines(),
          libs = [],
          libdirs = [libdir],
          other = self._get_flags(root, impl, self.libdir),
          logfile = logfile
        )
        return returncode, exe
    
    def _executeTest(self, logfile, exe, testdir):
        # TODO: control objdump and nm
        logfile = file(logfile, 'w')
        args = [exe] + self.tests
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
          cwd = testdir)
        while True:
            # Each operation test begins with a line on stderr
            errline = proc.stderr.readline()
            if not errline:
                break
            logfile.write(errline)
            resfile = errline.split()[-1]
            testname = resfile[6:-5-len(self.libname)]
            self.Print(resfile)
            
            # 100 different sizes for each operation test
            self.Print.down()
            for i in xrange(100):
                outline = proc.stdout.readline()
                logfile.write(outline)
                self.Print(outline.strip())
            self.Print.up()
        logfile.close()
        proc.wait()
        return proc.returncode
    
    def save_results(self, results, figdir):
        basemodule.BaseModule.save_results(self, results,figdir, 'semilogx')
