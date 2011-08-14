import sys, os, time
from os.path import join as pjoin
import subprocess as sp
#from benchutils import mkdir
import benchutils as bu

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
    if os.environ.has_key('BTLDIR'):
        btldir = os.environ['BTLDIR']
    else:
        btldir = '/usr/include/btl'
    
    # Library directory (lib32 vs. lib64)
    libdir = sp.Popen \
      ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/', \
      stdout=sp.PIPE, shell=True).communicate()[0].strip()
    
    # roots, tests, packages directories -- report, logs base directories
    if isroot:
        testsdir = "/var/tmp/benchmarks/tests/"
        rootsdir = "/var/tmp/benchmarks/roots/"
        pkgsdir = "/var/cache/benchmarks/packages/"
        
        reportdirb = "/var/cache/benchmarks/reports/"
        logdirb = "/var/log/benchmarks/"+modname+"_"+time.strftime('%Y-%m-%d')
    else:
        testsdir = os.environ['HOME'] + "/.benchmarks/tests/"
        rootsdir = os.environ['HOME'] + "/.benchmarks/roots/"
        pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
        reportdirb = os.environ['HOME'] + "/.benchmarks/reports/"
        logdirb = pjoin(os.environ['HOME'], ".benchmarks/log",
                        modname + "_" + time.strftime('%Y-%m-%d'))
    
    # Report directory
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
    bu.mkdir(rootsdir)
    bu.mkdir(testsdir)
    bu.mkdir(pkgsdir)
    bu.mkdir(reportdir)
    bu.mkdir(logdir)
    
    
    
inizialized = True
