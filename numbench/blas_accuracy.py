#=====================================================
# Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
#=====================================================
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
import subprocess as sp
import shlex, os
from os.path import join as pjoin

from benchutils import mkdir, run_cmd
from benchprint import Print
from htmlreport import HTMLreport
import basemodule
import benchconfig as cfg
import benchchildren

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
        self.files = [pjoin('accuracy_%s_%s.dat' % (op, self.libname)) \
          for op in self.tests]
        
    @staticmethod
    def _testClass():
        return BLAS_accuracyTest

    
    def save_results(self, results):
        basemodule.BaseModule.save_results(self, results, 'loglog')
            
class BLAS_accuracyTest(basemodule.BaseTest):
    
    compileenv = {}
    runenv = {}
        
    def _compileTest(self):
        self.compileenv = {}
        
        # Flags and envvars lists
        includes = [pjoin(self.root, 'usr/include')]
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
        source = pjoin(cfg.btldir, "accuracy/main_blas.cpp")
    
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
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
          env=self.runenv, cwd=self.testdir)
        benchchildren.append(proc)
        
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
                Print(line.strip().split()[-1])
                Print.down()
            else:
                Print(line.strip())
        Print.up()          
        logfile.close()      
        proc.wait()
        return proc.returncode
