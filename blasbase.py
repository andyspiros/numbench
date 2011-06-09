import os, shlex
import commands as cmd
import subprocess as sp

run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]

class ModuleBase:
    def __init__(self, Print, libdir, args):
        self.Print = Print
        self.libdir = libdir
        
        avail1 = ['axpy', 'axpby', 'rot']
        avail2 = ['matrix_vector','atv','symv','syr2','ger','trisolve_vector']
        avail3 = ['matrix_matrix', 'aat', 'trisolve_matrix', 'trmm']
        
        tests = []
        for i in args:
            if i == '1':
                tests += avail1
                continue
            if i == '2':
                tests += avail2
                continue
            if i == '3':
                tests += avail3
                continue
            if i in avail1 + avail2 + avail3:
                tests.append(i)
                continue
            raise Exception("Argument not recognized: " + i)
        self.tests = [i for i in avail1+avail2+avail3 if i in tests]
        
        if len(self.tests) == 0:
            self.tests = ['axpy', 'matrix_vector', \
              'trisolve_vector', 'matrix_matrix']
        
          
    def run_test(self, root, impl, testdir):
        Print = self.Print
        libdir = self.libdir
        name = 'blas'
        files = ['%s/bench_%s_%s.dat' %(testdir, op, name) for op in self.tests]
        
        # Create dir. If all results already exist use them, otherwise
        # remove old results
        runtests = False
        if os.path.exists(testdir):
            runtests = not all([os.path.exists(i) for i in files])
        else:
            os.makedirs(testdir)
            runtests = True
        
        if not runtests:
            Print("Not testing: results exist")
            return files
        
        for i in files:
            if os.path.exists(i): os.remove(i)
        
        # Setup environment for testing
        oldenv = {}
        for v in ('LIBRARY_PATH', 'INCLUDE_PATH', 'LD_LIBRARY_PATH'):
            # Backup old environment variables
            oldenv[v] = \
              (os.environ.has_key(v) and (os.environ[v],) or (None,))[0]
        os.environ['LIBRARY_PATH'] = root + libdir
        os.environ['INCLUDE_PATH'] = root + '/usr/include'
        if oldenv['LD_LIBRARY_PATH'] != None:
            os.environ['LD_LIBRARY_PATH'] = \
              root + libdir + ":" + oldenv['LD_LIBRARY_PATH']
        else:
            os.environ['LD_LIBRARY_PATH'] = root + libdir
        
        # Compile
        exe = testdir + "/test"
        inc = """
        -Ibtl/actions
        -Ibtl/generic_bench
        -Ibtl/generic_bench/utils
        -Ibtl/libs/BLAS
        -Ibtl/libs/STL
        """.replace('\n', '')
        libs = "-lrt -L" + root+libdir
        cxxflags = run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()
        defines = "-DCBLASNAME=" + name
        implflags = self._get_flags(root, impl, libdir)
        cl = "g++ %s %s %s %s %s btl/libs/BLAS/main.cpp -o %s" \
            % (inc, libs, cxxflags, defines, implflags, exe)
        cl = shlex.split(cl)
        cp = sp.Popen(cl, stdout=sp.PIPE, stderr=sp.PIPE)
        cp.communicate()
        if cp.returncode != 0:
            raise Exception("Compilation failed: " + " ".join(cl))
        Print("Compilation successful: %s" % " ".join(cl))
        
        # Run test
        args = [exe] + self.tests
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
          cwd = testdir)
        results = []
        while True:
            errline = proc.stderr.readline()
            if not errline:
                break
            resfile = errline.split()[-1]
            results.append(resfile)
            Print(resfile)
            Print.down()
            for i in xrange(100):
                outline = proc.stdout.readline().rstrip()
                Print(outline)
            Print.up()
        Print.up()
        proc.wait()
        if proc.returncode != 0:
            Print('Test failed')
        else:
            Print('Test successful')
        
        # Restore old environment variables
        for v in ('LIBRARY_PATH', 'INCLUDE_PATH', 'LD_LIBRARY_PATH'):
            if oldenv[v] != None:
                os.environ[v] = oldenv[v]
            elif os.environ.has_key(v):
                del os.environ[v]
        return files
