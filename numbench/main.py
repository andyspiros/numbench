#! /usr/bin/env python2

import os, sys, signal, shlex, shutil, time
from os.path import join as pjoin
import subprocess as sp

# Set the signal handler
def close(*args):
    load.close()
    benchchilds.terminate()
    Print._level = 0
    Print()
    Print(80*'-')
    Print("INTERRUPT TRIGGERED")
    Print("Exiting")
    exit(0)
signal.signal(signal.SIGINT, close)
    

def print_usage():
    print "Usage: numbench [blas|cblas|lapack|scalapack|fftw|metis|" + \
    "blas_accuracy|lapack_accuracy] file args"

def print_help():
    print "Usage: numbench module conffile [options]"
    print "       numbench [ -h | --help ]"
    print "       numbench module [ -h | --help ]"
    print
    print "Modules:"
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
    
def readEnvFile(fname):
    """Reads a bash file with void environment and returns the environment
    at the end of the execution."""
    proc = sp.Popen('. '+fname+' &> /dev/null; env', \
      shell=True, stdout=sp.PIPE, env={})
    lines = proc.stdout.read().split('\n')[:-1]
    env = dict([l.split('=', 1) for l in lines])
    
    for k in ('SHLVL', 'PWD', '_'):
        if env.has_key(k):
            del env[k]
    return env
    
    
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
        skip = []
        change = {}
        descr = None
        fileenv = {}
        
        # Interpret arguments
        for var in spl[2:]:
            
            # if begins with '-': skip implementation
            if var[0] == '-':
                skip.append(var[1:])
                
            # if key:value, substitute pkg-config dependency
            elif ':' in var and not '=' in var:
                c_0, c_1 = var.split(':', 1)
                change[c_0] = c_1
                
            # if descr|text set description (for future use)
            elif var[:6] == 'descr|':
                descr = var[6:]
                
            # if @file: read bash script and set env
            elif var[0] == '@':
                fileenv = readEnvFile(pjoin(cfg.curdir, var[1:]))
            
            # Otherwise, assume key=value syntax
            else:
                e_0, e_1 = var.split('=', 1)
                env[e_0] = e_1
                
        # Set environment (argument overrides bash file)
        env = dict( fileenv.items() + env.items() )
        
        try:
            # Insert test
            avail = available_packages(spl[1])[-1]
            tests[spl[0]] = {'package':avail , 'env':env, 'skip':skip, \
              'changes':change, 'descr':descr}
        except:
            # Or trigger an non-fatal error
            sys.stderr.write('Error: package ' + spl[1] + ' not found\n')
    return tests


##########################
# HERE BEGINS THE SCRIPT #
##########################

import benchconfig as cfg
import benchchilds
import benchutils as bu


# If no argument is given, print the help
if (len(sys.argv) < 2):
    print_help()
    exit(0)


# Import the desired module or print help and exit
try:

    # Print main help
    if (sys.argv[1] in ('-h', '--help')):
        print_help()
        exit(0);
    
    cfg.modulename = sys.argv[1]
    
    # Print module help
    if (sys.argv[2] in ('-h', '--help')):
        cfg.inputfile = ''
        tmp = __import__('numbench.'+cfg.modulename, fromlist = ['Module'])
        tmp.Module.printHelp()
        exit(0)
    
    # Normal run: import module
    cfg.inputfile = os.path.abspath(sys.argv[2])
    os.chdir(cfg.scriptdir)
    tmp = __import__('numbench.'+cfg.modulename, fromlist = ['Module'])
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
import benchload as load



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
Print._level = 0
Print(80*'=')
Print("The following tests will be run:")
Print("-------------------------------")
Print()
for tname, ttest in cfg.tests.items():
    Print("Test: " + tname)
    if ttest['descr'] is not None:
        Print(" - Description: " + ttest['descr'])
    Print(" - Package: " + normalize_cpv(ttest['package']))
    if len(ttest['env']) != 0:
        Print(" - Environment: " + \
          ' '.join([n+'="'+v+'"' for n,v in ttest['env'].items()]))
    if len(ttest['skip']) != 0:
        Print(" - Skip implementations: " + ' '.join(ttest['skip']))
    if len(ttest['changes']) != 0:
        Print(" - Dependency substitutions:", '')
        for c_0, c_1 in ttest['changes'].items():
            Print(c_0 + ':' + c_1, '')
        Print()
    Print()
Print(80*'=')
Print()
Print("The script is located in the directory " + cfg.scriptdir)
Print("The script is run from the directory " + os.path.realpath('.'))
Print("The BTL is located in the directory " + cfg.btldir)
Print("The logs will be available in the directory " + cfg.logdir)
Print("The results will be available in the directory " + cfg.reportdir)
Print()

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
    

load.close()

# Save the results (first re-order them)     
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
