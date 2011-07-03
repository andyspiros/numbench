import blasbase

class Module(blasbase.BLASBase):
    def _initialize(self):
        self.libname = "blas"
        blasbase.BLASBase._initialize(self)