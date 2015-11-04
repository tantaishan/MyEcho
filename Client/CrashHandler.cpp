#include "StdAfx.h"
#include "CrashHandler.h"
#include "../base/process/launch.h"
#include "../Core/json/json.h"
#include "FileMgr.h"
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

const TCHAR kBugReportExe[] = _T("BugReport.exe");
CrashHandler* CrashHandler::instance = 0;
Locker CrashHandler::singletonLocker;

CrashInfo GetCrashInfo(const EXCEPTION_RECORD* pRecord)
{
	CrashInfo crashInfo;
	strcpy_s(crashInfo.Address, MAX_ADDRESS_LENGTH, "N/A");
	strcpy_s(crashInfo.ErrorCode, MAX_ADDRESS_LENGTH, "N/A");
	strcpy_s(crashInfo.Flags, MAX_ADDRESS_LENGTH, "N/A");

	sprintf_s(crashInfo.Address, "%08X", pRecord->ExceptionAddress);
	sprintf_s(crashInfo.ErrorCode, "%08X", pRecord->ExceptionCode);
	sprintf_s(crashInfo.Flags, "%08X", pRecord->ExceptionFlags);

	return crashInfo;
}

std::vector<CallStackInfo> GetCallStack(const CONTEXT* pContext)
{
	HANDLE hProcess = GetCurrentProcess();
	SymInitialize(hProcess, NULL, TRUE);
	vector<CallStackInfo> arrCallStackInfo;
	CONTEXT c = *pContext;

	STACKFRAME64 sf;
	memset(&sf, 0, sizeof(STACKFRAME64));
	DWORD dwImageType = IMAGE_FILE_MACHINE_I386;
#ifdef _M_IX86
	sf.AddrPC.Offset = c.Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
#elif _M_X64
	dwImageType = IMAGE_FILE_MACHINE_AMD64;
	sf.AddrPC.Offset = c.Rip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.Rsp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.Rsp;
	sf.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	dwImageType = IMAGE_FILE_MACHINE_IA64;
	sf.AddrPC.Offset = c.StIIP;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = c.IntSp;
	sf.AddrFrame.Mode = AddrModeFlat;
	sf.AddrBStore.Offset = c.RsBSP;
	sf.AddrBStore.Mode = AddrModeFlat;
	sf.AddrStack.Offset = c.IntSp;
	sf.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

	HANDLE hThread = GetCurrentThread();
	while (true)
	{
		if (!StackWalk64(dwImageType, hProcess, hThread, &sf, &c,
			NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			break;
		if (sf.AddrFrame.Offset == 0)
			break;

		CallStackInfo callstackinfo;
		strcpy_s(callstackinfo.MethodName, MAX_NAME_LENGTH, "N/A");
		strcpy_s(callstackinfo.FileName, MAX_NAME_LENGTH, "N/A");
		strcpy_s(callstackinfo.ModuleName, MAX_NAME_LENGTH, "N/A");
		strcpy_s(callstackinfo.LineNumber, MAX_NAME_LENGTH, "N/A");

		BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH];
		IMAGEHLP_SYMBOL64 *pSymbol = (IMAGEHLP_SYMBOL64*)symbolBuffer;
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_NAME_LENGTH);

		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = MAX_NAME_LENGTH;

		DWORD symDisplacement = 0;

		// 得到函数名  
		if (SymGetSymFromAddr64(hProcess, sf.AddrPC.Offset, NULL, pSymbol))  
			strcpy_s(callstackinfo.MethodName, MAX_NAME_LENGTH, pSymbol->Name);  

		IMAGEHLP_LINE64 lineInfo;
		memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));
		lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD dwLineDisplacement;

		// 得到文件名和所在的代码行
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
		{
			strcpy_s(callstackinfo.FileName, MAX_NAME_LENGTH, lineInfo.FileName);
			sprintf_s(callstackinfo.LineNumber, "%d", lineInfo.LineNumber);
		}

		IMAGEHLP_MODULE64 moduleInfo;
		memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));

		moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

		// 得到模块名
		if (SymGetModuleInfo64(hProcess, sf.AddrPC.Offset, &moduleInfo))
		{
			strcpy_s(callstackinfo.ModuleName, MAX_NAME_LENGTH, moduleInfo.ModuleName);
		}

		arrCallStackInfo.push_back(callstackinfo);
	}

	SymCleanup(hProcess);
	return arrCallStackInfo;
}

CrashHandler* CrashHandler::getInstance()
{
	if (!instance)
	{
		singletonLocker.Lock();
		if (!instance)
		{
			instance = new CrashHandler;
		}
		singletonLocker.Unlock();
	}

	return instance;
}

CrashHandler::CrashHandler(void) {}
CrashHandler::~CrashHandler(void) {}

