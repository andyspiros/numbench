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
import subprocess as sp
import benchpkgconfig as pc
import shlex

def getImplementations(root, libname):
    cmd = ['eselect', '--no-color', '--brief', libname, 'list']
    env = dict(ROOT=root)
    output = sp.Popen(cmd, env=env, stdout=sp.PIPE).communicate()[0].strip()
    if '(none found)' in output:
        return []
    else:
        return [i.split()[0] for i in output.split('\n')]
    
    
def getFlags(test, libname, impl):
    root = test['root']
    
    # 1. Run without requires
    pfile = pc.getFile(libname, impl, root)
    flags = pc.run(pfile, root, False)
    
    # TODO: add log
#    logfile = file(pjoin(self.logdir, 'pkg-config.log'), 'w')
#    print >> logfile, "File:", pfile
#    print >> logfile, "Result:", flags

    # 2. Get requires
    requires = pc.requires(pfile)
    # TODO: add log     print >> logfile, "Requires:", requires
    
    # 3. Substitute requires and add flags
    if test.has_key('requires'):
        for r in requires:
            if r in test['requires'].keys():
                pfile = pc.getFile(r, test['requires'][r])
                flags += ' ' + pc.run(pfile)
            else:
                flags += ' ' + pc.run(r)
    # TODO: add log
#    print >> logfile, "Third run:", flags
#    logfile.close()

    return shlex.split(flags)