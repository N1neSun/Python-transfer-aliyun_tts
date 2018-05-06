#!/python

import commands
import os
import time
from multiprocessing import Pool

def tts(name,text,voice):
    main='./main'
    exist=main+" "+name+" "+text+" "+voice
    os.system(exist)


if __name__=='__main__'
    os.makedirs('wavdir')
    for part in os.listdir('./txt'):
        os.makedirs('wavdir'+'/'+part)
        wavpart='wavdir'+'/'+part
        filepart='./txt'+'/'+part
        for txt in os.listdir(filepart):
            p=Pool(2)
            name=txt.split('.')
            for i in range(2):
                if i == 0:
                    p.apply_async(tts,args=(wavpart+'/'+name[0]+'_0.wav',filepart+'/'+txt,'xiaoyun',))
                else:
                    p.apply_async(tts,args=(wavpart+'/'+name[0]+'_1.wav',filepart+'/'+txt,'xiaogang',))
        print('endtime:%s\n' % time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())))
            #tts(wavpart+'/'+name[0]+'_0.wav',filepart+'/'+txt,'xiaoyun')
            #tts(wavpart+'/'+name[0]+'_1.wav',filepart+'/'+txt,'xiaogang')



