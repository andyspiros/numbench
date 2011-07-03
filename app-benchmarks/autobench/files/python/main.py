#! /usr/bin/env python2

import os, sys, shlex
from os.path import join as pjoin
from PortageUtils import *
import subprocess as sp
import time
 
# Retrieve relevant files/directories
# TODO: use external config module to share these variables (or use environ?)
curdir = os.path.abspath('.')
scriptdir = os.path.dirname(os.path.realpath(__file__))
rootsdir = "/var/tmp/benchmarks/roots/"
testsdir = "/var/tmp/benchmarks/tests/"
if os.getuid() == 0:
    pkgsdir = "/var/cache/benchmarks/packages/"
    figdirb = "/var/cache/benchmarks/results/"
else:
    pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
    figdirb = os.environ['HOME'] + "/.benchmarks/results/"
    
# Library directory (lib32 vs. lib64)
libdir = sp.Popen \
  ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/', \
  stdout=sp.PIPE, shell=True).communicate()[0].strip()
    
# Figures directory
figdir = figdirb + time.strftime('%Y-%m-%d')
if os.path.exists(figdir):
    n = 1
    while True:
        figdir = figdirb + time.strftime('%Y-%m-%d') + "_%i"%n
        if not os.path.exists(figdir):
            os.makedirs(figdir)
            break
        n += 1
else:
    os.makedirs(figdir)
  
# Logs directory
logdir = "/var/log/benchmarks/" + time.strftime('%Y-%m-%d')
if os.path.exists(logdir):
    n = 1
    while True:
        logdir = "/var/log/benchmarks/" + time.strftime('%Y-%m-%d') + "_%i"%n
        if not os.path.exists(logdir):
            os.makedirs(logdir)
            break
        n += 1
else:
    os.makedirs(logdir)

def print_usage():
    print "Usage: benchmarks [blas|cblas|lapack] file args"   
      
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
Print = _Print(3)

# Import the desired module or print help and exit
try:
    testsfname = os.path.abspath(sys.argv[2])
    os.chdir(scriptdir)
    tmp = __import__(sys.argv[1], fromlist = ['Module'])
    mod = tmp.Module(Print, libdir, sys.argv[3:])
    del tmp
except ImportError, IndexError:
    print_usage()
    exit(1)
  
    
def tests_from_input(input):
    tests = {}
    for line in input.split('\n'):
        line = line.strip()
        spl = [i.strip() for i in shlex.split(line)]
        if len(spl) < 2:
            continue
        if line[0] == '#':
            continue
        env = {}
        # TODO: add @file for env set based on external file
        # TODO: add -impl for skipping implementation
        for var in spl[2:]:
            s = var.split('=', 1)
            env[s[0]] = s[1]
        avail = available_packages(spl[1])
        if len(avail) > 1:
            for n,p in enumerate(avail):
                tests[spl[0]+"_%02i"%n] = {'package':p , 'env':env}
        elif len(avail) == 1:
            tests[spl[0]] = {'package':avail[0] , 'env':env}
        else:
            sys.stderr.write('Error: package ' + spl[1] + ' not found\n')
    return tests
    



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
#    "reference-gfortran" : {
#        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r1'),
#        "env" : {'FC' : 'gfortran'}
#    },
#         
#    "eigen-gcc" : {
#        "package" : ('dev-cpp', 'eigen', '3.0.0', 'r1'),
#        "env" : {'CXX' : 'g++', 'CXXFLAGS' : '-O2'}
#    },
#         
#    "eigen-icc" : {
#        "package" : ('dev-cpp', 'eigen', '3.0.0', 'r1'),
#        "env" : {'CXX' : 'icc', 'CXXFLAGS' : '-O3'}
#    },
#
#    "reference-ifort" : {
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

# Write summary
print 80*'='
print "The following tests will be run:"
for tname, ttest in tests.items():
    print "Test: " + tname
    print " - Package: " + normalize_cpv(ttest['package'])
    print " - Environment: " + \
      ' '.join([n+'="'+v+'"' for n,v in ttest['env'].items()])
    print
print 80*'='
print

for tn,(name,test) in enumerate(tests.items(),1):
    Print("BEGIN TEST %i - %s" % (tn, name))
    
    pkgdir = pjoin(pkgsdir, name)
    root = pjoin(rootsdir, name)
    tlogdir = pjoin(logdir, name)
    os.path.exists(tlogdir) or os.makedirs(tlogdir)
    
    # Emerge package
    Print.down()
    package = normalize_cpv(test['package'])
    archive = pjoin(pkgdir, package+".tbz2")
    Print("Emerging package %s" % package)
    if os.path.exists(archive):
        Print("Package already emerged - skipping")
    else:
        try:
            logfile = pjoin(tlogdir, 'emerge.log')
            Print("(Run 'tail -f " + logfile + " | less' on another terminal" \
              + " to see the progress)")
            install_package( \
              test['package'], env=test['env'], root=root, pkgdir=pkgdir, \
              logfile=logfile
              )
            # Unpack the archive onto the given root directory
            os.path.exists(root) or os.makedirs(root)
            tarcmd = ['tar', 'xjf', archive, '-C', root]
            logfile = file(pjoin(tlogdir, 'tar.log'), 'w')
            tarp = sp.Popen(tarcmd, stdout=logfile, stderr=sp.STDOUT)
            tarp.wait()
            logfile.close()
            if tarp.returncode != 0:
                raise InstallException(tarcmd, logfile.name)
                
        except InstallException as e:
            Print("Package %s failed to emerge" % package)
            Print("Error log: " + e.logfile)
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
        
        # Run the test suite
        testdir = os.path.join(testsdir, name, impl)
        test['results'][impl] = \
          mod.run_test(root, impl, testdir, env=test['env'], logdir=tlogdir)
        Print.up()
            
    Print.up()
    print
    

# Reports will be saved in figdir

# Results are reordered:
# results
# |-(name1, impl1) -> resultobject11
# |-(name1, impl2) -> resultobject12
# |-(name2, impl1) -> resultobject21
os.path.exists(figdir) or os.makedirs(figdir)        
results = {}
for (name,test) in tests.items():
    if test.has_key('implementations'):
        for impl in test['implementations']:
            results[(name, impl)] = test['results'][impl]

mod.save_results(results, figdir)
