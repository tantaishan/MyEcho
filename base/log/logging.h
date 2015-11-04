// Copyright (c) 2012 Ohyo net work. All rights reserved.
// 2012/01/04
// logging.h
//
//

#ifndef __INCLUDE_LOGGING_H__
#define __INCLUDE_LOGGING_H__

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


BEGIN_NAMESPACE(logging)


enum LOG_LEVEL {
	LOG_NONE		= 0,
	LOG_DEBUG		= 1	<< 0,
	LOG_INFO		= 1	<< 1,
	LOG_WARNING		= 1	<< 2,
	LOG_ERROR		= 1	<< 3,
	LOG_EXCEPTION	= 1	<< 4,

	_LOG_ALL_			= 0xFFFFFFFF
};

#if defined(_DEBUG)
__declspec(selectany) volatile unsigned long g_dwLogLevel = static_cast<unsigned long>(_LOG_ALL_);
#else
//	release版本 不记录任何信息.
__declspec(selectany) volatile unsigned long g_dwLogLevel = static_cast<unsigned long>(LOG_NONE);
#endif


const char *const G_pwszLogLevel[] = {
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



class log
{
private:
	DISALLOW_COPY_AND_ASSIGN(log);

protected:
	LOG_LEVEL	m_LogLevel;
	const char	*pfile;
	std::ostringstream m_stream;

public:
	explicit log(LOG_LEVEL leval, const char *pszFile, int iLine)
	{
		m_LogLevel = leval;
		m_stream << "[";
		switch(m_LogLevel)
		{
		case LOG_DEBUG:
			m_stream << G_pwszLogLevel[1];
			break;
		case LOG_INFO:
			m_stream << G_pwszLogLevel[2];
			break;
		case LOG_WARNING:
			m_stream << G_pwszLogLevel[3];
			break;
		case LOG_ERROR:
			m_stream << G_pwszLogLevel[4];
			break;
		case LOG_EXCEPTION:
			m_stream << G_pwszLogLevel[5];
			break;
		default:
			m_stream << G_pwszLogLevel[0];
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
	~log()
	{
		if(m_LogLevel & GetLogLevel())
		{
			m_stream << std::endl;
			std::string str_newline(m_stream.str());

			CheckLogDir();

			std::ofstream	outfile(GetLogFileName(), std::ios::app);
			outfile << str_newline;
			//std::cout << wstr_newline;
		}
		m_LogLevel = LOG_NONE;
	}

	std::ostream& stream() { return m_stream; }

private:
	static BOOL CheckLogDir(void)
	{
		wchar_t	wszLogFile[MAX_PATH];
		::StrCpyNW(wszLogFile, GetLogFileName(), _countof(wszLogFile));
		::PathAppendW(wszLogFile, L"..\\");

		if( ! ::PathFileExistsW(wszLogFile))//判断文件夹是否存在
		{
			::SHCreateDirectoryExW(NULL, wszLogFile, NULL);	//创建多级文件夹
		}
		return TRUE;
	}
public:

	static wchar_t* GetLogFileName(void)
#if defined(_MSC_VER)
	{
		static wchar_t	wszLogFile[MAX_PATH] = {0};
		if( ! wszLogFile[0])
		{
			LPITEMIDLIST	lpIdl = NULL;
			if(S_OK == ::SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &lpIdl) && lpIdl)
			{
				::SHGetPathFromIDListW(lpIdl, wszLogFile);
				::PathAppendW(wszLogFile, L"DouXie\\DxGame.log");
			}
			::CoTaskMemFree(static_cast<void*>(lpIdl));

		}
		return wszLogFile;
	}
#else
		//	TODO
#endif

#if defined(_MSC_VER)
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
	static unsigned long SetLogLevel(unsigned long dwNewLevel)
	{
		unsigned long old = g_dwLogLevel;
		g_dwLogLevel = dwNewLevel;
		return (old);
	}
	static unsigned long GetLogLevel(void)
	{
		return (g_dwLogLevel);
	}
};



// This class is used to explicitly ignore values in the conditional
// logging macros.  This avoids compiler warnings like "value computed
// is not used" and "statement has no effect".
class VoidifyLog {
public:
	VoidifyLog() { }
	// This has to be an operator with a precedence lower than << but
	// higher than ?:
	void operator&(std::ostream&) { }
};

#define LOG_IS_ON(severity) \
	(severity & ::logging::g_dwLogLevel)

#define LAZY_STREAM(stream, condition) \
	!(condition) ? (void) 0 : ::logging::VoidifyLog() & (stream)

#define LOG_STREAM(severity) ::logging::log(severity, __FILE__, __LINE__).stream()



#ifdef _NO_LOG_
#	define LOG(level) LAZY_STREAM(LOG_STREAM(::logging::LOG_ ## level), (0))
#	define LOG_IF(level, condition) LAZY_STREAM(LOG_STREAM(::logging::LOG_ ## level), (0))
#else
#	define LOG_IF(level, condition) LAZY_STREAM(LOG_STREAM(::logging::LOG_ ## level), LOG_IS_ON(::logging::LOG_ ## level) && (condition))
#	define LOG(level) LAZY_STREAM(LOG_STREAM(::logging::LOG_ ## level), LOG_IS_ON(::logging::LOG_ ## level))
#endif



_inline BOOL ResetLogFile(void)
{
	::DeleteFileW(::logging::log::GetLogFileName());

	SYSTEMTIME st;
	::GetLocalTime(&st);
	char szTime[128] = {0};
	::wnsprintfA(szTime, _countof(szTime), "[%04u-%02u-%02u_%02u:%02u:%02u_%03u_%u] ",
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, st.wDayOfWeek);

	LOG(INFO) << "Log file reset "<< szTime;
	return TRUE;
}

END_NAMESPACE(logging)


#endif	//  #ifndef __INCLUDE_LOGGING_H__
