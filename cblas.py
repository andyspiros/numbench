import os, blasbase
import subprocess as sp

class Module(blasbase.ModuleBase):
    @staticmethod
    def get_impls(root):
        output = sp.Popen(
          ['eselect', '--no-color', '--brief', 'cblas', 'list'],
          env={'ROOT' : root}, stdout=sp.PIPE
        ).communicate()[0]
        return output.strip().split('\n')
          
    def _get_flags(self, root, impl, libdir):
        # Retrieve pkgconfig settings and map the directories to the new root
        path = \
          "%s/etc/env.d/alternatives/cblas/%s/%s/pkgconfig" % (root,impl,libdir)
        pkgconf = sp.Popen('pkg-config --libs --cflags cblas', shell=True, \
          stdout=sp.PIPE, env={'PKG_CONFIG_PATH':path}).communicate()[0]
        pkgconf = pkgconf.replace('-L/', '-L'+root+'/')
        pkgconf = pkgconf.replace('-I/', '-I'+root+'/')
        return pkgconf + " -DCBLAS_INTERFACE"

del blasbase
