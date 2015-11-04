#ifndef curlcpp_h
#define curlcpp_h

#pragma warning(disable:4251)

#include <string>
using namespace std;

struct curl_slist;


typedef int (progress_callback)(const void *,
	double, double, double, double);

typedef size_t (write_func_callback)(void* ptr, size_t size, size_t nmemb, FILE* stream);
typedef size_t (read_func_callback)(void* ptr, size_t size, size_t nmemb, FILE* stream);

class curlcpp
{
public:
	static int global_init();
	static int global_init(long flags);
	static void global_cleanup();
	
public:
	curlcpp(bool is_https=true, char* ssl_cert=0);
	~curlcpp(void);

public:
	//int perform();

	int set_post_data(string data);

	int set_url(string url);

	void add_header(string header);

	void add_header(string headerKey, string headerValue);

	void reset_headers();

	int enable_error();

	int set_verbose(long verbose);

	int post(string request, string& response);
	int post_multipart(const string& request, string& response);

	int get(string& response);

	int download(string filePath);

	int get_response_code(long& retcode);

	string get_error() { return m_error; }

	__int64 get_http_file_size(string url);

	int download_progress(string url, string output, write_func_callback write_func,
		read_func_callback read_func, progress_callback progress_func, void* data);
public:
	static size_t write_data(void *ptr, size_t size, size_t nmemb, void *userp)
	{
		string data((char*)ptr, size * nmemb);
		string* responseData = (string*)userp;
		responseData->append(data);
		return size * nmemb;
	}

	static size_t read_data(void *ptr, size_t size, size_t nmemb, void *userp)
	{
		size_t ret = 0;
		curlcpp* pThis = (curlcpp*)userp;
		size_t leftsize = pThis->m_request_data.length() - pThis->m_request_data_offset;

		if(leftsize)
		{
			ret = size * nmemb;

			if(ret > leftsize)
				ret = leftsize;

			memcpy(ptr, pThis->m_request_data.c_str() + pThis->m_request_data_offset, ret);
			pThis->m_request_data_offset += ret;
		}
		
		return ret;
	}


protected:
	void* m_curl;
	char* m_error;
    //string m_error;
	
	curl_slist* m_headers;

	string m_request_data;
	size_t m_request_data_offset;
	string m_response_data;
};

#endif //!curlcpp_h
