#ifndef __STD_CXX_H__
#define __STD_CXX_H__

#include <iostream>
#include <string>
#include <sstream>
using namespace	std;
#include <locale>

namespace base
{
	wstring	stdcxx_s2ws(string s);
	string stdcxx_ws2s(wstring ws);
	string stdcxx_replaceA(const string& str, const string& src, const string& dest);
	wstring stdcxx_replaceW(const wstring& str, const wstring& src, const wstring& dest);
	string	stdcxx_urlencoder(string& sIn);
	wstring stdcxx_f2ws(double n);
	wstring stdcxx_l2ws(long long n);
	string	stdcxx_u2s(const char* str, size_t str_len);
	wstring stdcxx_hwnd2ws(HWND hWnd);
	HWND stdcxx_ws2hwnd(const wstring& wstr);
	int stdcxx_wscmp_ver(const wstring& ver1, const wstring& ver2);
	wstring	stdcxx_f2bytes(double f);
	wstring	stdcxx_second2ws(unsigned long long n);
	wstring	stdcxx_ftime2ws(const FILETIME *ftime);
	string stdcxx_s2utf8(const string& str);
	string stdcxx_utf8ts(const string& utf8);
	string stdcxx_ws2utf8(const wstring& wstr);
	wstring stdcxx_utf8tws(const string& utf8);

	template<class T>
	wstring stdcxx_n2ws(T n)
	{
		wstringstream wss;
		wss << n;
		return wss.str();
	}

	template<class T>
	string stdcxx_n2s(T n)
	{
		stringstream ss;
		ss << n;
		return ss.str();
	}

	template<class T>
	wstring stdcxx_n2hex(T n)
	{
		wstringstream wss;
		wss << std::hex << n;
		//wstring ws;
		//ws << std::hex << wss.str();
		return wss.str();
	}

	inline unsigned char tohex(const unsigned char& x)
	{
		return x > 9 ? x + 55: x + 48;
	}
}

#endif//__STD_CXX_H__
