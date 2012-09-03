from os.path import dirname, join as pjoin
import os, re, subprocess as sp

from numbench import benchchildren, benchconfig as cfg
from numbench.utils import alternatives as alt, benchutils as bu
from numbench.benchprint import Print

def compileExe(test, libname, implementation):
    src = pjoin(cfg.accudir, 'libraries', libname.upper(), 'main.cpp')
    exe = pjoin(test['testdir'], implementation, 'test')

    # Compiler switches
    includes = ['-I' + pjoin(cfg.accudir, i) for i in
                ('', 'actions', 'utilities')]
    libraries = ['-lm']

    # Flags (also update compile and run environments)
    flags = alt.getFlags(test, libname, implementation)
    libenv = ""
    for i in [j[2:] for j in flags if j.startswith('-L')]:
        libenv += i + ":"

    libenvc = libenv + test['compileenv'].get('LIBRARY_PATH', '')
    test['compileenv']['LIBRARY_PATH'] = libenvc

    libenvr = libenv + test['runenv'].get('LD_LIBRARY_PATH', '')
    test['runenv']['LD_LIBRARY_PATH'] = libenvr

    # Set compile-time environment
    compileenv = test['compileenv'].copy()


    # Set C++ compiler
    cxx = compileenv.get('CXX', '') or \
          bu.run_cmd(['portageq', 'envvar', 'CXX']).strip() or '/usr/bin/g++'


    # Form command-line arguments
    args = [cxx, src, '-o', exe] + includes + libraries + flags

    # Open logfile and write environment
    logdir = pjoin(test['logdir'], implementation)
    bu.mkdir(logdir)
    logfile = pjoin(logdir, "accuracyCompile.log")
    logfs = file(logfile, 'w')
    logfs.write('\n'.join([n + '=' + v for n, v in compileenv.items()]))
    logfs.write(3 * '\n' + ' '.join(args) + 3 * '\n')
    logfs.flush()

    # Execute compilation
    bu.mkdir(dirname(exe))
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
    return retcode, exe


def runExe(test, implementation, exe, args):
    logdir = pjoin(test['logdir'], implementation)
    testdir = pjoin(test['testdir'], implementation)

    # Check linking
    logfs = file(pjoin(logdir, 'accuracyLinking.log'), 'w')
    sp.Popen(['ldd', '-v', exe], stdout=logfs, env=test['runenv']).wait()
    logfs.close()

    # Prepare arguments
    args = (exe,) + tuple(args)

    # Open log
    logfs = file(pjoin(logdir, "accuracyRun.log"), 'w')
    logfs.write('\n'.join([n + '=' + v for n, v in test['runenv'].items()]))
    logfs.write(3 * '\n' + ' '.join(args) + 3 * '\n')
    logfs.flush()

    # Error log
    errfname = pjoin(logdir, "accuracyRun.err")
    errfs = file(errfname, 'w')

    # Open pipe
    try:
        proc = sp.Popen(args, bufsize=1, stdout=sp.PIPE, stderr=errfs, \
                        env=test['runenv'], cwd=testdir)
        benchchildren.append(proc)
    except OSError:
        Print('Execution failed to start')
        Print('Command line: ' + ' '.join(args))
        return -1, None

    # Interpret output
    Print('Begin execution')
    result = interpretOutput(proc.stdout, logfs, testdir)
    proc.wait()
    Print("Execution finished with return code " + str(proc.returncode))

    # Close logs
    logfs.close()
    errp = errfs.tell()
    errfs.close()
    if errp == 0:
        os.unlink(errfname)

    # Close, return
    logfs.close()
    return proc.returncode, result


def interpretOutput(stdout, logfs, testdir):
    result = {}

    operationre = 'Beginning operation on file (accuracy_(.*).dat)'
    while True:
        operation = None
        while operation is None:
            line = stdout.readline()
            if not line:
                break

            try:
                resfile, operation = re.match(operationre, line).groups()
                logfs.write(line)
            except:
                pass

        # Check is program is terminated
        if operation is None:
            break

        result[operation] = pjoin(testdir, resfile)
        Print(operation + " -> " + resfile)


        # Many different sizes for each operation test
        Print.down()

        while True:
            outline = stdout.readline()

            if not outline:
                Print.up()
                Print('Execution error')
                return None

            logfs.write(outline)
            logfs.flush()

            if not outline.startswith(' -- Size'):
                break

            Print(outline[4:].strip())

        Print.up()

    return result


def runTest(self, test, implementation):
    exe = compileExe(test, self.libname, implementation)[1]
    return runExe(test, implementation, exe, self.tests)[1]

def reportConf(*args):
    return {'xscale':'log', 'yscale':'log', 'xlabel':'size', 'ylabel':'Error'}
