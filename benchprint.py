try:
    needsinitialization = not initialized
except NameError:
    needsinitialization = True


if needsinitialization:
    import benchconfig as cfg, benchutils as bu
    from os.path import dirname, join as pjoin

    class _Print:
        def __init__(self, logfile, maxlevel=10):
            self._level = 0
            self._maxlevel = maxlevel
            self._logfile = logfile
        
        def __call__(self, arg='\n'):
            if self._level > self._maxlevel:
                return
                
            if self._level <= 0:
                print str(arg)
                bu.mkdir(dirname(self._logfile))
                logfile = file(self._logfile, 'a')
                print >> logfile, str(arg)
                logfile.close()
                return
                
            printstr = (self._level-1)*"  " + "-- " + str(arg)
            if self._level <= self._maxlevel-1:
                bu.mkdir(dirname(self._logfile))
                logfile = file(self._logfile, 'a')
                print >> logfile, printstr
                logfile.close()
            print printstr
            
        def up(self, n=1):
            self._level = max(self._level-n, 0)
        
        def down(self, n=1):
            self._level = max(self._level+n, 0)
    Print = _Print(pjoin(cfg.logdir, 'main.log'), 3)

initialized = True
