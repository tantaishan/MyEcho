#include "StringUtil.h"
#include <time.h>
#include <stdlib.h>

static const std::string s_RandomCharsA = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz";

static const std::wstring s_RandomCharsW =
	L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	L"abcdefghijklmnopqrstuvwxyz";

std::string StringUtil::RandomStringA(int nLength)
{
	srand((int)time(0));
	std::string strRandom;
	for (int i = 0; i < nLength; ++i)
		strRandom += s_RandomCharsA[rand() % s_RandomCharsA.length()];
	return strRandom;
}

std::wstring StringUtil::RandomStringW(int nLength)
{
	srand((int)time(0));
	std::wstring strRandom;
	for (int i = 0; i < nLength; ++i)
		strRandom += s_RandomCharsW[rand() % s_RandomCharsW.length()];
	return strRandom;
}

std::vector<std::string> StringUtil::SplitA(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;
	unsigned int size = str.size();

	for (unsigned int i = 0; i < size; i++)
	{
		pos=str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos-i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}

	return result;
}

std::vector<std::wstring> StringUtil::SplitW(std::wstring str, std::wstring pattern)
{
	std::wstring::size_type pos;
	std::vector<std::wstring> result;
	str += pattern;
	unsigned int size = str.size();

	for (unsigned int i = 0; i < size; i++)
	{
		pos=str.find(pattern, i);
		if (pos < size)
		{
			std::wstring s = str.substr(i, pos-i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}

	return result;
}

static const std::string base64_chars = 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}
