#! /usr/bin/env python2

import os, sys, shlex, time
from os.path import join as pjoin
import subprocess as sp

def print_usage():
    print "Usage: numbench [blas|cblas|lapack|scalapack|fftw|metis|" + \
    "blas_accuracy|lapack_accuracy] file args"

def print_help():
    print "Usage: numbench module conffile [options]"
    print "       numbench module [-h|--help]"
    print
    print "numbench is a tool for compiling and benchmarking numerical"
    print "libraries. It is useful to find out the implementations of common"
    print "libraries that run faster on the machine and are more accurate."
    print "The script makes use of the portage features in order to download,"
    print "compile, install and test the packages"
    print
    print
    print "Modules:"
    print "The following modules are available:"
    print "   blas - Test BLAS implementations"
    print "   cblas - Test CBLAS implementations"
    print "   lapack - Test LAPACK implementations"
    print "   scalapack - Test the ScaLAPACK library"
    print "   blas_accuracy - Test BLAS implementations for accuracy"
    print "   lapack_accuracy - Test LAPACK implementations for accuracy"
    print "   fftw - Test the FFTW library"
    print "   metis - Test the METIS tools"
    print
    print "More information about a module is available through the command:"
    print "  numbench module --help"
    print
    print
    print "Config file:"
    print "In order to run a test, a configuration file has to be provided."
    print "Each line of this file defines a package that is to be tested."
    print "It is possible to test different versions of the same package, or"
    print "even to test many times the same package with different compile-time"
    print "environment; for each different version or environment, a different"
    print "line has to be written."
    print
    print "Each line begins with an identification token of the test. This"
    print "identification can contain any characters, but it is recommended"
    print "that it only contains alphanumeric digits, the period . , the minus"
    print "sign - and the underscore _ ."
    print "After the identification word, the package has to be provided. The"
    print "package specification should be fully provided, in the usual"
    print "category/package-version[-revision] format. For instance"
    print "sci-libs/lapack-reference-3.3.1-r1. However, the script will try to"
    print "choose the best package in case of lacking information."
    print "After the package, the environment has to be specified. In order"
    print "to do that, the user has to use the KEY=VALUE format. If the value"
    print "contains a whitespace, the single or double quoting marks have to be"
    print "used. The following are two valid configuration lines that define"
    print "the tests for the sci-libs/atlas package with different compilers"
    print "and CFLAGS:"
    print
    print "atlas-gcc sci-libs/atlas-3.9.46 CC=gcc CFLAGS=-O2"
    print "atlas-gcc-4.6.1 sci-libs/atlas-3.9.46 CC=gcc-4.6.1",
    print "CFLAGS=\"-O3 -march=native\""
    print
    print "Variables that affect the emerge process, such as USE, can be used"
    print "and are effective."
    print "More configuration options are available. As each package can"
    print "install many implementations of the same library (for instance, the"
    print "sci-libs/atlas package installs the serial version and the"
    print "parallelized version with threads or openmp, depending on the USE"
    print "flags), each implementation is tested; but if you do not want to"
    print "test a specific implementation, you can mask it by adding to the"
    print "configuration line the string '-implementation' (without quoting"
    print "marks); then the script will skip the implementation. The following"
    print "is an example where the 32-bit implementations of the acml are"
    print "skipped:"
    print
    print "acml sci-libs/acml-4.4.0 -acml32-gfortran -acml32-gfortran-openmp"
    print
    print "The last configuration option that can be used is for libraries that"
    print "internally use other libraries. For example, most LAPACK"
    print "implementations lie on BLAS and/or CBLAS implementations. It is"
    print "possible to make the tested LAPACK implementation use a specific"
    print "BLAS implementation through the 'blas:implementation' (without"
    print "quotation marks) format. For instance, the following line"
    print "defines a test where the atlas LAPACK implementation uses the"
    print "openblas library as BLAS and CBLAS (openblas has to be installed for"
    print "this to work):"
    print
    print "atlas sci-libs/atlas-3.9.46 blas:openblas"
    
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
        skip = []
        change = {}
        descr = None
        for var in spl[2:]:
            if var[0] == '-':
                skip.append(var[1:])
            elif ':' in var and not '=' in var:
                c_0, c_1 = var.split(':', 1)
                change[c_0] = c_1
            elif var[:6] == 'descr|':
                descr = var[6:]
            else:
                e_0, e_1 = var.split('=', 1)
                env[e_0] = e_1
        try:
            avail = available_packages(spl[1])[-1]
            tests[spl[0]] = {'package':avail , 'env':env, 'skip':skip, \
              'changes':change, 'descr':descr}
        except:
            sys.stderr.write('Error: package ' + spl[1] + ' not found\n')
    return tests

import benchconfig as cfg


