#include "stdafx.h"
#include "stdcxx.h"
#include <io.h>
#include <wchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace base
{
	wstring	stdcxx_s2ws(string s)
	{
		wstring ws;
	#ifdef _MSC_VER
		int iLen = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);
		if(iLen > 0)
		{
			wchar_t* pwszDst = new wchar_t[iLen];
			if(pwszDst)
			{
				::MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, pwszDst, iLen);
				pwszDst[iLen -1] = 0;
				ws= pwszDst;
				delete []pwszDst;
			}
		}
	#else
		std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
		setlocale(LC_ALL, "chs"); 
		size_t _Dsize = 2 * (wcslen(wstr) + 1);
		char *_Dest = new char[_Dsize];
		memset(_Dest, 0, _Dsize);
		wcstombs(_Dest, wstr, _Dsize);
		ws= _Dest;
		delete []_Dest;

		setlocale(LC_ALL, curLocale.c_str());
	#endif
		return (ws);
	}

	string stdcxx_ws2s(wstring wstr)
	{
		string str;
	#ifdef _MSC_VER
		int iLen = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if(iLen > 0)
		{
			char* pszDst = new char[iLen];
			if(pszDst)
			{
				::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszDst, iLen, NULL, NULL);
				pszDst[iLen -1] = 0;
				str= pszDst;
				delete [] pszDst;
			}
		}
	#else
		std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
		setlocale(LC_ALL, "chs"); 
		size_t _Dsize = 2 * (wcslen(wstr) + 1);
		char *_Dest = new char[_Dsize];
		memset(_Dest, 0, _Dsize);
		wcstombs(_Dest, wstr, _Dsize);
		str= _Dest;
		delete []_Dest;

		setlocale(LC_ALL, curLocale.c_str());
	#endif
		return (str);
	}

	string stdcxx_replaceA(const string& str, const string& src, const string& dest)
	{
		string ret;
		string::size_type pos_begin = 0;
		string::size_type pos       = str.find(src);
		while (pos != string::npos)
		{
			ret.append(str.data() + pos_begin, pos - pos_begin);
			ret += dest;
			pos_begin = pos + src.length();
			pos       = str.find(src, pos_begin);
		}
		if (pos_begin < str.length())
		{
			ret.append(str.begin() + pos_begin, str.end());
		}
		return ret;
	}

	wstring stdcxx_replaceW(const wstring& str, const wstring& src, const wstring& dest)
	{
		wstring ret;
		wstring::size_type pos_begin = 0;
		wstring::size_type pos       = str.find(src);
		while (pos != wstring::npos)
		{
			ret.append(str.data() + pos_begin, pos - pos_begin);
			ret += dest;
			pos_begin = pos + src.length();
			pos       = str.find(src, pos_begin);
		}
		if (pos_begin < str.length())
		{
			ret.append(str.begin() + pos_begin, str.end());
		}
		return ret;
	}

	string	stdcxx_urlencoder(string& sIn)
	{
		string sOut;
		for(unsigned int ix = 0; ix < sIn.size(); ix++ )
		{
			unsigned char buf[4];
			memset( buf, 0, 4 );
			if( isalnum( (unsigned char)sIn[ix] ) )
			{
				buf[0] = sIn[ix];
			}
			else if ( isspace( (unsigned char)sIn[ix] ) )
			{
				buf[0] = '+';
			}
			else
			{
				buf[0] = '%';
				buf[1] = tohex( (unsigned char)sIn[ix] >> 4 );
				buf[2] = tohex( (unsigned char)sIn[ix] % 16);
			}
			sOut += (char *)buf;
		}
		return sOut;
	}

	wstring stdcxx_l2ws(long long n)
	{
		wstringstream wss;
		wss << n;
		return wss.str();
	}

	wstring stdcxx_f2ws(double n)
	{
		wstringstream wss;
		wss << n;
		return wss.str();
	}

	string	stdcxx_u2s(const char* str, size_t str_len)
	{
	#define CVT(x) ((x)>=0 && (x)<=9)?(x)+0x30:((x)>=0xa && (x)<=0xf)?(x)+0x57:0;
		char* buffer= new char[str_len*2+1];
		memset(buffer, 0, str_len*2+1);
		char temp[16]={};
		for(size_t ix=0; ix<str_len; ++ix)
		{
			buffer[ix*2+0]= (str[ix]&0xF0)>>4;
			buffer[ix*2+0]= CVT(buffer[ix*2+0]);
			buffer[ix*2+1]= (str[ix]&0x0F)>>0;
			buffer[ix*2+1]= CVT(buffer[ix*2+1]);
		}
		string _(buffer);
		delete buffer;
		return _;
	#undef CVT
	}

	wstring stdcxx_hwnd2ws(HWND hWnd)
	{
		wchar_t wszHWndView[64] = {};
		::wnsprintfW(wszHWndView, _countof(wszHWndView), L"%X", hWnd);
		return wszHWndView;
	}

	HWND stdcxx_ws2hwnd(const wstring& wstr)
	{
		__int64 i64tmp = __int64(0);
		std::wstring wstrWnd = wstr;
		if (wstrWnd[0] != L'0' && (wstrWnd[0] != L'x' || wstrWnd[0] != L'X'))
			wstrWnd = L"0x" + wstrWnd;
		::StrToInt64ExW(wstrWnd.c_str(), STIF_SUPPORT_HEX, &i64tmp);
		return (HWND)i64tmp;
	}

	int stdcxx_wscmp_ver(const wstring& ver1, const wstring& ver2)
	{
		int vVer1[4] = {0};
		int vVer2[4] = {0};
		swscanf(ver1.c_str(), L"%d.%d.%d.%d", &vVer1[0], &vVer1[1], &vVer1[2], &vVer1[3]);
		swscanf(ver2.c_str(), L"%d.%d.%d.%d", &vVer2[0], &vVer2[1], &vVer2[2], &vVer2[3]);
		return ::memcmp(vVer1, vVer2, sizeof(vVer1));
	}

	wstring	stdcxx_f2bytes(double f)
	{
		WCHAR wbuf[64]={};
		if(1.0> f)
		{
			return wstring(L"0KB");
		}
		else if((1024.0*1024.0*1024.0*0.8)< f)
		{
			swprintf(wbuf, L"%.1fGB", f/(1024.0*1024.0*1024.0) );
		}
		else if((1024.0*1024.0*0.8)< f)
		{
			swprintf(wbuf, L"%.1fMB", f/(1024.0*1024.0) );
		}
		else
		{
			swprintf(wbuf, L"%.1fKB", f/(1024.0) );
		}
		return wstring(wbuf );
	}

	wstring	stdcxx_second2ws(unsigned long long n)
	{
		wstring _wstr;
		if(n/ 3600)
		{
			_wstr+= stdcxx_n2ws(n/ 3600 );
			_wstr+= L"Ð¡Ê±";
		}
		if((n%3600)/ 60)
		{
			_wstr+= stdcxx_n2ws((n%3600)/ 60 );
			_wstr+= L"·Ö";
		}
		if(n%60 )
		{
			_wstr+= stdcxx_n2ws(n%60 );
			_wstr+= L"Ãë";
		}
		if(0== _wstr.length() )
		{
			_wstr+= L"0Ãë";
		}
		return _wstr;
	}

	wstring	stdcxx_ftime2ws(const FILETIME *ftime)
	{
		SYSTEMTIME st = { 0 };
		wchar_t wsz[MAX_PATH] = { 0 };
		if (ftime)
		{
			FileTimeToSystemTime(ftime, &st);
			swprintf_s(wsz, MAX_PATH, L"%4d-%02d-%02d %02d:%02d:%2d",
				st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}
		return wsz;
	}
	string stdcxx_s2utf8(const string& str)
	{
		string _utf8(str);
		__int32 nLen = ::MultiByteToWideChar(CP_ACP, 0, _utf8.c_str(), _utf8.size(), NULL, 0);
		wchar_t* lpszW = NULL;
		lpszW = new wchar_t[nLen];
		__int32 nRtn = ::MultiByteToWideChar(CP_ACP, 0, _utf8.c_str(), _utf8.size(), lpszW, nLen);
		if (nRtn != nLen)
		{
			delete[] lpszW;
			return "";
		}
		// convert widechar string to utf-8
		__int32 utf8Len = ::WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, NULL, 0, NULL, NULL);
		if (utf8Len <= 0)
			return "";
		_utf8.resize(utf8Len);
		nRtn = ::WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, (LPSTR)_utf8.c_str(), utf8Len, NULL, NULL);
		delete[] lpszW;
		return _utf8;
	}

	string stdcxx_utf8ts(const string& utf8)
	{
		// convert an utf8 string to widechar
	
		__int32 nLen = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
		wchar_t* lpszW = NULL;
		lpszW = new wchar_t[nLen];
		__int32 nRtn = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), lpszW, nLen);
		if (nRtn != nLen)
		{
			delete[] lpszW;
			return "";
		}

		// convert widechar string to multibyte
		__int32 mbLen = ::WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);
		if (mbLen <= 0)
		{
			return "";
		}

		string _str;
		_str.resize(mbLen);
		nRtn = ::WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, (LPSTR)_str.c_str(), mbLen, NULL, NULL);
		if (nRtn != mbLen)
		{
			delete[] lpszW;
			return "";
		}

		delete[] lpszW;
		return _str;
	}
	string stdcxx_ws2utf8(const wstring& wstr)
	{
		string _utf8;
		__int32 utf8Len = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		if (utf8Len <= 0)
			return "";
		_utf8.resize(utf8Len);
		::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), (LPSTR)_utf8.c_str(), utf8Len, NULL, NULL);
		return _utf8;
	}
	wstring stdcxx_utf8tws(const string& utf8)
	{
		// convert an utf8 string to widechar
		__int32 nLen = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), NULL, 0);
		wstring _str;
		_str.resize(nLen);
		__int32 nRtn = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), (LPWSTR)_str.c_str(), nLen);
		return _str;
	}


}


