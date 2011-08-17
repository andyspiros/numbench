import os, shlex, numpy as np, subprocess as sp
from os.path import realpath, exists as pexists, join as pjoin
from random import randint

import basemodule
import benchconfig as cfg
from benchutils import mkdir
from benchprint import Print
import benchchilds

inputsdir = pjoin(cfg.testsdir, 'metis-input')
mkdir(inputsdir)

avail_graph = ['pmetis-8', 'kmetis-8', 'pmetis-64', 'kmetis-64']
avail_mesh = []
avail_matrix = []

class Module(basemodule.BaseModule):
    
    #classmethod
    def _initialize(cls):
        cls.avail = avail_graph + avail_mesh + avail_matrix
    
    def _parse_args(self, args):
        tests = []
        
        # Parse arguments
        for i in args:
            if i in self.avail:
                tests.append(i)
            else:
                raise Exception("Argument not recognized: " + i)
                
        if len(tests) == 0:
            # If not test provided, perform all
            self.tests = self.avail
        else:
            # Sort tests
            self.tests = [i for i in self.avail if i in tests]
    
    @staticmethod
    def get_impls(*args, **kwargs):
        return ('metis',)
        
    def instructionsFor(self, impl):
        Print("Nothing to do")
    
    def save_results(self, results):
        basemodule.BaseModule.save_results(self, results, 'loglog', 'Seconds')
    
    def getTest(self, root, impl, testdir, logdir):
        t = MetisTest(root, testdir, logdir, self.tests)
        return t
    

class MetisTest:
    sizes = None
    
    def __init__(self, root, testdir, logdir, tests):
        self.root = root
        self.testdir = testdir
        self.logdir = logdir
        self.tests = tests
        mkdir(logdir)
        mkdir(testdir)
        
    @classmethod
    def _getSizes(cls):
        if cls.sizes is None:
            cls.sizes = [int(i) for i in np.logspace(3, 6, 100)]
        return cls.sizes
    
    def _generateFiles(self):
        Print("Generating input files...")
        # Graph
        if len([i for i in self.tests if i in avail_graph]) != 0:
            for size in self._getSizes():
                fname = pjoin(inputsdir, 'input_%i.graph' % size)
                if not pexists(fname):
                    writeGraph(size, fname)
        Print("Done")
                    
        
    def run_test(self, changes={}):
        self._generateFiles()
        result = {}
        
        for t in [i for i in self.tests if i in avail_graph]:
            Print('Doing ' + t)
            Print.down()
            res = []
            
            for s,size in enumerate(self._getSizes(), 1):
                inputfile = pjoin(inputsdir, 'input_%i.graph' % size)
                
                # Setting environment
                env = os.environ.copy()
                envpath = env.has_key('PATH') and env['PATH'] or ''
                env['PATH'] = ':'.join([pjoin(self.root, p) for p in \
                  ('bin', 'usr/bin')]) + ':' + envpath
                envlib = env.has_key('LD_LIBARY_PATH') and \
                  env['LD_LIBARY_PATH'] or ''
                env['LD_LIBRARY_PATH'] = ':'.join([pjoin(self.root,p) for p in \
                  ('usr/lib', 'usr/lib64', 'usr/lib32')]) + ':' + envlib
                  
                # Get executable
                exe, parts = t.split('-')
                exe = pjoin(self.root, 'usr/bin', exe)
                
                # Check dynamic linking
                logf = file(pjoin(self.logdir, 'ldd.log'), 'w')
                p = sp.Popen(\
                  ['ldd', '-v', exe], stdout=logf, stderr=sp.STDOUT, env=env
                )
                p.wait()
                
                # Execute
                logname = pjoin(self.logdir, t + '_%i.log' % size)
                cmd = [exe, inputfile, parts]
                pr = sp.Popen(cmd, stdout=sp.PIPE, stderr=sp.STDOUT, env=env)
                benchchilds.append(pr)
                lines = pr.communicate()[0].split('\n')
                
                # Interpret output
                for i,line in enumerate(lines):
                    if line[:18] == "Timing Information":
                        begin_timing = i+1
                        break
                    
                lines = [i[:-1] for i in lines[begin_timing+1:]]
                for l in lines:
                    if l.strip()[:13] == "Partitioning:":
                        time = float(shlex.split(l)[1])
                        break
                res.append((size,time))
                Print("size: %6i    %2.3f sec.  (%i/%i)" % (size, time, s, 100))
            
            Print.up()
            
            # Write sizes / times to result file
            resfname = pjoin(self.testdir, t+'.dat')
            resfs = file(resfname, 'w')
            for i in res:
                print >> resfs, i[0], i[1]
            resfs.close()
            result[t] = resfname
        return result
            
            
def writeGraph(size, filename):
    edges = [[] for i in xrange(size)]
    nedges = 0
    for e1 in xrange(len(edges)):
        n = 0
        tot = randint(1, min(size / 4, 5))
        while n < tot:
            e2 = randint(0, size - 1)
            if e2 not in edges[e1] and e1 != e2:
                edges[e1].append(e2)
                edges[e2].append(e1)
                n += 1
                nedges += 1
    fs = file(filename, 'w')
    print >> fs, size, nedges
    for s in edges:
        print >> fs, ' '.join([str(i+1) for i in s])
    fs.close()
