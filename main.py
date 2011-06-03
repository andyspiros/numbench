import os, sys
import commands as cmd
from PortageUtils import *
  
class _Print:
    def __init__(self):
        self._level = 0
    
    def __call__(self, arg):
        if self._level <= 0:
            print str(arg)
            return
        print (self._level-1)*"  " + "-- " + str(arg)
        
    def up(self, n=1):
        self._level = max(self._level-n, 0)
    
    def down(self, n=1):
        self._level = max(self._level+n, 0)
     
Print = _Print()
    
    
# Retrieve relevant directories
if os.getuid() == 0:
    pkgsdir = "/var/cache/benchmarks/packages/"
else:
    pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
rootsdir = "/var/tmp/benchmarks/roots/"
testsdir = "/var/tmp/benchmarks/tests/"
libdir = cmd.getoutput \
  ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/')



def print_usage():
    print "Usage: benchmarks [blas|cblas|lapack]"
    
    
# Import the desired module or print help and exit
try:
    tmp = __import__(sys.argv[1], fromlist = ['Module'])
    mod = tmp.Module(Print, libdir)
    del tmp
except:
    print_usage()
    exit(1)


"""
The test is the main configuration variable. Every entry in this dictionary
represents a package that has to be tested with his special environment,
which can contain information about the compiler, the flags,...
The dictionary key (e.g. "abcde" here) is just an identification method for
the test; it is safe to generate a random key, with some attention to avoid
overlapping keys.
Every entry (which is a dictionary itself) has to contain the item "package" in
the form of a tuple (category, package, version, revision) [see
portage.catpkgsplit], and the item "env", which describes the environment to be
used at compile-time as dictionary (it can just be a void one).
After the tests every successful tested item will contain the item "result",
which can contain any type of data and will be used for the final report.
"""
tests = {
    "abcde" : {
        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r1'),
        "env" : {'FC' : 'gfortran'}
    },
         
    "fghij" : {
        "package" : ('dev-cpp', 'eigen', '3.0.0', 'r1'),
        "env" : {'CXX' : 'gcc', 'CXXFLAGS' : '-O2'}
    },
         
#    "klmno" : {
#        "package" : ('dev-cpp', 'eigen', '3.0.0', 'r1'),
#        "env" : {'CXX' : 'icc', 'CXXFLAGS' : '-O3'}
#    },

    "pqrst" : {
        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r1'),
        "env" : {'FC' : 'ifort'}
    }
}

for tn,(name,test) in enumerate(tests.items(),1):
    Print("BEGIN TEST %i" % tn)
    
    pkgdir = "%s/%s/" % (pkgsdir, name)
    root = "%s/%s/" % (rootsdir, name)
    
    # Emerge package
    Print.down()
    package = "%s/%s-%s-%s" % test['package']
    Print("Emerging package %s" % package)
    try:
        install_package( \
          test['package'], env=test['env'], root=root, pkgdir=pkgdir)
    except InstallException as e:
        Print("Package %s failed to emerge: %s" % (package, e.command))
        Print.up()
        print
        continue
    Print("Package emerged")
    
    # Find implementations
    impls = mod.get_impls(root)
      
    # Test every implementation
    test['results'] = {}
    for impl in impls:
        Print("Testing " + impl)
        Print.down()
        
        # Compile/link the test suite against the library
        testdir = "%s/%s/%s" % (testsdir, name, impl)
        test['results'][impl] = mod.run_test(root, impl, testdir)
        Print.up()
            
    Print.up()
    print
    

print tests
exit(0)
