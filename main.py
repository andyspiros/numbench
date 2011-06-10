import os, sys, shlex
from PortageUtils import *
import subprocess as sp
  
class _Print:
    def __init__(self, maxlevel=10):
        self._level = 0
        self._maxlevel = maxlevel
    
    def __call__(self, arg):
        if self._level > self._maxlevel:
            return
        if self._level <= 0:
            print str(arg)
            return
        print (self._level-1)*"  " + "-- " + str(arg)
        
    def up(self, n=1):
        self._level = max(self._level-n, 0)
    
    def down(self, n=1):
        self._level = max(self._level+n, 0)
     
Print = _Print(2)
    
    
# Retrieve relevant directories
if os.getuid() == 0:
    pkgsdir = "/var/cache/benchmarks/packages/"
    figdir = "/var/cache/benchmarks/results/"
else:
    pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
    figdir = os.environ['HOME'] + "/.benchmarks/results/"
figdir += time.strftime('%Y%m%d-%H%M') + '/'
rootsdir = "/var/tmp/benchmarks/roots/"
testsdir = "/var/tmp/benchmarks/tests/"
libdir = sp.Popen \
  ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/', \
  stdout=sp.PIPE, shell=True).communicate()[0].strip()



def print_usage():
    print "Usage: benchmarks [blas|cblas|lapack] file args"
    
    
def tests_from_input(input):
    tests = {}
    for line in input.split('\n'):
        line = line.strip()
        spl = [i.strip() for i in shlex.split(line)]
        if len(spl) < 2:
            continue
        if line[0] == '#':
            continue
        avail = available_packages(spl[1])
        env = {}
        # TODO: add @file for env set based on external file
        # TODO: add -impl for skipping implementation
        for var in spl[2:]:
            s = var.split('=')
            env[s[0]] = s[1]
	avail = available_packages(spl[1])
	if len(avail) > 1:
            for n,p in enumerate(avail):
	            tests[spl[0]+"_%02i"%n] = {'package':p , 'env':env}
	else:
	    tests[spl[0]] = {'package':avail[0] , 'env':env}
    return tests
    
    
# Import the desired module or print help and exit
try:
    tmp = __import__(sys.argv[1], fromlist = ['Module'])
    mod = tmp.Module(Print, libdir, sys.argv[3:])
    del tmp
    testsfname = sys.argv[2]
except ImportError, IndexError:
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
#tests = {
#    "abcde" : {
#        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r1'),
#        "env" : {'FC' : 'gfortran'}
#    },
#         
#    "fghij" : {
#        "package" : ('dev-cpp', 'eigen', '3.0.0', 'r1'),
#        "env" : {'CXX' : 'gcc', 'CXXFLAGS' : '-O2'}
#    },
#         
#    "klmno" : {
#        "package" : ('dev-cpp', 'eigen', '3.0.0', 'r1'),
#        "env" : {'CXX' : 'icc', 'CXXFLAGS' : '-O3'}
#    },
#
#    "pqrst" : {
#        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r1'),
#        "env" : {'FC' : 'ifort'}
#    }
#}


"""
The test variable is generated from a string which can be read from the file.
Here is an example of the parsed input.
Every line contains a configuration and will be an entry in the tests
dictionary; the line has to contain:
- an identification string
- a package description, which can, but does not must to, contain a version
- a list of environment variables separated by means of spaces 
"""
input = file(testsfname).read()
tests = tests_from_input(input)

for tn,(name,test) in enumerate(tests.items(),1):
    Print("BEGIN TEST %i - %s" % (tn, name))
    
    pkgdir = "%s/%s/" % (pkgsdir, name)
    root = "%s/%s/" % (rootsdir, name)
    
    # Emerge package
    Print.down()
    package = "%s/%s-%s-%s" % test['package']
    archive = pkgdir+package+".tbz2"
    Print("Emerging package %s" % package)
    if os.path.exists(archive):
        Print("Package already emerged - skipping")
    else:
        try:
            install_package( \
              test['package'], env=test['env'], root=root, pkgdir=pkgdir)
            # Unpack the archive onto the given root directory
            archive = pkgdir + package + '.tbz2'
            os.path.exists(root) or os.makedirs(root)
            tarcmd = "tar xjf " + archive + " -C " + root
            tarp = sp.Popen(tarcmd, stdout=sp.PIPE, stderr=sp.PIPE, shell=True)
            tarp.communicate()
            if tarp.returncode != 0:
                raise InstallException(tarcmd)
                
        except InstallException as e:
            Print("Package %s failed to emerge: %s" % (package, e.command))
            Print.up()
            print
            continue
    Print("Package emerged")
    
    # Find implementations
    impls = mod.get_impls(root)
    test['implementations'] = impls
      
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
    

                
if not os.path.exists(figdir):
    os.makedirs(figdir)
        
results = {}
for (name,test) in tests.items():
    for impl in test['implementations']:
        results[(name, impl)] = test['results'][impl]

mod.save_results(results, figdir)
