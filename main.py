import commands as cmd
#import portage
import os, string
from PortageUtils import *

# Libraries to be emerged and tested
#libs = {
#    'blas-reference': {},
#    'eigen':{},
#    'atlas': {}
#}

# Retrieve relevant directories
if os.getuid() == 0:
    pkgsdir = "/var/cache/benchmarks/packages/"
else:
    pkgsdir = os.environ['HOME'] + "/.benchmarks/packages/"
rootsdir = "/var/tmp/benchmarks/roots/"
libdir = cmd.getoutput \
  ('ABI=$(portageq envvar ABI); echo /usr/`portageq envvar LIBDIR_$ABI`/')

#CC = 'gcc'
#CXX = 'g++'
#home = os.environ['HOME']

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
        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r0'),
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
        "package" : ('sci-libs', 'blas-reference', '3.3.1', 'r0'),
        "env" : {'FC' : 'ifort'}
    }
}

for tn,(name,test) in enumerate(tests.items(),1):
    print "BEGIN TEST %i" % tn
    
    pkgdir = "%s/%s/" % (pkgsdir, name)
    root = "%s/%s/" % (rootsdir, name)
    
    # Emerge package
    package = "%s/%s-%s-%s" % test['package']
    print "-- Emerging package %s" % package
    try:
        install_package(test['package'], env=test['env'], root=root, pkgdir=pkgdir)
    except InstallException:
        print "-- Package %s failed to emerge" % package
        print
        continue
    
    # Setup environment for testing
    oldenv = {}
    for v in ('LIBRARY_PATH', 'INCLUDE_PATH', 'LD_LIBRARY_PATH'):
        # Backup old environment variables
        oldenv[v] = (os.environ.has_key(v) and (os.environ[v],) or (None,))[0]
    os.environ['LIBRARY_PATH'] = root + libdir
    os.environ['INCLUDE_PATH'] = root + '/usr/include'
    os.environ['LD_LIBRARY_PATH'] = root+libdir+":"+oldenv['LD_LIBRARY_PATH']
    
    # Find implementations -- TODO implementation-specific -> module
    impls = \
      [i for i in os.listdir(root+"/etc/env.d/alternatives/blas") if i[0]!='_']
      
    # Test every implementation
    # TODO implementation-specific -> maybe replace through module.test(...)
    for impl in impls:
        print "-- Testing " + impl
        
        # Retrieve pkgconfig settings and map the directories to the new root
        os.environ['PKG_CONFIG_PATH'] = \
          "%s/etc/env.d/alternatives/blas/%s/%s/pkgconfig" % (root,impl,libdir)
        pkgconf = cmd.getoutput('pkg-config --libs --cflags blas')
        del os.environ['PKG_CONFIG_PATH']
        pkgconf = pkgconf.replace('-L/', '-L'+root+'/')
        pkgconf = pkgconf.replace('-I/', '-I'+root+'/')
        
        # Compile/link the test suite against the library
        print '  -- Compiling with ' + pkgconf
        outexe = "/var/tmp/benchmarks/test_" + name
        cl = "%s %s /var/tmp/test.cpp -o %s" % ('g++', pkgconf, outexe)
        so = cmd.getstatusoutput(cl)
        if so[0] != 0:
            print "  -- Compilation failed: " + cl
            continue
        else:
            print "  -- Compilation successful"
            
        # Execute test suite
        so = cmd.getstatusoutput(outexe)
        if so[0] != 0:
            print "  -- Test failure"
        else:
            print "  -- Test success"
            test['result'] = so[1]
            
    # Restore old environment variables
    for v in ('LIBRARY_PATH', 'INCLUDE_PATH', 'LD_LIBRARY_PATH'):
        if oldenv[v] != None:
            os.environ[v] = oldenv[v]
        elif os.environ.has_key(v):
            del os.environ[v]
    print
    

print tests
exit(0)
