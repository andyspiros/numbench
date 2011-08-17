import subprocess as sp
import shlex, os
from os.path import join as pjoin

from benchutils import mkdir, run_cmd
from benchprint import Print
from htmlreport import HTMLreport
import basemodule
import benchconfig as cfg
import benchchilds

class Module(basemodule.BaseModule):
    
    def _initialize(self):
        self.libname = 'lapack'
        self.avail=['lu_decomp', 'cholesky', 'svd_decomp', 'qr_decomp', \
                    'syev', 'stev']
    
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
        self.files = [pjoin('accuracy_%s_%s.dat' % (op, self.libname)) \
          for op in self.tests]
        
    @staticmethod
    def _testClass():
        return LAPACK_accuracyTest

    
    def save_results(self, results):
        basemodule.BaseModule.save_results(self, results, 'plot', \
          'Relative error')
            
class LAPACK_accuracyTest(basemodule.BaseTest):
    
    compileenv = {}
    runenv = {}
        
    def _compileTest(self):
        self.compileenv = {}
        
        # Flags and envvars lists
        includes = [pjoin(self.root, 'usr/include'),
                    pjoin(cfg.btldir, 'accuracy'),
                    pjoin(cfg.btldir, 'libs', 'LAPACK'),
                    pjoin(cfg.btldir, 'generic_bench', 'utils')]
        libraries = []
        libdirs = [self.libdir]
        defines = ['NDEBUG']
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
        source = pjoin(cfg.btldir, "accuracy/lapack/main_lapack.cpp")
    
        # Retrieve compiler
        cxx = 'g++'
        cxx_portage = run_cmd(['portageq', 'envvar', 'CXX']).strip()
        if cxx_portage != '':
            cxx = cxx_portage
        if os.environ.has_key('CXX'):
            cxx = os.environ['CXX']
        
        # Form command line arguments
        args = [cxx, source, '-o', exe]
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
    
    
    def _executeTest(self, exe, logfile):
        # Log dynamic link
        lddlogfile = file(pjoin(self.logdir, 'ldd.log'), 'w')
        sp.Popen(['ldd', '-v', exe], stdout=lddlogfile, env=self.runenv).wait()
        
        # Open pipe
        logfile = file(logfile, 'w')
        args = [exe] + self.tests
        logfile.write(' '.join([n+'='+v for n,v in self.runenv.items()]) + ' ')
        logfile.write(' '.join(args) + '\n')
        logfile.write(80*'-' + '\n')
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=logfile, 
          env=self.runenv, cwd=self.testdir)
        benchchilds.append(proc)
        
        # Interpret output
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
                Print(line.strip())
                Print.down()
            else:
                Print(line[1:-1])
        Print.up()
        logfile.close()
        proc.wait()
        return proc.returncode
