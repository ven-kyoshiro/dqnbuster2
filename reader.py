from get_value_dummy import MCPDUMMY
import numpy as np
import time
import mmap
adc = MCPDUMMY()

share_File = open('share.dat', 'r+')

share_No = share_File.fileno()
#ここのlengthは重要で、ファイルサイズの値と一致させないといけない
length = 8
share_Map = mmap.mmap(share_No,length,access=mmap.ACCESS_WRITE) 
share_Map.seek(0)
write_str = "01234567".encode()
print(write_str) #書き込む文字列長をコンソール上に表示して確認してみる
share_Map.write(write_str)
print('ok')




while True:
    vs = adc.get_value()[0:2]
    write_str = (str(vs[0]).zfill(4) + str(vs[1]).zfill(4)).encode()
    print(write_str)
    #share_Map.seek(0)
    #share_Map.write(write_str)
    share_Map[0:8] = write_str
    time.sleep(1)

share_File.close()
