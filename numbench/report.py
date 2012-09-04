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
import sys, numpy as np
from shutil import copy as fcopy, copytree, rmtree

import benchconfig as cfg
from utils import benchutils as bu
from reports.html import ReportFile as HTMLreport
from testdescr import testdescr
from benchprint import Print


class Plotter:
    def __init__(self, conf):
        # Store configuration
        self.conf = conf

        # Labels
        self.xlabel = conf.get('xlabel', '')
        self.ylabel = conf.get('ylabel', '')

        # Initialize markers
        markers = ('-', '--', 'v', '^', 'o', 's', 'p', 'h', '*', '+', 'x', 'D')
        colors = ('k', 'r', 'g', 'b', 'c')
        self.linestyles = tuple([c + m for m in markers for c in colors])
        self.curstyle = 0

        # Open figure
        plt.figure(figsize=(12, 9), dpi=300)


    def addPlot(self, X, label):
        style = self.linestyles[self.curstyle]
        self.curstyle = (self.curstyle + 1) % len(self.linestyles)
        if X.shape[1] == 2:
            plt.plot(X[:, 0], X[:, 1], style, label=label, hold=True)
        elif X.shape[1] == 3:
            x, y, e = X[:, 0], X[:, 1], X[:, 2]
            plt.errorbar(x, y, e, fmt=style, label=label, hold=True)

    def savePlot(self, fname):
        axes = plt.axes()
        axes.set_xscale(self.conf.get('xscale', 'linear'))
        axes.set_yscale(self.conf.get('yscale', 'linear'))
        plt.legend(loc='best', prop={'size':'x-small'})
        plt.xlabel(self.xlabel)
        plt.ylabel(self.ylabel)
        plt.grid(True)
        plt.savefig(fname, format=cfg.imageformat, \
                    bbox_inches='tight', transparent=True)



def saveReport():
    # Check whether pyplot is working
    try:
        plt.figure()
    except:
        Print("Unable to generate plots")
        Print("Please make sure that X is running and $DISPLAY is set")
        return

    # Regenerate report in any case
    rmtree(cfg.reportdir, ignore_errors=True)

    # Open HTML file
    bu.mkdir(cfg.reportdir)
    bu.mkdir(pjoin(cfg.reportdir, 'images'))
    htmlfname = pjoin(cfg.reportdir, 'index.html')
    html = HTMLreport(htmlfname)

    for operation in cfg.module.getTests():

        # Begin plot
        p = Plotter(cfg.module.reportConf())

        for tid, test in cfg.tests.items():

            longlabel = len(test.get('implementations', [])) > 1
            for impl in test.get('implementations', []):

                # Add line to the plot
                implres = test['results'][impl]
                if implres and implres.has_key(operation):
                    resultsFile = implres[operation]
                    X = np.loadtxt(resultsFile, unpack=False)
                    label = tid + '/' + impl if longlabel else tid
                    p.addPlot(X, label)

        imgpath = pjoin('images', operation + '.' + cfg.imageformat)
        fname = pjoin(cfg.reportdir, imgpath)
        p.savePlot(fname)
        html.addFig(testdescr[operation], image=imgpath)

    # Copy logs and input file
    copytree(cfg.logdir, pjoin(cfg.reportdir, 'log'))
    fcopy(cfg.inputfile, pjoin(cfg.reportdir, basename(cfg.inputfile)));

    # Close HTML file
    html.close()
    Print('HTML report generated: ' + htmlfname)


# Initialize module
# Import matplotlib and use 'Agg' as backend
try:
    if 'initialized' not in locals():
        initialized = True
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        with_images = True
except ImportError:
    sys.stderr.write('Error: matplotlib and numpy are needed' + \
      'in order to generate the reports!\n')
    sys.stderr.write('Continue anyway.\n\n')
    with_images = False
