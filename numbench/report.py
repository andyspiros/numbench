from os.path import join as pjoin, basename
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
                    if test['results'][impl].has_key(operation):
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
