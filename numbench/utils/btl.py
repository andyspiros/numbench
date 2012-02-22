#=====================================================
# Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
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
from .. import benchchildren, benchutils as bu, benchconfig as cfg
from ..benchprint import Print

from os.path import join as pjoin, dirname
import shlex, subprocess as sp

# BTL global flags
btlincludes = ('actions','generic_bench','generic_bench/utils','libs/STL')
btllibraries = ('rt',)
btldefines = ('NDEBUG',)



def compileTest(test, btlconfig):
    
    # Include directories
    includes = [pjoin(cfg.btldir, i) for i in btlincludes]
    if btlconfig.has_key('btlincludes'):
        includes += [pjoin(cfg.btldir, i) for i in btlconfig['btlincludes']]
    if btlconfig.has_key('includes'):
        includes += btlconfig['includes']
    
    # Linked libraries
    libraries = list(btllibraries)
    if btlconfig.has_key('libraries'):
        libraries += btlconfig['libraries']
    
    # Library directories
    libdirs = [pjoin(test['root'], cfg.libdir)]
    if btlconfig.has_key('libdirs'):
        libdirs += btlconfig['libdirs']
    
    # Defined preprocessor macros
    defines = list(btldefines)
    if btlconfig.has_key('defines'):
        defines += btlconfig['defines']
    
    # Other flags
    flags = []
    
    # Interpret flags
    interpret = shlex.split(bu.run_cmd(['portageq', 'envvar', 'CXXFLAGS']))
    if btlconfig.has_key('flags'):
        interpret += btlconfig['flags']
    for flag in interpret:
        flag = flag.strip()
        if flag[:2] == '-I':
            includes.append(flag[2:])
        elif flag[:2] == '-l':
            libraries.append(flag[2:])
        elif flag[:2] == '-L':
            libdirs.append(flag[2:])
        else:
            flags.append(flag)
    del interpret
    
    
    # Set compile-time environment
    compileenv = test['compileenv'].copy()
    
    if compileenv.has_key('INCLUDE_PATH'):
        compileenv['INCLUDE_PATH'] += ':' + ':'.join(includes)
    else:
        compileenv['INCLUDE_PATH'] = ':'.join(includes)
    
    libenv = ':'.join(libdirs)
    if compileenv.has_key('LIBRARY_PATH'):
        compileenv['LIBRARY_PATH'] += ':' + libenv
    else:
        compileenv['LIBRARY_PATH'] = libenv
        
    if compileenv.has_key('LD_LIBRARY_PATH'):
        compileenv['LD_LIBRARY_PATH'] += ':' + libenv
    else:
        compileenv['LD_LIBRARY_PATH'] = libenv
    
    pathenv = ':'.join([pjoin(test['root'], l) for l in ('bin', 'usr/bin')])
    if compileenv.has_key('PATH'):
        compileenv['PATH'] += pathenv
    else:
        compileenv['PATH'] = pathenv
        
    # Set run-time environment
    runenv = test['runenv'].copy()
    
    if runenv.has_key('LD_LIBRARY_PATH'):
        runenv['LD_LIBRARY_PATH'] += ':' + libenv
    else:
        runenv['LD_LIBRARY_PATH'] = libenv
    
    pathenv = ':'.join([pjoin(test['root'], l) for l in ('bin', 'usr/bin')])
    if runenv.has_key('PATH'):
        runenv['PATH'] += pathenv
    else:
        runenv['PATH'] = pathenv
        
    btlconfig['runenv'] = runenv
    
        
    # Set C++ compiler
    cxx = '/usr/bin/g++'
    
    portageq_cxx = bu.run_cmd(['portageq', 'envvar', 'CXX'])
    if portageq_cxx.strip() != "":
        cxx = portageq_cxx
    del portageq_cxx
    
    if btlconfig.has_key('CXX'):
        cxx = btlconfig['CXX']
    
    if compileenv.has_key('CXX'):
        cxx = compileenv['CXX']
    
    
    # Form command-line arguments
    args = [cxx, pjoin(cfg.btldir, btlconfig['source']), '-o', btlconfig['exe']]
    args += ['-I'+I for I in includes]
    args += ['-l'+l for l in libraries]
    args += ['-L'+L for L in libdirs]
    args += ['-D'+D for D in defines]
    args += flags
    
    # Open logfile and write environment
    bu.mkdir(btlconfig['logdir'])
    logfile = pjoin(btlconfig['logdir'], "btlCompile.log")
    logfs = file(logfile, 'w')
    logfs.write('\n'.join([n+'='+v for n,v in compileenv.items()]))
    logfs.write(3*'\n' + ' '.join(args) + 3*'\n')
    logfs.flush()
    
    # Execute compilation
    bu.mkdir(dirname(btlconfig['exe']))
    proc = sp.Popen(args, stdout=logfs, stderr=sp.STDOUT, env=compileenv)
    proc.wait()
    logfs.flush()
    retcode = proc.returncode
    if retcode == 0:
        logfs.write("\n\n<<< Compilation terminated successfully >>>")
    else:
        logfs.write("\n\n<<< Compilation failed >>>")
    
    # Close, return
    logfs.close()
    return retcode


def runTest(test, btlconfig):
    runenv = btlconfig['runenv']
    
    # Check linking
    logfs = file(pjoin(btlconfig['logdir'], 'btlLinking.log'), 'w')
    sp.Popen(['ldd', '-v', btlconfig['exe']], stdout=logfs, env=runenv).wait()
    logfs.close()
    
    
    # Prepare arguments
    args = (btlconfig['exe'],) + tuple(btlconfig['tests'])
    if btlconfig.has_key('preargs'):
        args = btlconfig['preargs'] + args
    if btlconfig.has_key('postargs'):
        args = args + btlconfig['postargs']
    
    # Open log
    logfs = file(pjoin(btlconfig['logdir'], "btlRun.log"), 'w')
    logfs.write('\n'.join([n+'='+v for n,v in runenv.items()]))
    logfs.write(3*'\n' + ' '.join(args) + 3*'\n')
    logfs.flush()
    
    # Open pipe
    proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=sp.PIPE, \
                    env=runenv, cwd=btlconfig['testdir'])
    benchchildren.append(proc)
    
    result = {}
    
    # Interpret output
    Print('Begin execution')
    while True:
        # Each operation test begins with a line on stderr
        errline = proc.stderr.readline()
        if not errline:
            break
        logfs.write(errline)
        
        resfile = errline.split()[-1]
        operation = resfile.split('_', 1)[-1].rsplit('_', 1)[0]
        result[operation] = pjoin(btlconfig['testdir'], resfile)
        Print(operation + " -> " + resfile)
        
        
        # Many different sizes for each operation test
        Print.down()
        cur = 0
        tot = 100
        while cur != tot:
            outline = proc.stdout.readline()
            # If the line is void, something gone wrong
            if not outline:
                Print.up()
                Print('Execution error')
                return 1
            logfs.write(outline)
            logfs.flush()
            
            # Interpret line
            outline = outline.strip()
            try:
                (cur, tot) = shlex.split(outline)[-1][1:-1].split('/')
                cur = int(cur)
                tot = int(tot)
            except:
                cur += 1
            Print(outline)
            
            
        Print.up()
    proc.wait()
    Print("Execution finished with return code " + str(proc.returncode))
    
    # Close, return
    logfs.close()
    return proc.returncode, result
    
    
    
def selectTests(availableTests, args):
    return tuple([i for i in availableTests if i in args]) 
