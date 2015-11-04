#pragma once
#include <iostream>
#include <vector>

class StringUtil
{
public:
	static std::string RandomStringA(int nLength);
	static std::wstring RandomStringW(int nLength);
	static std::vector<std::string> SplitA(std::string str, std::string pattern);
	static std::vector<std::wstring> SplitW(std::wstring str, std::wstring pattern);
};

