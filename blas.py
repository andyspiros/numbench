import os
import commands as cmd
import subprocess as sp

run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]

class Module:
    def __init__(self, Print, libdir):
        self.Print = Print
        self.libdir = libdir
    
    @staticmethod
    def get_impls(root):
        return [i for i in os.listdir(root + "/etc/env.d/alternatives/blas") \
          if i[0] != '_']
          
    def run_test(self, root, impl, testdir):
        Print = self.Print
        libdir = self.libdir
        name = 'blas'
        files = ['%s/bench_%s_%s.dat' %(testdir, op, name) for op in (
              'axpy', 'axpby', 'matrix_vector', 'atv', 'symv',
              'syr2', 'ger', 'rot', 'matrix_matrix', 'aat',
              'trisolve_vector', 'trisolve_matrix', 'trmm')]
        
        # Create dir. If all results already exist use them, otherwise
        # remove old results
        runtests = False
        if os.path.exists(testdir):
             runtests = all([os.path.exists(i) for i in files])
        else:
            os.makedirs(testdir)
            runtests = True
        
        if not runtests:
            Print("Not testing: result exist")
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
        os.environ['LD_LIBRARY_PATH']= root+libdir+":"+oldenv['LD_LIBRARY_PATH']
        
        # Retrieve pkgconfig settings and map the directories to the new root
        os.environ['PKG_CONFIG_PATH'] = \
          "%s/etc/env.d/alternatives/blas/%s/%s/pkgconfig" % (root,impl,libdir)
        pkgconf = cmd.getoutput('pkg-config --libs --cflags blas')
        del os.environ['PKG_CONFIG_PATH']
        pkgconf = pkgconf.replace('-L/', '-L'+root+'/')
        pkgconf = pkgconf.replace('-I/', '-I'+root+'/')
        
        # Compile
        exe = testdir + "/test"
        inc = """
        -Ibtl/actions
        -Ibtl/generic_bench
        -Ibtl/generic_bench/utils
        -Ibtl/libs/BLAS
        -Ibtl/libs/STL
        """.replace('\n', ' ')
        libs = "-lrt -L" + root+libdir
        cxxflags = run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()
        defines = "-DCBLASNAME=" + name
        cl = "g++ %s %s %s %s %s btl/libs/BLAS/main.cpp -o %s" \
            % (pkgconf, inc, libs, cxxflags, defines, exe)
        so = cmd.getstatusoutput(cl)
        if so[0] != 0:
            raise Exception("Compilation failed: " + cl)
        Print("Compilation successful: %s" % cl)
        
        # Run test        
        proc = sp.Popen(exe, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, 
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