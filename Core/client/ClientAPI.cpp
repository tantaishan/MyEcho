#include "ClientAPI.h"
#include "UidManager.h"
#include "ClientVersion.h"
#include <atlstr.h>
#include <time.h>
#include <ShellAPI.h>
#include <shlobj.h>
#include "../file/FileManager.h"
#include "pipe/named_pipe.h"
#include "process/launch.h"
#include "str/stdcxx.h"
#include "../string/base64.h"
#include "../process/ProcessUtil.h"
//#include "../../Macros/duiConfig/window_main.h"
#include "../config.h"

#define USE_SSL		false


ClientAPI* ClientAPI::instance = 0;
Locker ClientAPI::singletonLocker;

ClientAPI* ClientAPI::Instance()
{
	if (!instance)
	{
		singletonLocker.Lock();
		if (!instance)
		{
			instance = new ClientAPI;
		}
		singletonLocker.Unlock();
	}
	return instance;
}

ClientAPI::ClientAPI(void)
{
	curlcpp::global_init();
}

ClientAPI::~ClientAPI(void)
{
	curlcpp::global_cleanup();
}

void ClientAPI::init_curl(curlcpp* curl, std::string url)
{
	curl->set_url(url);
	curl->enable_error();
	curl->reset_headers();

	curl->add_header(MAIN_USERAGENT);
	curl->add_header("Accept: text/html, application/xml;q=0.9, application/xhtml+xml, */*");
	curl->add_header("Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7");
	curl->add_header("Accept-Language: zh-cn,zh;q=0.5");
	curl->add_header("Content-Type: application/x-www-form-urlencoded;charset=UTF-8");
	curl->add_header("Connection: Keep-Alive");
}


ClientAPI_ErrorCode ClientAPI::DoPost(std::string url, std::string param, std::string &response)
{
	curlcpp curl(USE_SSL);
	init_curl(&curl, url);
	return curl.post(param, response) == 0 ? ClientAPI_OK : ClientAPI_Error_Network;
}

ClientAPI_ErrorCode ClientAPI::DoGet(__in std::string url, __out std::string &response)
{
	curlcpp curl(USE_SSL);
	init_curl(&curl, url);
	return curl.get(response) == 0 ? ClientAPI_OK : ClientAPI_Error_Network;
}

//////////////////////////////////////////////////////////////////////////
// UrlMake
namespace UrlMake
{
	class Url {
		typedef std::map<std::wstring, std::wstring> KeyValue;
	public:
		// constructor
		Url() {}
		Url(std::string url) {
			base_ = base::stdcxx_s2ws(url);
		}
		Url(std::wstring base) : base_(base) {}
		// set base url
		void setbase(std::wstring base) { base_ = base; }
		std::wstring getbase(void) { return base_; }
		// insert url param
		void add(std::wstring key, std::wstring value) {
			params_.insert(std::make_pair(key, value));
		}
		void add(std::wstring key, int value) {
			wchar_t _str[MAX_PATH] = { 0 };
			_itow_s(value, _str, 10);
			params_.insert(std::make_pair(key, _str));
		}
		// build url with params
		std::wstring buildw() {
			std::wstring _str;
			std::map<std::wstring, std::wstring>::iterator it = params_.begin();
			for (;it != params_.end(); ++it) {
				_str += it->first + L"=" + it->second + L"&";
			}
			std::wstring url = base_ + _str;
			url.erase(url.size()-1, 1);
			return url;
		}
		std::string builda() {
			return base::stdcxx_ws2s(buildw());
		}

		// conversion operator overloading
		operator std::string() {
			return builda();
		}
		operator std::wstring() {
			return buildw();
		}

	private:
		std::wstring	base_;
		KeyValue		params_;
	};
}

ClientAPI_ErrorCode ClientAPI::CheckUpdate(const std::string& url, UpdateInfo& info)
{
	USES_CONVERSION;
	ClientAPI_ErrorCode result = ClientAPI_OK;
	curlcpp curl(USE_SSL);

	do 
	{
		//SystemInfo::GetOSInfo(info.os_info);

		UrlMake::Url extra(url);
		extra.add(L"mid", UidManager::Instance()->GetMid());
		extra.add(L"aut", UidManager::Instance()->GetAut());
		extra.add(L"cid", ClientVersion::Instance()->GetCode());
		extra.add(L"ver", ClientVersion::Instance()->GetVersion());
		extra.add(L"pmid", UidManager::Instance()->GetPreMid());
		extra.add(L"mcn", UidManager::Instance()->GetMcn());

		extra.add(L"osver", info.os_info.os_ver);
		extra.add(L"osbit", info.os_info.os_bit);
		extra.add(L"iever", info.os_info.ie_ver);
		extra.add(L"dispix", info.os_info.dis_pix);

		init_curl(&curl, extra);

		std::string output;
		if (curl.post("", output) != 0)
		{
			result = ClientAPI_Error_Network;
			break;
		}

		try
		{
			Json::Reader reader;
			Json::Value responseJson;
			if (!reader.parse(output, responseJson))
			{
				result = ClientAPI_Error_Json;
				break;
			}
			if (!responseJson.isObject())
			{
				result = ClientAPI_Error_Json;
			}

			info.version_number = A2W(responseJson["version_number"].asString().c_str());
			info.version_explain = A2W(responseJson["version_explain"].asString().c_str());
			info.update_type = (UpdateType)atoi(responseJson["update_type"].asString().c_str());
			info.file_32 = A2W(responseJson["file_32"].asString().c_str());
		}
		catch (...)
		{
			result = ClientAPI_Error_Json;
			break;
		}

	} while (false);

	return result;
}


ClientAPI_ErrorCode ClientAPI::Download(const tstring& url,const tstring& outpath)
{
	USES_CONVERSION;
	std::string aUrl = W2A(url.c_str());
	std::string aOutpath = W2A(outpath.c_str());
	curlcpp curl(USE_SSL);
	init_curl(&curl, aUrl);
	int ret = curl.download(aOutpath);
	return (ClientAPI_ErrorCode)ret;
}

size_t my_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return _fwrite_nolock( ptr, size, nmemb, stream);
}

size_t my_read_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return _fread_nolock(ptr, size, nmemb, stream);
}

ClientAPI_ErrorCode ClientAPI::Download(const tstring& url,
	const tstring& output, progress_callback progress_func, void* progress_data)
{
	USES_CONVERSION;
	std::string aUrl = W2A(url.c_str());
	std::string aOutput = W2A(output.c_str());
	curlcpp curl(USE_SSL);
	init_curl(&curl, aUrl);
	ClientAPI_ErrorCode ret = (ClientAPI_ErrorCode)curl.download_progress(aUrl, aOutput, my_write_func, my_read_func, progress_func, progress_data);
	return ret;
}
