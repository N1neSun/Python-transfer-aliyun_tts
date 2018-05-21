# Python-transfer-aliyun_tts
python transfer c++ executable file to achieve tts(Text To Speech)

How to use it?<br>
1.the nlsSpeech-release is the file of tts api(c++)<br>
2.you can go to this site to learn more https://help.aliyun.com/document_detail/48717.html?spm=a2c4g.11186623.6.573.vjdVR0<br>
3.I modified this program(nlsSpeech-release/demo/tts-demo.cpp) to suit my needs<br>
4.you should use your own id and key in tts-demo.cpp<br>
5.environment is ubuntu and python 2.7.15<br>
6.use `g++ -o main tts-demo.cpp -pthread -I ./ -L. -lrealTimeUnity -lopus -lssl -lcrypto -ldl` to Compiled<br>
7.make sure the file of main and test_tts.py are in the same folder<br>
8.then execute "python test_tts.py",program will read txt file's data and convert to audio(format is wav)<br>
9.program will execute in multiprocess<br>