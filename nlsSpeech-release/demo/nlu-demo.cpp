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
#include <functional>
#ifdef _WINDOWS_COMPILE_
#include <windows.h>
#else
#include <unistd.h>
#endif
using namespace std;
#define FRAME_SIZE 640
NlsSpeechCallback* callback = NULL;
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
		/*******************第一种调用方式***************************/
		// 创建一句话识别  NlsRequest，第一个参数为gCallback对象，第二个参数为config.txt文件
		NlsRequest* request = gNlc->createNluRequest(callback, tst->config.c_str());
		/***********************************************************/

		/*******************第二种调用方式**************************
		NlsRequest* request = nlc.createNluRequest(callback, NULL);
		request->SetParam("AppKey","nls-service");
		request->SetParam("AppId","com.aliyun.dataapi.nls.api.nlu");
		request->SetParam("DeviceType","aliyun.dataapi.nls");
		request->SetParam("AppUserId","user_id");
		request->SetParam("Url","wss://nls.dataapi.aliyun.com:443");
		request->SetParam("DeviceId","device_id");
		request->SetParam("BstreamAttached","true");
		request->SetParam("Format","pcm");
		request->SetParam("AsrInEnable","true");
		request->SetParam("DsEnable","true");
		request->SetParam("BstreamAttached","true");
		******************/
		if (request == NULL)
		{
			std::cout << "createTtsRequest fail" << endl;
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

void OnResultDataRecved(NlsEvent* str, void* para = NULL)
{
	ParamStruct* tst = (ParamStruct*)para;
	std::cout << "Id:" << (*(int*)para) << " " << str->getResponse() << endl;
}

void OnOperationFailed(NlsEvent* str, void* para = NULL)
{
	ParamStruct* tst = (ParamStruct*)para;
	std::cout << "Id:" << (*(int*)para) << " " << str->getErrorMessage() << endl;
}
void OnChannelCloseed(NlsEvent* str, void* para = NULL)
{
	ParamStruct* tst = (ParamStruct*)para; 
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
		callback = new NlsSpeechCallback();
		int id = 123434;
		gNlc = NlsClient::getInstance();
		int ret = gNlc->setLogConfig("log-nlu.txt",3);
		ParamStruct pa;
		pa.filename = argv[1];
		pa.config = argv[2];
		pa.threadcount = atoi(argv[3]);
		pa.count = atoi(argv[4]);
		pa.id = argv[5];
		pa.key = argv[6];
		pa.isSendData = strcmp(argv[7], "0") == 0 ? false : true;
		const int ThreadCount = pa.threadcount;
		callback->setOnOperationFailed(OnOperationFailed, &id);
		callback->setOnChannelClosed(OnChannelCloseed, &id);
		callback->setOnMessageReceiced(OnResultDataRecved, &id);
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

