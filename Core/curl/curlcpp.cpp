#include "curlcpp.h"

#define CURL_STATICLIB
#include "curl.h"
#include <io.h>

#if defined(_DEBUG)
#pragma comment(lib, "libcurld.lib")
#else
#pragma comment(lib, "libcurl.lib")
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wldap32.lib")

int curlcpp::global_init()
{
	return (int)curl_global_init(CURL_GLOBAL_ALL);
}

int curlcpp::global_init(long flags)
{
	return (int)curl_global_init(flags);
}


void curlcpp::global_cleanup()
{
	curl_global_cleanup();
}

curlcpp::curlcpp(bool is_https, char* ssl_cert)
{
	m_error = new char[CURL_ERROR_SIZE];

	m_curl = curl_easy_init();
	m_headers = 0;
	m_request_data_offset = 0;

	curl_version_info_data * vinfo = curl_version_info( CURLVERSION_NOW );
	if( vinfo->features & CURL_VERSION_SSL)
	{
		//LOG4CXX_INFO(loggerNDisk, "SSL support enabled\n");
		if(is_https)
		{
			if(!ssl_cert)
			{
				curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, FALSE);
				curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, FALSE);
			}
			else
			{
				curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, TRUE);
				curl_easy_setopt(m_curl, CURLOPT_CAINFO, ssl_cert);
			}
		}
	}
	else
	{
		//LOG4CXX_INFO(loggerNDisk, "No SSL\n");
	}
}

curlcpp::~curlcpp(void)
{
	if (m_error)
	{
		delete []m_error;
		m_error = 0;
	}

	if(m_curl)
	{
		curl_easy_cleanup(m_curl);
		m_curl = 0;
	}

	if(m_headers)
	{
		curl_slist_free_all(m_headers);
		m_headers = 0;
	}
}

int curlcpp::enable_error()
{
	if(!m_curl)
		return -1;

	return (int)curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, m_error);
}

int curlcpp::set_verbose(long verbose)
{
	if(!m_curl)
		return -1;

	return (int)curl_easy_setopt(m_curl, CURLOPT_VERBOSE, verbose);
}

int curlcpp::set_post_data(string data)
{
	return curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data);
}

int curlcpp::set_url(string url)
{
	if(!m_curl)
		return -1;

	return (int)curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
}

void curlcpp::add_header(string header)
{
	m_headers = curl_slist_append(m_headers, header.c_str());
}

void curlcpp::add_header(string headerKey, string headerValue)
{
	string header = headerKey + ": " + headerValue;
	m_headers = curl_slist_append(m_headers, header.c_str());
}

void curlcpp::reset_headers()
{
	if(m_headers)
	{
		curl_slist_free_all(m_headers);
		m_headers = 0;
	}
}

int curlcpp::post(string request, string& response)
{
	int ret = 0;

	do 
	{
		if(!m_curl)
        {
			ret = -1;
			break;
        }

		m_response_data = "";
		m_request_data = request;

		/*ret = (int)curl_easy_setopt(m_curl, CURLOPT_FORBID_REUSE, 1); 
		
		if(ret != 0)
			break;*/

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_POST, 1L);

		if(ret != 0)
			break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, (curl_off_t)m_request_data.length());

		if(ret != 0)
			break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_request_data.c_str());

		if(ret != 0)
            break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlcpp::write_data);

		if(ret != 0)
            break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response_data);

		if(ret != 0)
            break;

		if(m_headers)
		{
			ret = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);

			if(ret != 0)
                break;
		}

		ret = curl_easy_perform(m_curl);
		 
		if(ret != 0)
            break;

		int httpCode;
		ret = curl_easy_getinfo(m_curl,CURLINFO_HTTP_CODE,&httpCode);

		if (ret != 0)
			break;

		if (httpCode != 200)
		{
			ret = -1;
			break;
		}

		response = m_response_data;

	} while (false);

	return ret;
}

int curlcpp::post_multipart(const string& request, string& response)
{
	int ret = 0;
	struct curl_httppost *formpost = 0;
	struct curl_httppost *lastptr = 0;
	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);
	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_PTRCONTENTS, request.c_str(), CURLFORM_CONTENTSLENGTH, request.size(), CURLFORM_END);

	//curl_easy_setopt(m_curl, CURLOPT_URL, URL);
	curl_easy_setopt(m_curl, CURLOPT_HTTPPOST, formpost);

	ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlcpp::write_data);
	ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response_data);

	ret = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);

	ret = curl_easy_perform(m_curl);

	int httpCode = 0;
	ret = curl_easy_getinfo(m_curl, CURLINFO_HTTP_CODE, &httpCode);

	curl_formfree(formpost);

	response = m_response_data;

	return ret;
}

