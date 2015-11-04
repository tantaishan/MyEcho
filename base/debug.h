// Copyright (c) 2012 Ohyo net work. All rights reserved.
// 2012/01/04
// debug.h
//
//

#ifndef __INCLUDE_DEBUG_MSG_H__
#define __INCLUDE_DEBUG_MSG_H__

#include "str_conv.inl"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>

#ifdef _WIN32
#	include <windows.h>
#	include <shlobj.h>
#	include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
#	include <locale.h>
#	include <string.h>
#endif


#ifndef BEGIN_NAMESPACE
#	define BEGIN_NAMESPACE(x) namespace x {
#endif

#ifndef END_NAMESPACE
#	define END_NAMESPACE(x) }
#endif

#ifndef DISALLOW_COPY_AND_ASSIGN
#	define DISALLOW_COPY_AND_ASSIGN(TypeName)	\
	TypeName(const TypeName&);					\
	void operator=(const TypeName&)
#endif


BEGIN_NAMESPACE(debug)


enum DBG_LEVEL {
	DBG_NONE		= 0x0000,
	DBG_DEBUG		= 0x0001,
	DBG_INFO		= 0x0002,
	DBG_WARNING		= 0x0004,
	DBG_ERROR		= 0x0008,
	DBG_EXCEPTION	= 0x0010,
};
__declspec(selectany) volatile unsigned long g_dwDbgLevel = static_cast<unsigned long>(DBG_DEBUG | DBG_INFO | DBG_WARNING | DBG_ERROR | DBG_EXCEPTION);

const char *const G_pwszDbgLevel[] = {
	"NONE",
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"EXCEPTION"
};


#ifndef __FUNCTION_W__
#	define WIDEN2(x) L ## x
#	define WIDEN(x) WIDEN2(x)
#	define __FUNCTION_W__ WIDEN(__FUNCTION__)
#endif



class dbg
{
private:
	DISALLOW_COPY_AND_ASSIGN(dbg);

protected:
	DBG_LEVEL	m_DbgLevel;
	const char	*pfile;
	std::ostringstream m_stream;

public:
	explicit dbg(DBG_LEVEL leval, const char *pszFile, int iLine)
	{
		m_DbgLevel = leval;
		m_stream << "[";
		switch(m_DbgLevel)
		{
		case DBG_DEBUG:
			m_stream << G_pwszDbgLevel[1];
			break;
		case DBG_INFO:
			m_stream << G_pwszDbgLevel[2];
			break;
		case DBG_WARNING:
			m_stream << G_pwszDbgLevel[3];
			break;
		case DBG_ERROR:
			m_stream << G_pwszDbgLevel[4];
			break;
		case DBG_EXCEPTION:
			m_stream << G_pwszDbgLevel[5];
			break;
		default:
			m_stream << G_pwszDbgLevel[0];
			break;
		}
		SYSTEMTIME st;
		::GetLocalTime(&st);
		char szTime[128] = {0};
		::wnsprintfA(szTime, _countof(szTime), "[%04u-%02u-%02u_%02u:%02u:%02u_%03u_%u] ",
			st.wYear, st.wMonth, st.wDay,
			st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, st.wDayOfWeek);
		m_stream << "] " << szTime << pszFile << ":" << iLine << " ";
	}
	~dbg()
	{
		if(m_DbgLevel & GetDbgLevel())
		{
			m_stream << std::endl;
			std::string str_newline(m_stream.str());

			//	用外部宏控制是否输出, 这句不要注释掉
			::OutputDebugStringA(str_newline.c_str());
			//std::ofstream	outfile(GetLogFileName(), std::ios::app);
			//outfile << str_newline;
			//std::cout << wstr_newline;
		}
		m_DbgLevel = DBG_NONE;
	}

	std::ostream& stream() { return m_stream; }

public:

#if defined(_WIN32)
	//
	//	这个类用于保留Last error值
	//
	// Stores the current value of GetLastError in the constructor and restores
	// it in the destructor by calling SetLastError.
	// This is useful since the LogMessage class uses a lot of Win32 calls
	// that will lose the value of GLE and the code that called the log function
	// will have lost the thread error value when the log call returns.
	class CSaveLastError {
	public:
		CSaveLastError() { m_dwLastErrorCode = ::GetLastError(); }
		~CSaveLastError() { ::SetLastError(m_dwLastErrorCode); }

		unsigned long GLE() const { return m_dwLastErrorCode; }

	protected:
		unsigned long m_dwLastErrorCode;
	};
	CSaveLastError m_last_error;
#endif

public:
	static unsigned long SetDbgLevel(unsigned long dwNewLevel)
	{
		unsigned long old = g_dwDbgLevel;
		g_dwDbgLevel = dwNewLevel;
		return (old);
	}
	static unsigned long GetDbgLevel(void)
	{
		return (g_dwDbgLevel);
	}
};



// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class VoidifyDebug {
public:
	VoidifyDebug() { }
	// This has to be an operator with a precedence lower than << but
	// higher than ?:
	void operator&(std::ostream&) { }
};

#define DBG_IS_ON(severity) \
	(severity & ::debug::g_dwDbgLevel)

#define DBG_LAZY_STREAM(stream, condition) \
	!(condition) ? (void) 0 : ::debug::VoidifyDebug() & (stream)

#define DBG_STREAM(severity) ::debug::dbg(severity, __FILE__, __LINE__).stream()



#ifdef _DEBUG
#	define DBG_IF(level, condition) DBG_LAZY_STREAM(DBG_STREAM(::debug::DBG_ ## level), DBG_IS_ON(::debug::DBG_ ## level) && (condition))
#	define DBG(level) DBG_LAZY_STREAM(DBG_STREAM(::debug::DBG_ ## level), DBG_IS_ON(::debug::DBG_ ## level))
#else
#	define DBG(level) DBG_LAZY_STREAM(DBG_STREAM(::debug::DBG_ ## level), (0))
#	define DBG_IF(level, condition) DBG_LAZY_STREAM(DBG_STREAM(::debug::DBG_ ## level), (0))
#endif



END_NAMESPACE(debug)


#endif	//  #ifndef __INCLUDE_DEBUG_MSG_H__
