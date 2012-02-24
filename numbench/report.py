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
from shutil import rmtree

import benchconfig as cfg
import benchutils as bu
from htmlreport import HTMLreport
from testdescr import testdescr
from benchprint import Print


class Plotter:
    def __init__(self, conf):
        # Set plot function
        self.conf = conf
        if not conf.has_key('type'):
            self.plotf = plt.plot
        elif type(conf['type']) == type(''):
            try:
                self.plotf = plt.__dict__[conf['type']]
            except:
                print >> sys.stderr, \
                  'Plot function "', conf['type'], '" not found. Using plot'
                self.plotf = plt.plot
                return
        else:
            self.plot = conf['type']
        
        # Labels
        self.xlabel = conf.has_key('xlabel') and conf['xlabel'] or ''
        self.ylabel = conf.has_key('ylabel') and conf['ylabel'] or ''
        
        # Initialize markers
        markers = ('-', '--', 'o', 'v', '^', 's', 'p', 'h', '*', '+', 'x', 'D')
        colors = ('k', 'r', 'g', 'b')
        self.linestyles = tuple([c+m for m in markers for c in colors])
        self.curstyle = 0
        
        # Open figure
        plt.figure(figsize=(12,9), dpi=300)
    
    
    def addPlot(self, x, y, label):
        style = self.linestyles[self.curstyle]
        self.curstyle = (self.curstyle+1) % len(self.linestyles)
        self.plotf(x, y, style, label=label, hold=True)
    
    def savePlot(self, fname):
        plt.legend(loc='best')
        plt.xlabel(self.xlabel)
        plt.ylabel(self.ylabel)
        plt.grid(True)
        plt.savefig(fname, format='png', bbox_inches='tight', transparent=True)
        


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

    for operation in cfg.mod.getTests():
        
        # Begin plot
        p = Plotter(cfg.mod.reportConf())
        
        for tid,test in cfg.tests.items():
            if test.has_key('implementations'):
                for impl in test['implementations']:
                    if test['results'][impl].has_key(operation):
                        resultsFile = test['results'][impl][operation]
                        x,y = np.loadtxt(resultsFile, unpack=True)
                        p.addPlot(x, y, tid+'/'+impl)

        imgpath = pjoin('images', operation+'.png')
        fname = pjoin(cfg.reportdir, imgpath)
        p.savePlot(fname)
        html.addFig(testdescr[operation], image=imgpath)

    # Close HTML file
    html.close()
    Print('HTML report generated: ' + htmlfname)


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
