import xml.dom.minidom
import sys, os, portage, types, shlex, subprocess as sp
from os.path import join as pjoin

import benchconfig as cfg
from utils import portageutils as pu

class Parser:
    def __init__(self, fname):
        self._dom = xml.dom.minidom.parse(fname)

    def getModuleName(self):
        opTag = self._dom.getElementsByTagName('operations')[0]
        return opTag.getAttribute('module')

    def getModuleArguments(self):
        opTag = self._dom.getElementsByTagName('operations')[0]
        return shlex.split(opTag.firstChild.data)

    def getTestCases(self):
        testNodes = self._dom.getElementsByTagName('case')

        tests = {}

        for t in testNodes:
            tid = t.getAttribute('id')

            # Get description
            descr = None
            if len(t.getElementsByTagName('descr')) != 0:
                descr = t.getElementsByTagName('descr')[0].firstChild.data

            # Get package
            pkg = portage.catpkgsplit(#@UndefinedVariable
              t.getElementsByTagName('pkg')[0].firstChild.data)
            normPkg = pu.normalize_cpv(pkg)

            # Skip implementations
            skip = []
            skipre = []
            for s in t.getElementsByTagName('skip'):
                if not s.hasAttribute('type') or s.getAttribute('type') == 'glob':
                    skip.append(s.firstChild.data)
                elif s.getAttribute('type') == 'regexp':
                    skipre.append(s.firstChild.data)
                else:
                    sys.stderr.write('Error in configuration file: skip type ' \
                      + s.getAttribute('type') + ' not supported')

            # Requirements
            requires = {}
            for i in t.getElementsByTagName('required'):
                requires[i.getAttribute('name').strip()] = i.firstChild.data.strip()

            # Environments
            dependenv = self._getEnvFromNode(t, 'dependenv')
            emergeenv = self._getEnvFromNode(t, 'emergeenv')
            compileenv = self._getEnvFromNode(t, 'compileenv')
            runenv = self._getEnvFromNode(t, 'runenv')

            # Adjust PATH
            if compileenv.has_key('PATH'):
                compileenv['PATH'] += ':' + os.environ['PATH']
            else:
                compileenv['PATH'] = os.environ['PATH']

            if runenv.has_key('PATH'):
                runenv['PATH'] += ':' + os.environ['PATH']
            else:
                runenv['PATH'] = os.environ['PATH']

            # Build test dictionary
            tests[tid] = dict(
              descr=descr,
              package=pkg,
              normalizedPackage=normPkg,
              skip=skip,
              skipre=skipre,
              requires=requires,

              dependenv=dependenv,
              emergeenv=emergeenv,
              compileenv=compileenv,
              runenv=runenv,

              pkgdir=pjoin(cfg.pkgsdir, tid),
              archive=pjoin(cfg.pkgsdir, tid, normPkg + '.tbz2'),
              root=pjoin(cfg.rootsdir, tid),
              testdir=pjoin(cfg.testsdir, tid),
              logdir=pjoin(cfg.logdir, tid)
            )

        return tests

    @staticmethod
    def _readFile(fs):
        result = {}

        # If fs is a filename, open it
        if type(fs) != types.FileType:
            fs = file(pjoin(cfg.curdir, fs))

        # Read line by line
        for l in fs.readlines():
            try:
                k, v = l.split('=', 1)
                result[k.strip()] = v.strip()
            except:
                pass

        return result

    @classmethod
    def _readScript(cls, fname):
        fname = pjoin(cfg.curdir, fname)

        # Execute script with void environment
        proc = sp.Popen('. ' + fname + ' &> /dev/null; env', shell=True,
          stdout=sp.PIPE, env={})
        result = cls._readFile(proc.stdout)

        # Remove useless variables
        for k in ('SHLVL', 'PWD', '_'):
            if result.has_key(k):
                del result[k]
        return result

    @classmethod
    def _getEnvFromNode(cls, node, envName):
        envs = node.getElementsByTagName(envName)

        # Check number of envs
        if len(envs) > 1:
            errstr = "Error: no more than one " + envName + " element is allowed!"
            raise Exception(errstr)
        elif len(envs) < 1:
            return {}

        e = envs[0]

        # Check attribute "append"
        if (e.attributes.has_key('append')):
            append = e.attributes['append'].value == '1'
        else:
            append = False

        if append:
            env = os.environ
        else:
            env = {}

        # Check attribute script
        # the script is run with a void environment
        if (e.attributes.has_key('script')):
            for k, v in cls._readScript(e.getAttribute('script')).items():
                env[k] = v

        # Check attribute file
        # the file must contain lines with key=value pairs (each line one pair)
        if (e.attributes.has_key('file')):
            for k, v in cls._readFile(e.getAttribute('file')).items():
                env[k] = v

        # Get Variables
        for v in e.getElementsByTagName('var'):
            envname = v.getAttribute('name')
            envvalue = v.firstChild.data
            env[envname] = envvalue

        return env