# Import the desired module or print help and exit
try:

    # Print main help
    if (sys.argv[1] in ('-h', '--help')):
        print_help()
        exit(0);
    
    cfg.modulename = sys.argv[1]
    
    # Print module help
    if (sys.argv[2] in ('-h', '--help')):
        os.chdir(cfg.scriptdir)
        cfg.inputfile = ''
        tmp = __import__(cfg.modulename, fromlist = ['Module'])
        tmp.Module.printHelp()
        exit(0)
    
    # Normal run: import module
    cfg.inputfile = os.path.abspath(sys.argv[2])
    os.chdir(cfg.scriptdir)
    tmp = __import__(sys.argv[1], fromlist = ['Module'])
    mod = tmp.Module(sys.argv[3:])
    del tmp
    cfg.makedirs()
    
except ImportError as e:
    print e
    print_usage()
    exit(1)
except IndexError:
    print_usage()
    exit(1)
  
from PortageUtils import *
from benchprint import Print



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


"""
The test variable is generated from a string which can be read from the file.
Here is an example of the parsed input.
Every line contains a configuration and will be an entry in the tests
dictionary; the line has to contain:
- an identification string
- a package description, which can, but does not must to, contain a version
- a list of environment variables separated by means of spaces 
"""
if not os.path.exists(cfg.inputfile):
    sys.stderr.write("File not found: " + cfg.inputfile)
    print_usage()
    exit(1)
input = file(cfg.inputfile).read()
cfg.tests = tests_from_input(input)

# Write summary
print 80*'='
print "The following tests will be run:"
print "-------------------------------"
print
for tname, ttest in cfg.tests.items():
    print "Test: " + tname
    if ttest['descr'] is not None:
        print " - Description: " + ttest['descr']
    print " - Package: " + normalize_cpv(ttest['package'])
    if len(ttest['env']) != 0:
        print " - Environment: " + \
          ' '.join([n+'="'+v+'"' for n,v in ttest['env'].items()])
    if len(ttest['skip']) != 0:
        print " - Skip implementations: " + ' '.join(ttest['skip'])
    if len(ttest['changes']) != 0:
        print " - Dependency specifications:",
        for c_0, c_1 in ttest['changes'].items():
            print c_0 + ':' + c_1,
        print
    print
print 80*'='
print

for tn,(name,test) in enumerate(cfg.tests.items(),1):
    Print._level = 0
    Print("BEGIN TEST %i - %s" % (tn, name))
    
    pkgdir = pjoin(cfg.pkgsdir, name)
    root = pjoin(cfg.rootsdir, name)
    tlogdir = pjoin(cfg.logdir, name)
    os.path.exists(tlogdir) or os.makedirs(tlogdir)
    
    # Emerge package
    Print.down()
    package = normalize_cpv(test['package'])
    archive = pjoin(pkgdir, package+".tbz2")
    test['pkgdir'] = pkgdir
    test['archive'] = archive
    if os.path.exists(archive):
        Print("Package already emerged - skipping")
        test['emergesuccess'] = True
    else:
        try:
            # Emerge dependencies
            Print("Emerging dependencies")
            install_dependencies( \
              test['package'], root=root, pkgdir=pkgdir, \
              logdir=tlogdir)
            
            # Emerge pacakge
            Print("Emerging package %s" % package)
            logfile = pjoin(tlogdir, 'emerge.log')
            Print("(Run 'tail -f " + logfile + "' on another terminal" \
              + " to see the progress)")
            install_package( \
              test['package'], env=test['env'], root=root, pkgdir=pkgdir, \
              logfile=logfile
              )
            test['emergesuccess'] = True
                
        except InstallException as e:
            test['emergesuccess'] = False
            Print("Package %s failed to emerge" % package)
            Print("Error log: " + e.logfile)
            Print.up()
            print
            continue
    Print("Package emerged")
    
    # Find implementations
    impls = [i for i in mod.get_impls(root) if not i in test['skip']]
    test['implementations'] = impls
    
    # Test every implementation
    test['results'] = {}
    if len(impls) == 0:
        Print("No implementation found")
    for impl in impls:
        Print("Testing " + impl)
        Print.down()
        
        # Run the test suite
        testdir = os.path.join(cfg.testsdir, name, impl)
        t = mod.getTest(root, impl, testdir, logdir=tlogdir)
        test['results'][impl] = t.run_test(test['changes'])
        Print.up()
            
    Print.up()
    print
    

# Reports will be saved in cfg.reportdir

# Results are reordered:
# results
# |-(name1, impl1) -> resultobject11
# |-(name1, impl2) -> resultobject12
# |-(name2, impl1) -> resultobject21        
results = {}
for (name,test) in cfg.tests.items():
    if test.has_key('implementations'):
        for impl in test['implementations']:
            results[(name, impl)] = test['results'][impl]

mod.save_results(results)


Print._level = 0
Print()
# Print instructions
for name,test in cfg.tests.items():
    if not test['emergesuccess']:
        continue
    printstr = "Instructions for " + name + ":"
    Print(printstr)
    Print(len(printstr)*'-')
    Print.down()
    Print("# PKGDIR=" + test['pkgdir'] + " emerge -K '=" + \
          normalize_cpv(test['package']) + "'")
    try:
        for impl in test['implementations']:
            Print("Implementation " + impl + ":")
            Print.down()
            mod.instructionsFor(impl)
            Print.up()
    except:
        pass
        
    Print.up()
    Print()
