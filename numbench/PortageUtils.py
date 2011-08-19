import commands as cmd
import subprocess as sp
import os, portage, shlex
from os.path import join as pjoin
import benchutils

class InstallException(Exception):
    def __init__(self, command, logfile):
        self.command = command
        self.logfile = logfile
        
def _getEnv(root='/', envAdds={}):
    denv = os.environ.copy()

    #PATH
    denv['PATH'] = ':'.join([pjoin(root, i) for i in ('bin', 'usr/bin')])
    if os.environ.has_key('PATH'):
        denv['PATH'] += ':' + os.environ['PATH']
    denv['ROOTPATH'] = denv['PATH']
    #LIBRARY_PATH
    denv['LIBRARY_PATH'] = ':'.join([pjoin(root, i) for i in \
      ('usr/lib', 'usr/lib64', 'usr/lib32')])
    if os.environ.has_key('LIBRARY_PATH'):
        denv['LIBRARY_PATH'] += ':' + os.environ['LIBRARY_PATH']
    #LD_LIBRARY_PATH
    denv['LD_LIBRARY_PATH'] = ':'.join([pjoin(root, i) for i in \
      ('usr/lib', 'usr/lib64', 'usr/lib32')])
    if os.environ.has_key('LD_LIBRARY_PATH'):
        denv['LD_LIBRARY_PATH'] += ':' + os.environ['LD_LIBRARY_PATH']
    #INCLUDE_PATH
    denv['INCLUDE_PATH'] = ':'.join([pjoin(root, i) for i in ('usr/include',)])
    if os.environ.has_key('INCLUDE_PATH'):
        denv['INCLUDE_PATH'] += ':' + os.environ['INCLUDE_PATH']
        
    # Adds
    for k,v in envAdds.items():
        denv[k] = v
    
    return denv

def available_packages(pattern):
    """Returns a list of packages matching the given pattern.
    
    The packages are returned as (category, package, version, revision) tuple.
    No test for keywords or mask is performed. The function just returns
    every matching pattern in the portage tree and installed overlays.
    """
    return [portage.catpkgsplit(l) \
      for l in cmd.getoutput('equery -q list -po ' + pattern).split()]
    
def normalize_cpv(cpv):
    if type(cpv) == type(''):
        try:
            cpv_ = portage.catpkgsplit(cpv)
            cpv_[-1]
            cpv = cpv_
        except:
            cpv = available_packages(cpv)[-1]
    if cpv[-1] != 'r0':
        return '%s/%s-%s-%s' % cpv
    else:
        return '%s/%s-%s' % cpv[:-1]
        
    
def get_dependencies(package, env={}, split=False):
    pkg = normalize_cpv(package)
    cmd = ['emerge', '--ignore-default-opts', '='+pkg, '-poq']
    proc = sp.Popen(cmd, stdout=sp.PIPE, stderr=sp.PIPE, env=env)
    output =  proc.communicate()[0]
    if proc.returncode != 0:
        return []
    lines = output.strip().split('\n')
    if not lines[0]:
        return []
    if split:
        return [portage.catpkgsplit(shlex.split(l.strip())[-1]) for l in lines]
    else:
        return [shlex.split(l.strip())[-1] for l in lines]

def install_dependencies(package, env={}, root='/', 
                         pkgdir='usr/portage/packages', logdir=None):
    if logdir is None:
        logdir = "/var/log/benchmarks/.unordered"
    
    # Adjust environment
    denv = _getEnv(root, dict(PKGDIR=pkgdir))
    
    # Retrieve dependencies
    deps = get_dependencies(package, denv, False)
    
    for i,d in enumerate(deps):
        logfile = pjoin(logdir, 'emergedep_%i.log' % i)
        install_package(d, env, root, pkgdir, logfile)


def install_package(package, env={}, root='/', pkgdir='usr/portage/packages',
                    logfile=None):
    """Emerge a package in the given root.
    
    package is the package to be emerged. It has to be a tuple
    (category, package, version, revision).
    
    env is a dictionary of KEY:VALUE pairs describing the environment changes
    the package will be emerged with. It is useful to specifiy the compilers and
    compiler flags. It is safe to use ACCEPT_KEYWORDS=* for testing purposes.
    
    root is the directory where the packaged will be emerged. A non-root user
    can use this function, provided he has write access to that directory.
    
    pkgdir is the directory where the binary package will be placed. The user
    has to be able to write in this directory.
    
    The function has no return value and raises an exception in case of building
    or emerging failure. Note: dependencies will NOT be emerged!
    """
    envAdds = env.copy()
    envAdds['PKGDIR'] = pkgdir
    denv = _getEnv(root, envAdds)
    del envAdds
    
    # Retrieve package string
    pkg = normalize_cpv(package)
    
    # Execute emerge command and log the results
    if logfile is not None:
        fout = file(logfile, 'w')
        fout.flush()
    else:
        fout = sp.PIPE
    cmd = ['emerge', '--ignore-default-opts', '-OB', '=' + pkg]
    p = sp.Popen(cmd, env=denv, stdout=fout, stderr=sp.STDOUT)
    p.wait()
    
    if p.returncode != 0:
        # In case of error, print the whole emerge command
        raise InstallException(' '.join(cmd), logfile)
    
    # Unpack package onto root
    benchutils.mkdir(pkgdir)
    benchutils.mkdir(root)
    archive = pjoin(pkgdir, pkg+'.tbz2')
    tarcmd = ['tar', 'xjvf', archive, '-C', root]
    cl = ' '.join(tarcmd)
    if logfile is not None:
        fout.write('\n\n' + 80*'#' + '\n\n')
        fout.write(cl + '\n' + 80*'-' + '\n')
    p = sp.Popen(tarcmd, stdout=fout, stderr=sp.STDOUT)
    p.wait()
    if p.returncode != 0:
        # In case of error, print the whole emerge command
        raise InstallException(cl, logfile)
    
    if logfile is not None:
        fout.close()
    
if __name__ == '__main__':
    # Just a test
    from pprint import pprint
    
    pprint(get_dependencies('sci-libs/blas-reference-3.3.1-r1'))
