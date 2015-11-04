//	std_str_ext.h
//
//

#ifndef __BASE_STR_STDSTREXT_H__
#define __BASE_STR_STDSTREXT_H__

#include <xfunctional>

namespace base
{
	inline int my_iswspace(int c){
		return ::iswspace((short)c);
	}
	// trim from start
	template<typename STR>
	STR &ltrim(STR &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(my_iswspace))));
		return s;
	}

	// trim from end
	template<typename STR>
	STR &rtrim(STR &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(my_iswspace))).base(), s.end());
		return s;
	}

	template<typename STR>
	STR &trim(STR &s) {
		return ltrim(rtrim(s));
	}

	// ASCII-specific tolower.  The standard library's tolower is locale sensitive,
	// so we don't want to use it here.
	template <class Char>
	inline Char ToLowerASCII(Char c) {
		return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
	}

	// ASCII-specific toupper.  The standard library's toupper is locale sensitive,
	// so we don't want to use it here.
	template <class Char>
	inline Char ToUpperASCII(Char c) {
		return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
	}
	// Converts the elements of the given string.  This version uses a pointer to
	// clearly differentiate it from the non-pointer variant.
	template <class str>
	inline void StringToLowerASCII(str* s) {
		for (typename str::iterator i = s->begin(); i != s->end(); ++i){
			*i = base::ToLowerASCII(*i);
		}
	}
	template <class str>
	inline str StringToLowerASCII(const str& s) {
		// for std::string and std::wstring
		str output(s);
		StringToLowerASCII(&output);
		return output;
	}

	// Converts the elements of the given string.  This version uses a pointer to
	// clearly differentiate it from the non-pointer variant.
	template <class str>
	inline void StringToUpperASCII(str* s) {
		for (typename str::iterator i = s->begin(); i != s->end(); ++i)
			*i = base::ToUpperASCII(*i);
	}

	template <class str>
	inline str StringToUpperASCII(const str& s) {
		// for std::string and std::wstring
		str output(s);
		StringToUpperASCII(&output);
		return output;
	}

	template <class str>
	void split(const str& src, const str& delim, std::vector<str>& ret)  
	{
		size_t last = 0;
		size_t index = src.find(delim, last);
		while (index!= str::npos)
		{
			ret.push_back(src.substr(last, index - last));
			last = index + delim.length();
			index = src.find(delim, last);
		}
		if (index - last > 0)
		{
			ret.push_back(src.substr(last, index - last));
		}
	}

	inline std::string WideToASCII(std::wstring wstr)
	{
		std::string out;
		int iLen = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
		if(iLen > 1)
		{
			out.resize(iLen);
			::WideCharToMultiByte(CP_ACP, 0,
				wstr.c_str(), out.size(),
				const_cast<char*>(out.data()), out.size(),
				NULL, NULL);
			out.resize(iLen-1);
		}
		return(out);
	}
	inline std::wstring ASCIIToWide(std::string str)
	{
		std::wstring out;
		int iLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		if(iLen > 1)
		{
			out.resize(iLen);
			::MultiByteToWideChar(CP_ACP, 0,
				str.c_str(), out.size(),
				const_cast<wchar_t*>(out.data()), out.size());
			out.resize(iLen-1);
		}
		return(out);
	}


} // namespace base

#endif // __BASE_STR_STDSTREXT_H__

