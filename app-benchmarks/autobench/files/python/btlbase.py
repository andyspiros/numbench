import sys, os, shlex
import subprocess as sp
from os.path import join as pjoin

from benchutils import *
from benchprint import Print
from htmlreport import HTMLreport
import basemodule
import benchconfig as cfg


class BTLBase(basemodule.BaseModule):
    
    def _parse_args(self, args):
        # Generate list of dat (result) files, relative to the testdir
        self.files = [pjoin('bench_%s_%s.dat' % (op, self.libname)) \
          for op in self.tests]
    
    def save_results(self, results):
        basemodule.BaseModule.save_results(self, results, 'semilogx')     
        
    
class BTLTest(basemodule.BaseTest):
    
    compileenv = {}
    runenv = {}
    
    def _compileTest(self):
        self.compileenv = {}
        
        # Includes
        includes = [pjoin(cfg.btldir, i) for i in \
        ('actions', 'generic_bench', 'generic_bench/utils', 'libs/STL') + \
        tuple(self._btl_includes())] + [pjoin(self.root, 'usr/include')]
        
        # Libraries
        libraries = ['rt']
        
        # Libdirs
        libdirs = [self.libdir]
        
        # Defines
        defines = ['NDEBUG'] + self._btl_defines()
        defines = self._btl_defines()
        
        # Flags
        flags = []
        
        ## Interpret flags        
        for flag in self._get_flags() + \
          shlex.split(run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()):
            flag = flag.strip()
            if flag[:2] == '-l':
                libraries.append(flag[2:])
            elif flag[:2] == '-L':
                libdirs.append(flag[2:])
            elif flag[:2] == '-I':
                includes.append(flag[2:])
            else:
                flags.append(flag)
        
        # Set compile environment
        self.compileenv['INCLUDE_PATH'] = ':'.join(includes)
        self.compileenv['LIBRARY_PATH'] = ':'.join(libdirs)
        self.compileenv['LD_LIBRARY_PATH'] = ':'.join(libdirs)
        self.runenv['LD_LIBRARY_PATH'] = ':'.join(libdirs)
        
        exe = pjoin(self.testdir, "test")
    
        # Retrieve compiler
        cxx = 'g++'
        cxx_portage = run_cmd(['portageq', 'envvar', 'CXX']).strip()
        if cxx_portage != '':
            cxx = cxx_portage
        if os.environ.has_key('CXX'):
            cxx = os.environ['CXX']
        
        # Form command line arguments
        args = [cxx, pjoin(cfg.btldir, self._btl_source()), '-o', exe]
        args += ['-I'+I for I in includes]
        args += ['-l'+l for l in libraries]
        args += ['-L'+L for L in libdirs]
        args += ['-D'+D for D in defines]
        args += flags
    
        # Open logfile or redirect to PIPE
        logfile = file(pjoin(self.logdir, "compile.log"), 'w')
        logfile.write(' '.join([n+'='+v for n,v in self.compileenv.items()]))
        logfile.write(' ' + ' '.join(args) + '\n' + 80*'-' + '\n')
        logfile.flush()
        
        # Execute
        proc=sp.Popen(args,stdout=logfile,stderr=sp.STDOUT,env=self.compileenv)
        proc.wait()
        
        # Close, return
        logfile.close()
        return proc.returncode, exe, logfile.name
    
    def _executeTest(self, exe):
        # Log dynamic link
        lddlogfile = file(pjoin(self.logdir, 'ldd.log'), 'w')
        sp.Popen(['ldd', '-v', exe], stdout=lddlogfile, env=self.runenv).wait()
        
        # Open pipe
        logfile = file(pjoin(self.logdir, 'btlrun.log'), 'w')
        args = [exe] + list(self.tests)
        logfile.write(' '.join([n+'='+v for n,v in self.runenv.items()]) + ' ')
        logfile.write(' '.join(args) + '\n')
        logfile.write(80*'-' + '\n')
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
          env=self.runenv, cwd=self.testdir)
        
        # Interpret output
        while True:
            # Each operation test begins with a line on stderr
            errline = proc.stderr.readline()
            if not errline:
                break
            logfile.write(errline)
            resfile = errline.split()[-1]
            testname = resfile[6:-5-len(self.libname)]
            Print(resfile)
            
            # 100 different sizes for each operation test
            Print.down()
            for i in xrange(100):
                outline = proc.stdout.readline()
                # If the line is void, something gone wrong
                if not outline:
                    return 1
                logfile.write(outline)
                Print(outline.strip())
            Print.up()
        logfile.close()
        proc.wait()
        return proc.returncode