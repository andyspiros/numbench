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
from os.path import join as pjoin, basename
from shutil import copy as fcopy, copytree
import numpy as np

import benchconfig as cfg
import benchutils as bu
from htmlreport import HTMLreport
from testdescr import testdescr
from benchprint import Print

def saveReport():

    # Check whether pyplot is working
    try:
        plt.figure()
    except:
        Print("Unable to generate plots")
        Print("Please make sure that X is running and $DISPLAY is set")
        return

    # Read configuration
    conf = cfg.mod.reportConf()

    if conf['type'] == 'plot':
        plotf = plt.plot
    elif conf['type'] == 'semilogx':
        plotf = plt.semilogx
    elif conf['type'] == 'semilogy':
        plotf = plt.semilogy
    elif conf['type'] == 'loglog':
        plotf = plt.loglog

    if conf.has_key('xlabel'):
        xlabel = conf['xlabel']
    else:
        xlabel = ''

    if conf.has_key('ylabel'):
        ylabel = conf['ylabel']
    else:
        ylabel = ''


    # Open HTML file
    bu.mkdir(cfg.reportdir)
    htmlfname = pjoin(cfg.reportdir, 'index.html')
    html = HTMLreport(htmlfname)

    for operation in cfg.mod.getTests():
        plt.figure(figsize=(12,9), dpi=300)

        for tid,test in cfg.tests.items():
            if test.has_key('implementations'):
                for impl in test['implementations']:
                    res = test['results'][impl]
                    if res and res.has_key(operation):
                        resultsFile = test['results'][impl][operation]
                        x,y = np.loadtxt(resultsFile, unpack=True)
                        plotf(x, y, label=tid+'/'+impl, hold=True)

        plt.legend(loc='best')
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.grid(True)

        fname = pjoin(cfg.reportdir, operation+'.png')
        plt.savefig(fname, format='png', bbox_inches='tight', transparent=True)
        html.addFig(testdescr[operation], image=basename(fname))

    # Close HTML file
    html.close()
    Print('HTML report generated: ' + htmlfname)

    # Copy input and logs
    inputdest = pjoin(cfg.reportdir, basename(cfg.inputfile))
    fcopy(cfg.inputfile, inputdest)
    copytree(cfg.logdir, pjoin(cfg.reportdir, 'log'))


# Initialize module
try:
    if not locals().has_key('initialized'):
        initialized = True
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        import numpy as np
        with_images = True
except ImportError:
    sys.stderr.write('Error: matplotlib and numpy are needed' + \
      'in order to generate the reports!\n')
    sys.stderr.write('Continue anyway.\n\n')
    with_images = False
