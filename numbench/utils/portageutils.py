#=====================================================
# Copyright (C) 2011 Andrea Arteaga <andyspiros@gmail.com>
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
import commands as cmd
import subprocess as sp
import os, portage, shlex
from os.path import join as pjoin, dirname
import benchutils as bu

class InstallException(Exception):
    def __init__(self, package, command, logfile):
        self.package = package
        self.command = command
        self.logfile = logfile

def _getEnv(root='/', envAdds={}):
    #denv = os.environ.copy()
    denv = {}

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

def availablePackages(pattern):
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
            cpv = availablePackages(cpv)[-1]
    if cpv[-1] != 'r0':
        return '%s/%s-%s-%s' % cpv
    else:
        return '%s/%s-%s' % cpv[:-1]


def getDependencies(package, env={}, split=False):
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


def installDependencies(test):
    # Adjust environment
    denv = _getEnv(test['root'], dict(PKGDIR=test['pkgdir']))

    # Retrieve dependencies
    deps = getDependencies(test['package'], denv, False)

    for i,d in enumerate(deps):
        logfile = pjoin(test['logdir'], 'emergedep_%i.log' % i)
        installPackage(test, package=d, env=test['dependenv'], logfile=logfile)


#def installPackage(package, env={}, root='/', pkgdir='usr/portage/packages',
#                    logfile=None):
def installPackage(test, package=None, env=None, logfile=None):
    # TODO: rewrite docstring
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
    if package is None:
        package = test['package']
    if env is None:
        env = test['emergeenv']
    if logfile is None:
        logfile = pjoin(test['logdir'], 'emerge.log')

    envAdds = env.copy()
    envAdds['PKGDIR'] = test['pkgdir']
    denv = _getEnv(test['root'], envAdds)
    del envAdds

    # Retrieve package string
    pkg = normalize_cpv(package)

    # Execute emerge command and log the results
    bu.mkdir(dirname(logfile))
    fout = file(logfile, 'w')
    cmd = ['emerge', '--ignore-default-opts', '-OB', '=' + pkg]
    p = sp.Popen(cmd, env=denv, stdout=fout, stderr=sp.STDOUT)
    p.wait()

    if p.returncode != 0:
        # In case of error, print the whole emerge command
        raise InstallException(p, ' '.join(cmd), logfile)

    fout.write('\n\n' + 80*'#' + '\n\n')

    # Unpack package onto root
    archive = pjoin(test['pkgdir'], pkg+'.tbz2')
    bu.mkdir(test['root'])
    tarcmd = ['tar', 'xjvf', archive, '-C', test['root']]
    fout.write(' '.join(tarcmd) + '\n' + 80*'-' + '\n')
    p = sp.Popen(tarcmd, stdout=fout, stderr=sp.STDOUT)
    p.wait()
    if p.returncode != 0:
        # In case of error, print the whole emerge command
        raise InstallException(pkg, ' '.join(tarcmd), logfile)

    # Close, return
    fout.close()

if __name__ == '__main__':
    # Just a test
    from pprint import pprint

    pprint(get_dependencies('sci-libs/blas-reference-3.3.1-r1'))
