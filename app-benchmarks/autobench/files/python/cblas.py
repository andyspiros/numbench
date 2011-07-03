import blasbase

class Module(blasbase.BLASBase):
    def _initialize(self):
        self.libname = "cblas"
        blasbase.BLASBase._initialize(self)