import sys, os, shlex
import subprocess as sp
from os.path import join as pjoin

from benchutils import mkdir, run_cmd
from benchprint import Print
from htmlreport import HTMLreport
import basemodule
import benchconfig as cfg
import benchchilds
from testdescr import testdescr

class BTLBase(basemodule.BaseModule):
    
    @classmethod
    def printHelp(cls):
        basemodule.BaseModule.printHelp()
        
        print "Tests:"
        for i in cls.avail:
            print "   " + i + ":"
            print "     " + testdescr[i]
            print
    
    def _parse_args(self, args):
        # Generate list of dat (result) files, relative to the testdir
        self.files = [pjoin('bench_%s_%s.dat' % (op, self.libname)) \
          for op in self.tests]
        
        for i in args:
            if i[:2] == '-N':
                BTLTest.N = i[2:]
                continue
            raise Exception("Argument not recognized: " + i)
    
    def save_results(self, results):
        basemodule.BaseModule.save_results(self, results, 'semilogx')     
        
    
class BTLTest(basemodule.BaseTest):
    
    N = None
    
    def _compileTest(self):
        self.compileenv = {}
        self.runenv = {}
        
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
        # PATH
        envpath = ':'.join([pjoin(self.root, l) for l in ('bin', 'usr/bin')])
        if (os.environ.has_key('PATH')):
            envpath += ':' + os.environ['PATH']
        self.compileenv['PATH'] = envpath
        self.runenv['PATH'] = envpath
        
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
    
    def _executeTest(self, exe, logfile, preargs=[]):
        # Log dynamic link
        lddlogfile = file(pjoin(self.logdir, 'ldd.log'), 'w')
        sp.Popen(['ldd', '-v', exe], stdout=lddlogfile, env=self.runenv).wait()
        
        # Open pipe
        logfile = file(logfile, 'w')
        args = preargs + [exe] + list(self.tests)
        if self.N is not None:
            args.append('-N' + self.N)
        logfile.write(' '.join( \
          [n + '="'+v+'"' for n,v in self.runenv.items()]  ) + ' ')
        logfile.write(' '.join(args) + '\n')
        logfile.write(80*'-' + '\n')
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
          env=self.runenv, cwd=self.testdir)
        benchchilds.append(proc)
        
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
            
            # Many different sizes for each operation test
            Print.down()
            cur = 0
            tot = 1
            while cur != tot:
                outline = proc.stdout.readline()
                # If the line is void, something gone wrong
                if not outline:
                    Print.up()
                    Print('Execution error')
                    return 1
                logfile.write(outline)
                logfile.flush()
                
                # Interpret line
                outline = outline.strip()
                (cur, tot) = shlex.split(outline)[-1][1:-1].split('/')
                cur = int(cur); tot = int(tot)
                Print(outline)
                
                
            Print.up()
        logfile.close()
        proc.wait()
        Print("Execution finished with return code " + str(proc.returncode))
        return proc.returncode
