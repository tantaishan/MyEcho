#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <tchar.h>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
typedef std::basic_ostringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > otstringstream;
typedef std::basic_istringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > itstringstream;

void remove_char(tstring & s, TCHAR c);				// removes particular character;
void remove_chars(tstring & s, LPCTSTR symbols);	// removes set of characters;
void remove_repeat(tstring & s, TCHAR c);			// removes repeated character;

#define SPACES _T(" \t\r\n")

inline void trim_right(tstring & s, LPCTSTR delims = SPACES)
{
	s.erase(s.find_last_not_of(delims) + 1);
}

inline void trim_left(tstring & s, LPCTSTR delims = SPACES)
{
	s.erase(0, s.find_first_not_of(delims));
}

inline void trim(tstring & s, LPCTSTR delims = SPACES)
{
	trim_right(s, delims);
	trim_left(s, delims);
}

inline void make_upper(tstring & s)
{
	transform(s.begin(), s.end(), s.begin(), toupper);
}

inline void make_lower(tstring & s)
{
	transform(s.begin(), s.end(), s.begin(), tolower);
}

inline void shrink(tstring & s)
{
	s.resize(::_tcslen(s.c_str()));
}

inline void tohex(tstring & s, long value)
{
	otstringstream os;
	os << std::hex << value;
	s = os.str();
}

inline void toint(tstring & s, long value)
{
	otstringstream os;
	os << value;
	s = os.str();
}

// adds necessary c symbols in front of the converted
// hex number to make up n symbols in total;
inline void tohexfill(tstring & s, long value, long n, TCHAR c = '0')
{
	otstringstream os;
	os << std::setfill<TCHAR>(c) << std::setw(n) << std::hex << value;
	s = os.str();
}

// adds necessary c symbols in front of the converted
// integer number to make up n symbols in total;
inline void tointfill(tstring & s, long value, long n, TCHAR c = '0')
{
	otstringstream os;
	os << std::setfill<TCHAR>(c) << std::setw(n) << value;
	s = os.str();
}

template<class T>
inline bool fromhex(LPCTSTR s, T & value)
{
	value = 0;
	itstringstream is(s);
	return (is >> std::hex >> value)?true:false;
}

template<class T>
inline bool fromint(LPCTSTR s, T & value)
{
	value = 0;
	itstringstream is(s);
	return (is >> value)?true:false;
}

inline bool isvalidhex(LPCTSTR s, long minValue = 0, long MaxValue = 0xFFFFFFFF)
{
	long value;
	return fromhex(s, value) && value >= minValue && value <= MaxValue;
}

inline bool isvalidint(LPCTSTR s, long minValue = 0, long MaxValue = 0xFFFFFFFF)
{
	long value;
	return fromint(s, value) && value >= minValue && value <= MaxValue;
}