void CrashHandler::handleUncaughtException(BOOL bNeedHandle/* = FALSE*/)
{
	_previousExceptionFilter = ::SetUnhandledExceptionFilter(bNeedHandle ? topLevelExceptionFilter : unHandleExceptionFilter);
}

static BOOL saveMiniDump(EXCEPTION_POINTERS* pExceptionInfos)
{
	BOOL result = FALSE;
#ifdef _M_IX86
	if (pExceptionInfos->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)  
	{  
		static char TempStack[1024 * 128];
		__asm mov eax,offset TempStack[1024 * 128];
		__asm mov esp,eax;
	}
#endif

	CrashInfo crashInfo = GetCrashInfo(pExceptionInfos->ExceptionRecord);
	vector<CallStackInfo> arrCallStackInfo = GetCallStack(pExceptionInfos->ContextRecord);
	CString dumpPath = FileMgr::GetAppPath() + _T("\\MyEcho.dmp");

	Json::Value jDump, jCrash, jCallStack;
	CHAR szObject[MAX_PATH];
	::GetModuleFileNameA(NULL, szObject, MAX_PATH);
	jCrash["Object"] = szObject;
	jCrash["ErrorCode"] = crashInfo.ErrorCode;
	jCrash["Address"] = crashInfo.Address;
	jCrash["Flags"] = crashInfo.Flags;

	for (vector<CallStackInfo>::iterator i = arrCallStackInfo.begin(); i != arrCallStackInfo.end(); ++i)
	{
		CallStackInfo callstackinfo = (*i);
		jCallStack[callstackinfo.MethodName] = std::string("[") + callstackinfo.ModuleName + std::string("] [File:") + callstackinfo.FileName + std::string(" @Line ") + callstackinfo.LineNumber + std::string("]");
		//cout << callstackinfo.MethodName << "() : [" << callstackinfo.ModuleName << "] (File: " << callstackinfo.FileName << " @Line " << callstackinfo.LineNumber << ")" << endl;  
	}

	jDump["crash"] = jCrash;
	jDump["callstack"] = jCallStack;

	// 程序崩溃时，将写入程序目录下的MyDump.dmp文件
	HANDLE hFile = ::CreateFile(dumpPath, GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL );
	if (hFile != INVALID_HANDLE_VALUE)
	{
		/*
		MINIDUMP_EXCEPTION_INFORMATION exptInfo;
		exptInfo.ThreadId = ::GetCurrentThreadId();
		exptInfo.ExceptionPointers = pExceptionInfos;

		result = ::MiniDumpWriteDump(::GetCurrentProcess(),
			::GetCurrentProcessId(),
			hFile, MiniDumpNormal,
			&exptInfo, NULL, NULL);
		if (!result)
		{
			TCHAR temp[1024] = {0};
			_stprintf(temp, _T("GetLastError() 2 = %d"), GetLastError());
			OutputDebugString(temp);
		} */

		DWORD dwWritten = 0;
		WriteFile(hFile, jDump.toStyledString().c_str(),
			jDump.toStyledString().length(), &dwWritten, NULL);
		::CloseHandle(hFile);
	}
	else
	{
		TCHAR temp[1024] = {0};
		_stprintf(temp, _T("GetLastError() 1 = %d"), GetLastError());
		OutputDebugString(temp);
	}

	return result;
}

static LONG onUncaughtException(EXCEPTION_POINTERS* pExceptionInfos)
{
	LONG ret = EXCEPTION_CONTINUE_SEARCH;

	try
	{
		//保存异常信息
		saveMiniDump(pExceptionInfos);

		//启动报错程序
// 		BOOL bMainProcess = TRUE;
// 
// 		TCHAR szExePath[MAX_PATH] = {0};
// 		GetModuleFileName(NULL, szExePath, MAX_PATH);
// 		PathRemoveFileSpec(szExePath);
// 		TCHAR szCmdLine[MAX_PATH] = {0};
// 		_stprintf_s(szCmdLine, _T("\"%s\\%s\"%s"), szExePath, kBugReportExe,
// 			bMainProcess ? _T(" background") : _T(""));
// 
// 		base::LaunchOptions lo;
// 		if (base::LaunchProcess(szCmdLine,lo,0))
// 			ret = EXCEPTION_EXECUTE_HANDLER;
	}
	catch (...)
	{
	}

	return ret;
}

LONG WINAPI  topLevelExceptionFilter(EXCEPTION_POINTERS *pExceptionInfos)  
{
	return onUncaughtException(pExceptionInfos);
}

LONG WINAPI  unHandleExceptionFilter(EXCEPTION_POINTERS *pExceptionInfos)
{
	return EXCEPTION_EXECUTE_HANDLER;
}