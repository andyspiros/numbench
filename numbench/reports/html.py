#=====================================================
# Copyright (C) 2011-2012 Andrea Arteaga <andyspiros@gmail.com>
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
import os, time
from os.path import join as pjoin, basename, exists
from xml.sax.saxutils import escape as xmlescape

from .. import benchconfig as cfg

class ReportFile:
    def __init__(self, fname, title='Benchmarks report', inputfile=None):
        if inputfile is None:
            inputfile = pjoin(cfg.reportdir, basename(cfg.inputfile))

        self.fname = fname
        self.content = """
<html>
<head>
<title>""" + title + """</title>
<style type="text/css">
body {
  background-color: #FFFFFF;
}

img {
  height:80%;
}

pre {
  background-color: #EEE;
}

h1, h2, .plot, .descr, .info {
  text-align: center;
}

.inputfile {
  margin-bottom: 40px;
}

.fig {
   margin-bottom: 50px;
   padding-top: 20px;
   padding-bottom: 20px;
}
</style>
</head>
<body>
<h1>""" + title + "</h1>"
        date = time.strftime('%Y-%m-%d, %I:%M %p')
        self.content += '<p class="info">Generated on ' + date + '</p>'

        # Information regarding the CPU
        cpuinfo = file('/proc/cpuinfo', 'r').readlines()
        cpu = None
        corecount = 0
        for l in cpuinfo:
            if l.startswith('model name'):
                cpu = l.split(':', 1)[1].strip()
            if l.startswith('processor'):
                corecount += 1

        if cpu:
            self.content += '<p class="info">CPU: ' + cpu + \
              ' (' + str(corecount) + ' cores)</p>'

        # Information regarding the memory
        meminfo = file('/proc/meminfo', 'r').readlines()
        mem = None
        for l in meminfo:
            if l[:8] == 'MemTotal':
                mem = l.split(':', 1)[1].strip()
        if mem:
            self.content += '<p class="info">Total memory: ' + mem + '</p>'

        # Information regarding the caches
        cachedir = '/sys/devices/system/cpu/cpu0/cache'
        if exists(cachedir):
            self.content += '<p class="info">Caches:<br />'
            for i in os.listdir(cachedir):
                cdir = pjoin(cachedir, i)
                ctxt = 'L' + file(pjoin(cdir, 'level')).read().strip()
                ctxt += ' ' + file(pjoin(cdir, 'type')).read().strip()
                ctxt += ': ' + file(pjoin(cdir, 'size')).read().strip()[:-1]
                self.content += ctxt + ' kB<br />'
            self.content += '</p>'


        # Input file
        self.content += '<div class="inputfile">Input file: ' + \
          '<a href="%s">%s</a>' % (basename(inputfile), cfg.inputfile) + \
          '<pre>%s</pre></div>' % xmlescape(file(cfg.inputfile, 'r').read())

        self.content += '<div class="log">Logs: <a href="log">%s</a></div>' \
          % cfg.logdir


    def addFig(self, title, image, descr='', alt='', width=None):
        self.content += '<div class="fig">'
        self.content += '<h2>' + title + '</h2>'
        if descr.strip() != '':
            self.content += '<p class="descr">' + descr + '</p>'
        self.content += '<div class="plot">'
        self.content += '<a href="' + image + '">'
        self.content += '<img src="' + image + '" alt="' + alt + '"'
        if width is not None:
            self.content += ' style="width:' + str(width) + '; height:auto"'
        self.content += ' /></a>'
        self.content += '</div>'
        self.content += '</div><hr />'

    def close(self):
        self.content += '</body></html>'
        f = file(self.fname, 'w')
        f.write(self.content)
        f.close()
        self.content = ''
