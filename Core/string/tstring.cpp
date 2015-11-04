#include "tstring.h"

void remove_char(tstring & s, TCHAR c)
{
	LPTSTR text = (LPTSTR)s.c_str();
	size_t size = _tcslen(text);
	long idx = 0;
	for(size_t i = 0;i < size;i ++)
		if(text[i] != c)
		{
			text[idx] = text[i];
			idx ++;
		}
	text[idx] = 0;
	s.resize(idx);
}

void remove_chars(tstring & s, LPCTSTR symbols)
{
	LPTSTR text = (LPTSTR)s.c_str();
	size_t s_size = _tcslen(text);
	size_t c_size = _tcslen(symbols);
	long idx = 0;
	for(size_t i = 0;i < s_size;i ++)
	{
		bool bAdd = true;
		for(size_t k = 0;k < c_size;k ++)
			if(text[i] == symbols[k])
			{
				bAdd = false;
				break;
			}
		if(bAdd)
		{
			text[idx] = text[i];
			idx ++;
		}
	}
	text[idx] = 0;
	s.resize(idx);
}

void remove_repeat(tstring & s, TCHAR c)
{
	LPTSTR text = (LPTSTR)s.c_str();
	size_t size = _tcslen(text);
	long idx = 0;
	bool bLastMatch = false;
	for(size_t i = 0;i < size;i ++)
	{
		if(!bLastMatch || text[i] != c)
		{
			text[idx] = text[i];
			idx ++;
		}
		bLastMatch = (text[i] == c);
	}			
	text[idx] = 0;
	s.resize(idx);
}
