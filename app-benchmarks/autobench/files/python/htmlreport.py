import time

class HTMLreport:
    def __init__(self, fname, title="Benchmarks report"):
        self.fname = fname
        self.content = """
<html>
<head>
<title>Benchmarks report</title>
<style type="text/css">
body {
  background-color: #FFFFFF;
}

img {
  height:80%;
}

h1, h2, .plot, .descr, .info {
  text-align: center;
}

.fig {
   margin-bottom: 50px;
   padding-top: 20px;
   padding-bottom: 20px;
}
</style>
</head>
<body>
<h1>
"""
        self.content += title + "</h1>"
        date = time.strftime('%Y-%m-%d, %I:%M %p')
        self.content += '<p class="info">Generated on ' + date + '</p>'
        
        cpuinfo = file('/proc/cpuinfo', 'r').readlines()
        cpu = None
        for l in cpuinfo:
            if l[:10] == 'model name':
                cpu = l.split(':',1)[1].strip()
        if cpu:
            self.content += '<p class="info">CPU: ' + cpu + '</p>'
        
        meminfo = file('/proc/meminfo', 'r').readlines()
        mem = None
        for l in meminfo:
            if l[:8] == 'MemTotal':
                mem = l.split(':',1)[1].strip()
        if mem:
            self.content += '<p class="info">Total memory: ' + mem + '</p>'
        
        
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
        self.content += "</body></html>"
        f = file(self.fname, 'w')
        f.write(self.content)
        f.close()
        self.content = ''
