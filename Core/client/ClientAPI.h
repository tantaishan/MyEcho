#pragma once
#include "../curl/curlcpp.h"
#include "../thread/Locker.h"
#include "../types.h"
//////////////////////////////////////////////////////////////////////////

class curlcpp;

class ClientAPI
{
public:
	static ClientAPI* Instance();
	
private:
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (ClientAPI::instance)
				delete ClientAPI::instance;
		}
	};
	static CGarbo Garbo;

	void init_curl(curlcpp* curl, std::string url);

public:
	virtual ClientAPI_ErrorCode CheckUpdate(const std::string& url, UpdateInfo& info);
	virtual ClientAPI_ErrorCode Download(const tstring& url, const tstring& outpath);
	virtual ClientAPI_ErrorCode Download(const tstring& url, const tstring& output,
		progress_callback progress, void* progress_data);
	virtual ClientAPI_ErrorCode DoPost(__in std::string url, __in std::string param, __out std::string &response);
	virtual ClientAPI_ErrorCode DoGet(__in std::string url, __out std::string &response);
private:
	ClientAPI(void);
	~ClientAPI(void);
	static ClientAPI* instance;
	static Locker singletonLocker;
};

