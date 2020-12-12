import csv
import numpy as np

def r_csv(f_name):
    with open(f_name, 'r') as f:
        reader = csv.reader(f)
        data = []
        for row in reader:
            data.append([int(row[0],16),int(row[1],16)])
    return data

# 64.20 Ksps(2ch)
f_name = 'source_test.csv'
src = np.array(r_csv(f_name))
means = src.mean()
l = len(src)
src = src - means
src_max = max(src.max(),-src.min())
src = (src/src_max * 32767.0).astype(np.int16)

from scipy.io.wavfile import write
fs = 64200
write('out.wav', fs, src)





