#!/python

import commands
import os
import time
from multiprocessing import Pool

def tts(name,text,voice):
    main='./main'
    exist=main+" "+name+" "+text+" "+voice
    os.system(exist)

def filepath(txtpath,wavpath):
    for txt in os.listdir(txtpath):
        name=txt.split('.')
        p=Pool(2)
        for i in range(2):
            if i == 0:
                p.apply_async(tts,args=(wavpath+'/'+name[0]+'_0.wav',txtpath+'/'+txt,'xiaoyun',))
            else:
                p.apply_async(tts,args=(wavpath+'/'+name[0]+'_1.wav',txtpath+'/'+txt,'xiaogang',))
        p.colse()
        p.join()


if __name__=='__main__':
    os.makedirs('wavdir')
    txtname=os.listdir('./txt')
    while txtname:
        file1=txtname.pop()
        file2=txtname.pop()
        os.makedirs('wavdir'+'/'+file1)
        os.makedirs('wavdir'+'/'+file2)
        wavpart1='wavdir'+'/'+file1
        wavpart2='wavdir'+'/'+file2
        filepart1='./txt'+'/'+file1
        filepart2='./txt'+'/'+file2
        pid=os.fork()
        if pid == 0:
            filepath(filepart1,wavpart1)
        else:
            filepath(filepart2,wavpart2)
    print('endtime:%s\n' % time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())))
            #tts(wavpart+'/'+name[0]+'_0.wav',filepart+'/'+txt,'xiaoyun')
            #tts(wavpart+'/'+name[0]+'_1.wav',filepart+'/'+txt,'xiaogang')

