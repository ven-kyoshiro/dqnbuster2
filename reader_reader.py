import numpy as np
import time
import mmap

#Map.tell() 今いる位置

share_File = open('share.dat', 'r')

share_No = share_File.fileno()
#ここのlengthは重要で、ファイルサイズの値と一致させないといけない
length = n = 8
share_Map = mmap.mmap(share_No,length,access=mmap.ACCESS_READ) 
while True:
    #share_Map.seek(0) # 読み始める位置の指定
    #params = share_Map.read(n) # n個読む
    print(share_Map[0:7],share_Map.tell())
    time.sleep(0.1)
share_File.close()
