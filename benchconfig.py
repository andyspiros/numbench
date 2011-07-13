import sys, os, time
import subprocess as sp
from benchutils import *

try:
    needsinitialization = not initialized
except NameError:
    needsinitialization = True


if needsinitialization:
    isroot = os.getuid() == 0
    modname = sys.argv[1]
    
    # Script directories
    curdir = os.path.abspath('.')
    scriptdir = os.path.dirname(os.path.realpath(__file__))
    btldir = 'btl'
    
    # Invariant directories:
    # roots, tests
    rootsdir = "/var/tmp/benchmarks/roots/"
    testsdir = "/var/tmp/benchmarks/tests/"
    
    # Library directory (lib32 vs. lib64)
    libdir = sp.Popen \
      ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/', \
      stdout=sp.PIPE, shell=True).communicate()[0].strip()
    
    # Packages directory
    if isroot:
        pkgsdir = "/var/cache/benchmarks/packages/"
    else:
        pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
    
    # Report directory
    if isroot:
        reportdirb = "/var/cache/benchmarks/reports/"
    else:
        reportdirb = os.environ['HOME'] + "/.benchmarks/reports/"
    reportdirb += modname + "_" + time.strftime('%Y-%m-%d')
    if os.path.exists(reportdirb):
        n = 1
        while True:
            reportdir = reportdirb + "_%i" % n
            if not os.path.exists(reportdir):
                break
            n += 1
    else:
        reportdir = reportdirb
    del reportdirb
    
    # Logs directory
    if isroot:
        logdirb = "/var/log/benchmarks/"+modname+"_"+time.strftime('%Y-%m-%d')
    else:
        logdirb = pjoin(os.environ['HOME'], ".benchmarks/log",
                        modname + "_" + time.strftime('%Y-%m-%d'))
    if os.path.exists(logdirb):
        n = 1
        while True:
            logdir = logdirb + "_%i"%n
            if not os.path.exists(logdir):
                break
            n += 1
    else:
        logdir = logdirb
    del logdirb
    
def makedirs():
    mkdir(rootsdir)
    mkdir(testsdir)
    mkdir(pkgsdir)
    mkdir(reportdir)
    mkdir(logdir)
    
    
    
inizialized = True