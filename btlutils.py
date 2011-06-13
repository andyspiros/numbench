import subprocess as sp
import shlex

run_cmd = lambda c : sp.Popen(c, stdout=sp.PIPE).communicate()[0]

def btlcompile(exe, source, btldir, includes, defines, libs, libdirs, other):
    incs = (
      "%s/actions" % btldir,
      "%s/generic_bench" % btldir,
      "%s/generic_bench/utils" % btldir,
      "%s/libs/STL" % btldir
    ) + tuple(includes)
    incs = ' '.join(['-I'+i for i in incs])
    
    defs = ' '.join(['-D'+d for d in ["NDEBUG"] + defines])
    
    libs = ' '.join(['-l'+l for l in ["rt"] + libs])
    
    libdirs = ' '.join(['-L'+L for L in libdirs])
    
    cxxflags = run_cmd(['portageq', 'envvar', 'CXXFLAGS']).strip()
    
    otherflags = ' '.join(other)
    
    cl = "g++ -o %s %s %s %s %s %s %s %s" \
        % (exe, source, incs, defs, libs, libdirs, cxxflags, otherflags)
    cl = shlex.split(cl)
    cp = sp.Popen(cl, stdout=sp.PIPE, stderr=sp.PIPE)
    cp.communicate()
    return (cp.returncode, ' '.join(cl))
