import commands as cmd
import portage
import os

class InstallException(Exception):
    def __init__(self, command):
        self.command = command

def available_packages(pattern):
    """Returns a list of packages matching the given pattern.
    
    The packages are returned as (category, package, version, revision) tuple.
    No test for keywords or mask is performed. The function just returns
    every matching pattern in the portage tree and installed overlays.
    """
    return [portage.catpkgsplit(l) \
      for l in cmd.getoutput('equery -q list -po ' + pattern).split()]


def install_package(package, env={}, root='/', pkgdir='usr/portage/packages'):
    """Emerge a package in the given root.
    
    package is the package to be emerged. It has to be a tuple
    (category, package, version, revision).
    
    env is a dictionary of KEY:VALUE pairs describing the environment changes
    the package will be emerged with. It is useful to specifiy the compilers and
    compiler flags. It is safe to use ACCEPT_KEYWORDS=* for testing purposes.
    
    root is the directory where the packaged will be emerged. A non-root user
    can also use this function, provided he has write access to that directory.
    
    pkgdir is the directory where the binary package will be placed. The user
    has to be able to write in this directory.
    
    The function has no return value and raises an exception in case of building
    or emerging failure. Note: dependencies will NOT be emerged!
    """  
    
    # Retrieve package string
    if package[-1] != 'r0':
        pkg = '%s/%s-%s-%s' % package
    else:
        pkg = '%s/%s-%s' % package[:-1]
    
    # Setup command line
    env['PKGDIR'] = pkgdir
    envl = ""
    for i in env:
        envl += i + '="' + env[i] + '" '
    cl = envl + 'emerge --ignore-default-opts -OB "=' + pkg + '"'
    
    # Execute emerge command
    so = cmd.getstatusoutput(cl)
    if so[0] != 0:
        # In case of error, print the whole emerge command
        raise InstallException(cl)
    
    # Unpack the archive onto the given root directory
    archive = pkgdir + pkg + '.tbz2'
    os.path.exists(root) or os.makedirs(root)
    so = cmd.getstatusoutput("tar xjf " + archive + " -C " + root);
    if so[0] != 0:
        raise InstallException(so[1])
    
if __name__ == '__main__':
    # Just a test
    print available_packages('blas-reference')