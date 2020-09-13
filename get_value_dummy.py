import sys
import time
import numpy as np

class MCPDUMMY(object):
    def __init__(self):
        pass
    def get_value(self):
        # values should be [0,4096]
        time.sleep(1/8500)
        return list(np.random.randint(0,4096+1,4))

