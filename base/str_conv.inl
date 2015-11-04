//	str_conv.inl
//	2012/08/24
//


#ifndef __INCLUDE_STR_CONV_H__
#define __INCLUDE_STR_CONV_H__

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

#ifdef _WIN32
#	include <windows.h>
#endif // _WIN32

/**
*	Usage:
*		std::ostringstream _stream;
*		_stream << "ansi text" << L"unicode text" << 123;
*
*		std::wostringstream _wstream;
*		_wstream << "ansi text" << L"unicode text" << 123;
*/

//
//	UNICODE => ANSI
//
_inline std::ostream& operator<<(std::ostream& out, const wchar_t* wstr)
{
#ifdef _MSC_VER
	int iLen = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if(iLen > 0)
	{
		char* pszDst = new char[iLen];
		if(pszDst)
		{
			::WideCharToMultiByte(CP_ACP, 0, wstr, -1, pszDst, iLen, NULL, NULL);
			pszDst[iLen -1] = 0;
			out << pszDst;
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
	out << _Dest;
	delete []_Dest;

	setlocale(LC_ALL, curLocale.c_str());
#endif
	return (out);
}
_inline std::ostream& operator<<(std::ostream& out, const std::wstring& wstr) {
	return out << wstr.c_str();
}

//
//	ANSI => UNICODE
//
_inline std::wostream& operator<<(std::wostream& out, const char* str)
{
#ifdef _MSC_VER
	int iLen = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	if(iLen > 0)
	{
		wchar_t* pwszDst = new wchar_t[iLen];
		if(pwszDst)
		{
			::MultiByteToWideChar(CP_ACP, 0, str, -1, pwszDst, iLen);
			pwszDst[iLen -1] = 0;
			out << pwszDst;
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
	out << _Dest;
	delete []_Dest;

	setlocale(LC_ALL, curLocale.c_str());
#endif
	return (out);
}
_inline std::wostream& operator<<(std::wostream& out, const std::string& str) {
	return out << str.c_str();
}


/**
*	Usage:
*	LOG(DEBUG) << "found some error" << GLE;
*
*	Demo Output:
*	[INFO] [2012-08-28_16:58:51_921_2] .\xxx.cpp:514 found some error [GLE:2]
*
*	>>>Attention<<<
*		以上用法可能记录的错误码有误差, 因为在发生错误的函数调用后,在调用GetLastError()
*	之前, LOG宏中调用了其他API, 这些API也是可能发生错误的, 而真正要记录的值会被覆盖.
*/
_inline std::ostream& GLE(std::ostream& out){
	out << " [GLE:" << ::GetLastError() << "]";
	return (out);
}

#endif	//  #ifndef __INCLUDE_STR_CONV_H__

