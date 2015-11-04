//////////////////////////////////////////////////////////////////////////
// Common convert functions.
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windef.h>
#include <wchar.h>
#include <tchar.h>
#include <iostream>
#include <afxstr.h>

#include "../base/str/stdcxx.h"
#include "../Core/json/json.h"

namespace conv
{
	static inline std::wstring i2ws(int value)
	{
		wchar_t _str[MAX_PATH] = { 0 };
		_itow_s(value, _str, MAX_PATH, 10);
		return _str;
	}

	// wstring -> string
	static inline std::string ws2s(const std::wstring& str)
	{
		return base::stdcxx_ws2s(str);
	}

	// int -> string
	static inline std::string i2s(int value)
	{
		char _str[MAX_PATH] = { 0 };
		_itoa_s(value, _str, MAX_PATH, 10);
		return _str;
	}

	// __int64 -> string
	static inline std::string i64tos(__int64 value)
	{
		char _str[MAX_PATH] = { 0 };
		_i64toa_s(value, _str, MAX_PATH, 10);
		return _str;
	}

	// json value to string
	static inline std::string js2s(const Json::Value& json, const std::string key)
	{
		return json[key].asString();
	}

	// json value to wstring
	static std::wstring js2ws(const Json::Value& json, const std::string key)
	{
		return base::stdcxx_s2ws(js2s(json, key));
	}

	// json value to int
	static inline int js2i(const Json::Value& json, const std::string key)
	{
		return atoi(json[key].asString().c_str());
	}

	static inline CString s2wcs(const std::string& str)
	{
		return CString(base::stdcxx_s2ws(str).c_str());
	}
}
