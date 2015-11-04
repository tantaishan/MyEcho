#pragma once

//////////////////////////////////////////////////////////////////////////
#include "../Core/thread/Locker.h"

const int MAX_ADDRESS_LENGTH = 32;
const int MAX_NAME_LENGTH = 1024;

// Crash info.
typedef struct _CrashInfo
{
	CHAR ErrorCode[MAX_ADDRESS_LENGTH];
	CHAR Address[MAX_ADDRESS_LENGTH];
	CHAR Flags[MAX_ADDRESS_LENGTH];
} CrashInfo;

// Callstack info.
typedef struct _CallStackInfo
{
	CHAR ModuleName[MAX_NAME_LENGTH];
	CHAR MethodName[MAX_NAME_LENGTH];
	CHAR FileName[MAX_NAME_LENGTH];
	CHAR LineNumber[MAX_NAME_LENGTH];
} CallStackInfo;

//////////////////////////////////////////////////////////////////////////

class CrashHandler
{
public:
	friend LONG WINAPI  topLevelExceptionFilter(EXCEPTION_POINTERS *pExceptionInfos);
	friend LONG WINAPI  unHandleExceptionFilter(EXCEPTION_POINTERS *pExceptionInfos);

public:
	static CrashHandler* getInstance();

public:
	void handleUncaughtException(BOOL bNeedHandle = FALSE);

private:
	LPTOP_LEVEL_EXCEPTION_FILTER _previousExceptionFilter;

private:
	CrashHandler(void);
	~CrashHandler(void);
	static CrashHandler* instance;
	static Locker singletonLocker;
};

