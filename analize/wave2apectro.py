#coding:utf-8

import scipy.io.wavfile as wio
import matplotlib.pyplot as plt
import numpy as np


if __name__ == '__main__':
    rate, data = wio.read("out.wav")
    fig,ax = plt.subplots(2,1,sharex=True,figsize=(16,9))
    pxx, freqs, bins, t = ax[0].specgram(data[:,0],Fs = rate)
    ax[0].pcolormesh(bins, freqs, 10*np.log10(pxx),cmap='magma')
    ax[0].set_yscale('symlog', linthreshy=0.01)
    pxx, freqs, bins, t = ax[1].specgram(data[:,1],Fs = rate)
    ax[1].pcolormesh(bins, freqs, 10*np.log10(pxx),cmap='magma')
    ax[1].set_yscale('symlog', linthreshy=0.01)
    ax[1].set_xlabel('sec')
    ax[0].set_ylabel('Hz')
    ax[1].set_ylabel('Hz')
    plt.savefig('out.png',dpi=300)
    #plt.figure()
    #pxx, freq, bins, t = plt.specgram(data[:,1],Fs = rate)
    #plt.savefig('out_ch1.png',dpi=300)
