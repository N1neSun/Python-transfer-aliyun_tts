#!/python

import commands
import os
import datetime
import shutil
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

def init(txt_name):
    file_1=txt_name.pop()
    file_2=txt_name.pop()
    os.makedirs('wavdir'+'/'+file_1)
    os.makedirs('wavdir'+'/'+file_2)
    filepart_1='./txt'+'/'+file_1
    filepart_2='./txt'+'/'+file_2
    wavpart_1=filepart_1.replace('./txt','wavdir')
    wavpart_2=filepart_2.replace('./txt','wavdir')
    return filepart_1,filepart_2,wavpart_1,wavpart_2


if __name__=='__main__':
    start=datetime.datetime.now()
    if os.path.exists('wavdir'):
        os.system('rm -rf wavdir')
    os.makedirs('wavdir')
    txtname=os.listdir('./txt')
    while txtname:
        filepart_1,filepart_2,wavpart_1,wavpart_2=init(txtname)
        pid=os.fork()
        if pid == 0:
            filepath(filepart_1,wavpart_1)
        else:
            filepath(filepart_2,wavpart_2)
    end=datetime.datetime.now()
    print(end-start)
            #tts(wavpart+'/'+name[0]+'_0.wav',filepart+'/'+txt,'xiaoyun')
            #tts(wavpart+'/'+name[0]+'_1.wav',filepart+'/'+txt,'xiaogang')

