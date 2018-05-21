#pragma once
#ifdef _WINDOWS_COMPILE_
#ifndef  ASR_API
#define ASR_API _declspec(dllexport)
#endif
#else
#define ASR_API
#endif
#include <iostream>
#include <vector>
#include <map>
#include "pthread.h"
class INlsRequestParam;
class Nls;
class ASR_API NlsEvent
{
public:
	enum EventType
	{
		Normal = 0,
		Error = 1,
		Close =2,
		Binary =3
	};
	NlsEvent(NlsEvent& );
	NlsEvent(std::string msg, int code, EventType type);
	NlsEvent(std::vector<unsigned char> data, int code, EventType type);
	~NlsEvent();
	std::string getStausCode();
	std::string getResponse();
	std::string getErrorMessage();
	std::vector<unsigned char> getBinaryData();
	EventType getMsgType();
	std::string getId();
	void setId(std::string id);
private:
	int _errorcode;
	std::string _msg;
	EventType _msgtype;
	std::string _identifyId;
	std::vector<unsigned char> _binaryData;
};

typedef void(*NlsCallbackMethod)(NlsEvent*,void*);

class ASR_API NlsSpeechCallback
{
public:
	NlsSpeechCallback();
	~NlsSpeechCallback();
	void setOnMessageReceiced(NlsCallbackMethod onMessageReceived,void*para=NULL);
	void setOnOperationFailed(NlsCallbackMethod _event, void*para=NULL);
	void setOnChannelClosed(NlsCallbackMethod _event, void*para=NULL);
	void setOnBinaryDataReceived(NlsCallbackMethod _event, void*para=NULL);
	NlsCallbackMethod _onMessageReceived;
	NlsCallbackMethod _onOperationFailed;
	NlsCallbackMethod _onChannelClosed;
	NlsCallbackMethod _onBinaryDataReceived;
	std::map < NlsEvent::EventType, void*> _paramap;
};
class ASR_API NlsRequest
{
public:
	NlsRequest(NlsSpeechCallback*, INlsRequestParam* rtnr, int requestmode = 3);
	~NlsRequest();
	int SetParam(const char* str_key, const char* str_value);	// add format
	int Start();
	int Stop();
	int Authorize(const char* id, const char* scret);
	int SendAudio(char* data, size_t num_byte);
private:
	Nls* _session;
	NlsSpeechCallback* _callbck;
	int _mode;
};

class ASR_API NlsClient
{
public:
	~NlsClient();
	int setLogConfig(const char* logOutputFile,int loglevel);
	NlsRequest* createRealTimeRequest(NlsSpeechCallback* onResultReceivedEvent, const char* config);//realtime
	NlsRequest* createAsrRequest(NlsSpeechCallback* onResultReceivedEvent, const char* config);//asr
	NlsRequest* createTtsRequest(NlsSpeechCallback* onResultReceivedEvent, const char* config);//tts
	NlsRequest* createNluRequest(NlsSpeechCallback* onResultReceivedEvent, const char* config);//
	static NlsClient* getInstance(bool sslInitial = true);
private:
	NlsClient();
	static pthread_mutex_t _mtx;
	static bool _isInitializeSSL;
	static NlsClient* _instance;
};