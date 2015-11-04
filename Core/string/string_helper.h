#pragma once

class string_helper
{
	//string -> T
public:
	static TCHAR	str_to_ch(tstring str);
	static int		str_to_int(tstring str);
	static __int64	str_to_int64(tstring str);
	static double	str_to_double(tstring str);

	//T -> string
public:
	static tstring	str_from_ch(TCHAR ch);
	static tstring	str_from_int(int i);
	static tstring	str_from_int64(__int64 i);
	static tstring	str_from_double(double d);

	// Others
public:
	static std::string atrim(__inout std::string& s);
	static std::wstring wtrim(__inout std::wstring& s);
	static tstring trim(__inout tstring& s);

private:
	string_helper();
	~string_helper();
};