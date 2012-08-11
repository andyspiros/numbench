#! /usr/bin/env python2

#=====================================================
# Copyright (C) 2011-2012 Andrea Arteaga <andyspiros@gmail.com>
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

import os, sys, signal
import benchchildren
import modules

# Set the signal handler
def close(*args):
    benchchildren.terminate()
    print
    print 80 * '-'
    print "INTERRUPT TRIGGERED"
    print "Exiting"
    exit(0)
signal.signal(signal.SIGINT, close)


def print_help():
    print "Usage: numbench conffile [options]"
    print "       numbench [ -h | --help ]"
#    print "       numbench module [ -h | --help ]"
    print
    print "Options:"
    print "   [ -h | --help ] - Displays an help message."
    print
    print "   [ -d | --directory] dir - Stores the data in the given directory."
    print "       If not given, a directory in ~/.numbench is chosen."
    print
    print "   [ -c | --clean] - Removes the temporary data."
    print
    print "   [ -i | --imageformat] format - Selects the given format for the"
    print "       resulting images. Available are png, svg, eps, ps, pdf."
    print "       Default is svg."
    print

    modnames = modules.getModulesNames()

    print "Modules:"
    for m in modnames:
        M = modules.loadModule(m)
        print "   %s - %s" % (m, M.descr)
#    print "   blas - Test BLAS implementations"
#    print "   cblas - Test CBLAS implementations"
#    print "   lapack - Test LAPACK implementations"
#    print "   lapacke - Test LAPACK implementations"
#    print "   scalapack - Test the ScaLAPACK library"
#    #print "   blas_accuracy - Test BLAS implementations for accuracy"
#    #print "   lapack_accuracy - Test LAPACK implementations for accuracy"
#    print "   fftw - Test the FFTW library"
#    #print "   metis - Test the METIS tools"
    print
    print "More information about a module is available through the command:"
    print "  numbench module --help"



## PRINT HELP IF NEEDED

# If no argument or '-h' is given, print the help
if len(sys.argv) < 2 or sys.argv[1] in ('-h', '--help'):
    print_help()
    exit(0)


## BEGIN THE TRUE SCRIPT

# Import the packages
import re
from fnmatch import fnmatch
from os.path import join as pjoin

import benchconfig as cfg
from xmlinput import Parser
from utils import envread, benchutils as bu, portageutils as pu
import benchprint

## Set-up the run configuration

# Parse the arguments
cfg.parseArguments()

# Start configuration parser
if not os.path.exists(cfg.inputfile):
    sys.stderr.write("File not found: " + cfg.inputfile)
    print_help()
    exit(1)
parser = Parser(cfg.inputfile)

# Get module name and arguments
cfg.modulename = parser.getModuleName()
cfg.moduleargs = parser.getModuleArguments()

# Set-up directories
cfg.setDirs()

# Get test cases
cfg.tests = parser.getTestCases()

# Initialize print system
Print = benchprint.initializePrint()

# Import the module
cfg.module = modules.loadModule(cfg.modulename, cfg.moduleargs)


## Write summary
Print._level = 0
Print(80 * '=')
Print("The following tests will be run:")
Print("-------------------------------")
Print()
for tname, ttest in cfg.tests.items():
    Print("Test: " + tname)

    if ttest['descr'] is not None:
        Print(" - Description: " + ttest['descr'])

    Print(" - Package: " + ttest['normalizedPackage'])

    if len(ttest['dependenv']) != 0:
        Print(" - Dependencies emerge environment: " + \
          ' '.join([n + '="' + v + '"' for n, v in ttest['dependenv'].items()]))

    if len(ttest['emergeenv']) != 0:
        Print(" - Emerge environment: " + \
          ' '.join([n + '="' + v + '"' for n, v in ttest['emergeenv'].items()]))

    if len(ttest['compileenv']) != 0:
        Print(" - Suite compile-time environment: " + \
          ' '.join([n + '="' + v + '"' for n, v in ttest['compileenv'].items()]))

    if len(ttest['runenv']) != 0:
        Print(" - Suite run-time environment: " + \
          ' '.join([n + '="' + v + '"' for n, v in ttest['runenv'].items()]))

    if len(ttest['skip']) != 0:
        Print(" - Skip implementations: " + ' '.join(ttest['skip']))

    if len(ttest['skipre']) != 0:
        Print(" - Skip implementations (regular expressions): " + \
              ' '.join(ttest['skipre']))

    if len(ttest['requires']) != 0:
        Print(" - Pkg-config requirements substitutions:", '')
        for c_0, c_1 in ttest['requires'].items():
            Print(c_0 + ':' + c_1, '')
        Print()

    Print()
Print(80 * '=')
Print()
Print("The script is located in the directory " + cfg.scriptdir)
Print("The script is run from the directory " + os.path.realpath('.'))
Print("The BTL is located in the directory " + cfg.btldir)
Print("The logs will be available in the directory " + cfg.logdir)
Print("The results will be available in the directory " + cfg.reportdir)
Print()


## Main iteration
for tn, (name, test) in enumerate(cfg.tests.items(), 1):
    Print._level = 0
    Print("BEGIN TEST %i - %s" % (tn, name))

    # Emerge package
    Print.down()
    if os.path.exists(test['archive']):
        Print("Package already emerged - skipping")
        test['emergesuccess'] = True
    else:
        try:
            # Emerge dependencies
            Print("Emerging dependencies")
            pu.installDependencies(test)

            # Emerge pacakge
            Print("Emerging package %s" % test['normalizedPackage'])
            logfile = pjoin(test['logdir'], 'emerge.log')
            Print("(Run 'tail -f " + logfile + "' on another terminal "
                  "to see the progress)")
            pu.installPackage(test)
            test['emergesuccess'] = True

        except pu.InstallException as e:
            test['emergesuccess'] = False
            Print("Package %s failed to emerge" % e.package)
            Print("Error log: " + e.logfile)
            Print.up()
            print
            continue
    Print("Package emerged")

    # Find implementations
    impls = []
    for i in cfg.module.getImplementations(test):
        skip = False

        for s in test['skip']:
            if fnmatch(i, s):
                skip = True
                break

        for s in test['skipre']:
            if re.search(s, i) != None:
                skip = True
                break

        if not skip:
            impls.append(i)

    test['implementations'] = impls

    # Automatically add environment
    envread.envread(test)

    # Test every implementation
    test['results'] = {}
    if len(impls) == 0:
        Print("No implementation found")
    for impl in impls:
        # Run the test suite
        Print("Testing " + impl)
        Print.down()
        test['results'][impl] = cfg.module.runTest(test, impl)
        Print.up()
    # All implementations tested

    Print.up()
    print


## End of execution

# Save the results
import report
report.saveReport()

# Clean up the directories
if cfg.clean:
    bu.rmdir(cfg.testsdir)
    bu.rmdir(cfg.rootsdir)


# TODO: reintroduce the instructions feature (and remove "exit)
exit(0)


Print._level = 0
Print()
# Print instructions
for name, test in cfg.tests.items():
    if not test['emergesuccess']:
        continue
    printstr = "Instructions for " + name + ":"
    Print(printstr)
    Print(len(printstr) * '-')
    Print.down()
    Print("# PKGDIR=" + test['pkgdir'] + " emerge -K '=" + \
          test['normalizedPackage'] + "'")
    try:
        for impl in test['implementations']:
            Print("Implementation " + impl + ":")
            Print.down()
            cfg.module.instructionsFor(impl)
            Print.up()
    except:
        pass

    Print.up()
    Print()
