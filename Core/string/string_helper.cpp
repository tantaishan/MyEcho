#include "../string/tstring.h"
#include "string_helper.h"

//string -> T
TCHAR string_helper::str_to_ch(tstring str)
{
	return str[0];
}

int string_helper::str_to_int(tstring str)
{
	return _ttoi(str.c_str());
}

__int64 string_helper::str_to_int64(tstring str)
{
	return _ttoi64(str.c_str());
}

double string_helper::str_to_double(tstring str)
{
	return _ttof(str.c_str());
}


//T -> string
tstring string_helper::str_from_ch(TCHAR ch)
{
	return tstring(1, ch);
}

tstring string_helper::str_from_int(int i)
{
	TCHAR buffer[1024] = {0};
	_itot_s(i, buffer, 10);
	return tstring(buffer);
}

tstring string_helper::str_from_int64(__int64 i)
{
	TCHAR buffer[1024] = {0};
	_i64tot_s(i, buffer, 1024, 10);
	return tstring(buffer);
}

tstring string_helper::str_from_double(double d)
{
	TCHAR buffer[1024] = {0};
	_stprintf_s(buffer, _T("%f"), d);
	return tstring(buffer);
}

std::string string_helper::atrim(__inout std::string& s)
{
	if (s.empty()) return s;

	std::string::iterator c;
	// Erase whitespace before the string
	for (c = s.begin(); c != s.end() && iswspace(*c++););
	s.erase(s.begin(), --c);

	// Erase whitespace after the string
	for (c = s.end(); c != s.begin() && iswspace(*--c););
	s.erase(++c, s.end());
	return s;
}

std::wstring string_helper::wtrim(__inout std::wstring& s)
{
	if (s.empty()) return s;

	std::wstring::iterator c;
	// Erase whitespace before the string
	for (c = s.begin(); c != s.end() && iswspace(*c++););
	s.erase(s.begin(), --c);

	// Erase whitespace after the string
	for (c = s.end(); c != s.begin() && iswspace(*--c););
	s.erase(++c, s.end());
	return s;
}

tstring string_helper::trim(__inout tstring& s)
{
#ifdef _UNICODE
	return wtrim(s);
#else
	return atrim(s);
#endif
}
