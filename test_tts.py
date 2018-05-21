#!/python

import commands
import os
import datetime
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
        p.close()
        p.join()

def makedirs(file_name):
    os.makedirs('wavdir'+'/'+file_name)
    filepart='./txt'+'/'+file_name
    wavpart=filepart.replace('./txt','wavdir')
    return filepart,wavpart

def init(txt_name):
    return makedirs(txt_name.pop())


if __name__=='__main__':
    start=datetime.datetime.now()
    if os.path.exists('wavdir'):
        os.system('rm -rf wavdir')
    os.makedirs('wavdir')
    txtname=os.listdir('./txt')
    while txtname:
        filepart_1,wavpart_1=init(txtname)
        pid=os.fork()
        if pid == 0:
            filepath(filepart_1,wavpart_1)
        else:
            filepart_2,wavpart_2=init(txtname)
            filepath(filepart_2,wavpart_2)
    end=datetime.datetime.now()
    print('\ntotaltime:%s\n' % (end-start))
    #tts(wavpart+'/'+name[0]+'_0.wav',filepart+'/'+txt,'xiaoyun')
    #tts(wavpart+'/'+name[0]+'_1.wav',filepart+'/'+txt,'xiaogang')