#pragma once
#include <vector>
#include "../types.h"

class ProcessUtil
{
public:
	static DWORD FindProcess(LPCTSTR strProcessName);
	static BOOL KillProcess(LPCTSTR strProcessName);
	static BOOL IsEnableUAC(void);
	static BOOL StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine);
	static BOOL GetProcessList(std::vector<ProcessInfo>& list);
	static int GetServiceState(LPCTSTR lpszServ);
	static BOOL RunAsLogon(LPCTSTR lpszRun);
	static int CheckServStartMode(void);
	static BOOL RecoveryService(void);
};

