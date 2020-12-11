from get_value import MCP3204
import numpy as np
import time
sec = 10
adc = MCP3204()
rec = np.zeros((2,4000*sec))
st = time.time() 
for i in range(4000*sec):
    vs = adc.get_value()[0:2]
    rec[0][i] = vs[0] 
    rec[1][i] = vs[1] 
t = time.time()-st
np.save('recorded1.npy',rec)
print(rec.max(),rec.min())
print(4000*sec/t,'sps')

