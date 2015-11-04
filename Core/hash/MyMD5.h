#pragma once
#include <istream>

class MyMD5
{
public:
	static std::string MD5(const std::string& str);
	static std::wstring MD5(const std::wstring& str);
};

