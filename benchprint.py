try:
    needsinitialization = not initialized
except NameError:
    needsinitialization = True


if needsinitialization:
    class _Print:
        def __init__(self, maxlevel=10):
            self._level = 0
            self._maxlevel = maxlevel
        
        def __call__(self, arg):
            if self._level > self._maxlevel:
                return
            if self._level <= 0:
                print str(arg)
                return
            print (self._level-1)*"  " + "-- " + str(arg)
            
        def up(self, n=1):
            self._level = max(self._level-n, 0)
        
        def down(self, n=1):
            self._level = max(self._level+n, 0)
    Print = _Print(3)

initialized = True