int curlcpp::get(string& response)
{
	int ret = 0;

	do 
	{
		if(!m_curl)
        {
			ret = -1;
			break;
        }

		m_response_data = "";

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_POST, 0L);

		if(ret != 0)
			break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlcpp::write_data);

		if(ret != 0)
            break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response_data);

		if(ret != 0)
            break;

		if(m_headers)
		{
			ret = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);

			if(ret != 0)
                break;
		}

		ret = curl_easy_perform(m_curl);

		if(ret != 0)
            break;

		int httpCode;
		ret = curl_easy_getinfo(m_curl,CURLINFO_HTTP_CODE,&httpCode);

		if (ret != 0)
			break;

		if (httpCode != 200)
		{
			ret = -1;
			break;
		}

		response = m_response_data;

	} while (false);

	return ret;
}

int curlcpp::get_response_code(long& retcode)
{
	if(!m_curl)
		return -1;

	return (int)curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE , &retcode); 
}

__int64 curlcpp::get_http_file_size(string url)
{
	__int64 size = 0;
	CURL* curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);

		CURLcode res = curl_easy_perform(curl);

		if(res == CURLE_OK)
		{
			double sz = 0;
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD/*CURLINFO_SIZE_DOWNLOAD*/, &sz);
			size = (__int64)sz;
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	return size;
}

int curlcpp::download(string filePath)
{
	int ret = 0;

	do 
	{
		if(!m_curl)
		{
			ret = -1;
			break;
		}

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_FORBID_REUSE, 1); 

		if(ret != 0)
			break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, curlcpp::write_data);

		if(ret != 0)
			break;

		ret = (int)curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response_data);

		if(ret != 0)
			break;

		ret = curl_easy_perform(m_curl);
		
		if(ret != 0)
			break;

		int retcode = 0;
		ret = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE , &retcode);  
		if (CURLE_OK == ret && 200 == retcode)
		{
			HANDLE hFile = ::CreateFileA(filePath.c_str(), GENERIC_WRITE | GENERIC_READ,
				0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
				return FALSE;

			DWORD dwWritten = 0;
			WriteFile(hFile, m_response_data.c_str(), m_response_data.length(), &dwWritten, NULL);

			::FlushFileBuffers(hFile);
			::CloseHandle(hFile);

// 			FILE* pf = 0;
// 			errno_t err = fopen_s(&pf, filePath.c_str(), "wb+");
// 			if(!pf)
// 			{
// 				ret = -2;
// 				break;
// 			}
// 
// 			fwrite(m_response_data.c_str(), 1, m_response_data.length(), pf);
// 			fclose(pf);
		}
		else
		{
			ret = -3;
			break;
		}

	} while (false);

	return ret;
}

int curlcpp::download_progress(string url, string output, write_func_callback write_func,
	read_func_callback read_func, progress_callback progress_func, void* data)
{
	int res = -1;
	//m_curl = curl_easy_init();
	FILE *file = NULL;
	errno_t err;
	int retcode, ret;
	
	//HANDLE hFile = (HANDLE)CreateFileA(output.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
	//int nHandle = _open_osfhandle((long)hFile, _O_TEXT | _O_APPEND);
	//file = _fdopen(nHandle, "wt"); 
	err = fopen_s(&file, output.c_str(), "wb");
	if (err || !file) return res;
	//curl_easy_setopt(m_curl, CURLOPT_URL, url);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, file);
	//curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_response_data);
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION,write_func);
	curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, read_func);
	curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE , &retcode);
	curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, FALSE);
	curl_easy_setopt(m_curl, CURLOPT_PROGRESSFUNCTION, progress_func);
	curl_easy_setopt(m_curl, CURLOPT_PROGRESSDATA, data);

	//res = curl_easy_perform(curl);
	//fclose(file);
	/* always cleanup */
	//curl_easy_cleanup(curl);
	ret = curl_easy_perform(m_curl);
	if (ret == CURLE_OK)
	{
		ret = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE , &retcode);
		if (CURLE_OK == ret && 200 == retcode)
		{	//
			//FILE* pf = 0;
			//fopen_s(&pf, output.c_str(), "w+b");
			//if(!pf) ret = -2;
			fwrite(m_response_data.c_str(), 1, m_response_data.length(), file);
			res = CURLE_OK;
		}
	}
	
	fclose(file);
	//curl_easy_cleanup(curl);
	return res;
}
