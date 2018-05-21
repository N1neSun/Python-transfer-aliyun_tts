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
#ifdef _WINDOWS_COMPILE_
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;
#define FRAME_SIZE 640

NlsSpeechCallback* gCallback = NULL;
NlsClient* gNlc = NULL;

struct ParamStruct
{
	std::string filename;
	std::string config;
	int threadcount;
	int count;
	std::string id;
	std::string key;
	bool isSendData;
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
		ifstream fs;
		fs.open(tst->filename.c_str(), ios::binary | ios::in);
		if (!fs)
		{
			throw ("file isn't exist");
		}
		/*******************第一种调用方???**************************/
		NlsRequest* request = gNlc->createAsrRequest(gCallback, tst->config.c_str());					//创建一句话识别  NlsRequest，第一个参数为callback对象，第二个参数为config.txt文件
		/***********************************************************/

		/*******************第二种调用方式*************************
		  NlsRequest* request = nlc.createAsrRequest(gCallback, NULL);
		  request->SetParam("AppKey","nls-service");
		  request->SetParam("AsrInEnable","true");
		  request->SetParam("ResponseMode","normal");
		  request->SetParam("Format","pcm");
		  request->SetParam("Url","wss://nls.dataapi.aliyun.com:443");
		 ******************/
		if (request == NULL)
		{
			std::cout << "createAsrRequest fail" << endl;
			return 0;
		}
		request->Authorize(tst->id.c_str(), tst->key.c_str());
		if (request->Start() < 0)
		{
			std::cout << "start fail" << endl;
			delete request;
			return 0;
		}
		if (tst->isSendData)
		{
			while (!fs.eof())
			{
				vector<char> data(FRAME_SIZE, 0);
				fs.read(&data[0], sizeof(char) * FRAME_SIZE);
				int nlen = fs.gcount();
				nlen = request->SendAudio((char*)&data[0], nlen);
				if (nlen <= 0)
				{
					std::cout << "send data fail" << endl;
					break;
				}
				//		std::cout << "Send len: " << nlen << endl;
#ifdef _WINDOWS_COMPILE_
				Sleep(50);
#else
				usleep(50 * 1000);
#endif
			}
		}
		fs.close();
		request->Stop();
		delete request;
		request = NULL;
	}
	catch (const char* e)
	{
		std::cout << e << endl;
	}
	return 0;
}
void OnResultDataRecved(NlsEvent* str,void* para=NULL)
{
	std::cout << "Id:" << (*(int*)para) << " " << str->getResponse() << endl;
}

void OnOperationFailed(NlsEvent* str, void* para=NULL)
{
	std::cout << "Id:" << (*(int*)para) << " " << str->getErrorMessage() << endl;
}
void OnChannelCloseed(NlsEvent* str, void* para=NULL)
{
	std::cout << "Id:" << (*(int*)para) << " " << str->getResponse() << endl;
}


int main(int arc, char* argv[])
{
	try
	{
		if (arc <= 7)
		{
			cout << "param is not valid. Usage: demo test.wav config.txt 1 1 [your-id] [your-scret] 1" << endl;
			return -1;
		}
        int id = 123434;
		gCallback = new NlsSpeechCallback();
		gCallback->setOnMessageReceiced(OnResultDataRecved,&id);
		gCallback->setOnOperationFailed(OnOperationFailed,&id);
		gCallback->setOnChannelClosed(OnChannelCloseed,&id);
		gNlc = NlsClient::getInstance();
		int ret = gNlc->setLogConfig("log-asr.txt",3);
		ParamStruct pa;
		pa.filename = argv[1];
		pa.config = argv[2];
		pa.threadcount = atoi(argv[3]);
		pa.count = atoi(argv[4]);
		pa.id = argv[5];
		pa.key = argv[6];
		pa.isSendData = strcmp(argv[7], "0") == 0 ? false : true;
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
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
	return 0;
}

