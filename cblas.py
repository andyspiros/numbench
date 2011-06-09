import os, blasbase
import subprocess as sp

class Module(blasbase.ModuleBase):
    @staticmethod
    def get_impls(root):
        return [i for i in os.listdir(root + "/etc/env.d/alternatives/cblas") \
          if i[0] != '_']
          
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
