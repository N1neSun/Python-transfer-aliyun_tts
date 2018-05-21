// created by zxn
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "pthread.h"
#include "NlsClient.h"	
#include <stdlib.h>
#include <string.h>
#include <vector>
#ifdef _WINDOWS_COMPILE_
#include <windows.h>
#else
#include <unistd.h>
#endif
using namespace std;

NlsSpeechCallback* gCallback = NULL;
NlsClient* gNlc = NULL;
vector<unsigned char> databuf;
struct ParamStruct
{
	std::string filename;
	std::string config;
	int threadcount;
	int count;
	std::string id;
	std::string key;
	bool isSendData;
	std::string txtname;
	std::string voice;
};
void* func(void* arg)
{
	try
	{
		ParamStruct* tst = (ParamStruct*)arg;
		if (tst == NULL)
		{
			std::cout << "filename is not valid" << endl;
			return 0;
		}
		/*******************第一种调用方式***************************/
		//创建一句话识别  NlsRequest，第一个参数为gCallback对象，第二个参数为config.txt文件
		NlsRequest* request = gNlc->createTtsRequest(gCallback, tst->config.c_str());					
		/***********************************************************/

		/*******************第二种调用方式**************************/
		//NlsRequest* request = nlc.createTtsRequest(gCallback, NULL);
		ifstream text(tst->txtname);
		if(!text)cout<<"error"<<endl;
		ostringstream buf;
		char ch;
		while(buf&&text.get(ch)){
			buf.put(ch);
		}
		text.close();
		request->SetParam("AppKey","nls-service-en-opus");
		request->SetParam("TtsEnable","true");
		request->SetParam("TtsEncodeType","wav");
		request->SetParam("TtsVolume","50");
		request->SetParam("Url","wss://nls.dataapi.aliyun.com:443");
		request->SetParam("Id","1234567");
		request->SetParam("TtsNus","1");
		request->SetParam("TtsVoice",tst->voice.c_str());
		request->SetParam("TtsSpeechRate","-200");
		request->SetParam("BackgroundMusicId","1");
		request->SetParam("BackgroundMusicOffset","0");
		request->SetParam("BstreamAttached","false");
		request->SetParam("TtsReq",buf.str().c_str());
		/******************/
		if (request == NULL)
		{
			std::cout << "createTtsRequest fail" << endl;
			return 0;
		}
		request->SetParam("Id", "1234567");
		request->Authorize(tst->id.c_str(), tst->key.c_str());
		if (request->Start() < 0)
		{
			std::cout << "start fail" << endl;
			delete request;
			return 0;
		}
		request->Stop();
		delete request;
		request = NULL;

		if (databuf.size() > 0)
		{
			ofstream fss;
			fss.open(tst->filename.c_str(), ios::binary | ios::out);
			fss.write((char*)&databuf[0], databuf.size());
			fss.close();
		}
	}
	catch (const char* e)
	{
		std::cout << e << endl;
	}
	return 0;
}


void OnResultDataRecved(NlsEvent* str,void* para=NULL)
{
	ParamStruct* tst = (ParamStruct*)para;
	std::cout << "Id:" << (*(int*)para) << " " << str->getResponse() << endl;
}

void OnOperationFailed(NlsEvent* str, void* para=NULL)
{
	ParamStruct* tst = (ParamStruct*)para;
	std::cout << "Id:" << (*(int*)para) << " " << str->getErrorMessage() << endl;
}
void OnChannelCloseed(NlsEvent* str, void* para=NULL)
{
	ParamStruct* tst = (ParamStruct*)para; 
	std::cout << "Id:" << (*(int*)para) << " " << str->getResponse() << endl;
}
void OnBinaryDataRecved(NlsEvent* str, void* para=NULL)
{
	vector<unsigned char> data = str->getBinaryData();
	databuf.insert(databuf.end(), data.begin(), data.end());
}

int main(int arc, char* argv[])
{
	try
	{
		/*if (arc <= 7)
		{
			cout << "param is not valid. Usage: demo test.wav config.txt 1 1 [your-id] [your-scret] 0" << endl;
			return -1;
		}*/
		gNlc = NlsClient::getInstance();
		int ret = gNlc->setLogConfig("log-tts.txt",3);
		int id = 123434;
		gCallback = new NlsSpeechCallback();
		gCallback->setOnMessageReceiced(OnResultDataRecved, &id);
		gCallback->setOnOperationFailed(OnOperationFailed, &id);
		gCallback->setOnChannelClosed(OnChannelCloseed, &id);
		gCallback->setOnBinaryDataReceived(OnBinaryDataRecved, &id);
		ParamStruct pa;
		pa.filename = argv[1];
		pa.config = "config-tts.txt";
		pa.threadcount = 1;
		pa.count = 1;
		pa.id = "your-id";
		pa.key = "your-key";
		pa.isSendData =  true;
		pa.txtname=argv[2];
		pa.voice=argv[3];
		const int ThreadCount = pa.threadcount;
		vector<pthread_t> tarr(ThreadCount);
		for (int i = 0; i < pa.count; i++)
		{
			for (int j = 0; j < ThreadCount; j++)
				pthread_create(&tarr[j], NULL, &func, (void*)&pa);
			for (int j = 0; j < ThreadCount; j++)
				pthread_join(tarr[j], NULL);
		}
		if(gNlc != NULL)
		{
			delete gNlc;
			gNlc = NULL;
		}
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
	return 0;
}
