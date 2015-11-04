#include "ProcessUtil.h"
#include "../types.h"
#include <Psapi.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include "../base/process/launch.h"
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Psapi.lib ")
#include "../wmi/WmiQueryHelper.h"

DWORD ProcessUtil::FindProcess(LPCTSTR strProcessName)
{
	DWORD aProcesses[1024], cbNeeded, cbMNeeded;
	HMODULE hMods[1024];
	HANDLE hProcess;
	TCHAR szProcessName[MAX_PATH];
	if (!::EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 0;
	for (int i = 0; i < (int)(cbNeeded / sizeof(DWORD)); ++i)
	{
		hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
		::EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
		::GetModuleFileNameEx(hProcess, hMods[0], szProcessName,sizeof(szProcessName));
		if (_tcsstr(szProcessName, strProcessName))
			return(aProcesses[i]);
	}
	return 0;
}

BOOL ProcessUtil::KillProcess(LPCTSTR strProcessName)
{
	DWORD dwProcessId = FindProcess(strProcessName);
	if (dwProcessId == 0)
		return FALSE;
	HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, dwProcessId);
	if (hProcess == NULL)
		return FALSE;
	return ::TerminateProcess(hProcess, 0);
}

BOOL ProcessUtil::IsEnableUAC(void)
{
	BOOL bEnableUAC = FALSE;

	OSVERSIONINFO ovi = {0};
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	if (::GetVersionEx(&ovi))
	{
		// window vista or windows server 2008 or later operating system
		if ( ovi.dwMajorVersion > 5 )
		{
			HKEY	hKey = NULL;
			DWORD	dwType = REG_DWORD;
			DWORD	dwEnableLUA = 0;
			DWORD	dwSize = sizeof(DWORD);
			LSTATUS	lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\"),
				0, KEY_READ | KEY_WOW64_64KEY, &hKey);
			if( ERROR_SUCCESS == lRet )
			{
				lRet = ::RegQueryValueEx(hKey, _T("EnableLUA"), NULL, &dwType, (BYTE*)&dwEnableLUA, &dwSize);
				::RegCloseKey(hKey);

				if( ERROR_SUCCESS == lRet )
				{
					bEnableUAC = (dwEnableLUA) ? TRUE : FALSE;
				}
			}
		}
	}

	return bEnableUAC;
}

BOOL ProcessUtil::StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine)
{
	BOOL bRun = FALSE;
	if (!IsUserAnAdmin() && IsEnableUAC())
	{
		SHELLEXECUTEINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(si);
		si.lpVerb = _T("runas");
		si.lpFile = szExecutable;
		si.lpParameters = szCmdLine;
		si.nShow = SW_SHOWNORMAL;
		bRun = ShellExecuteEx(&si);
	}
	else
	{
		TCHAR szLaunch[MAX_PATH] = { 0 };
		_stprintf_s(szLaunch, _T("\"%s\" %s"), szExecutable, szCmdLine);
		base::LaunchOptions lo;
		bRun = base::LaunchProcess(szLaunch, lo, 0) ? TRUE : FALSE;
	}

	return bRun;
}

BOOL ProcessUtil::GetProcessList(std::vector<ProcessInfo>& list)
{
	BOOL bRet = TRUE;
	DWORD aProcesses[1024] = {0}, cbNeeded = 0, cbMNeeded = 0;
	HMODULE hMods[1024];
	HANDLE hProcess = NULL;
	TCHAR szProcessName[MAX_PATH] = _T("<unknown>");
	TCHAR  szProcessPath[MAX_PATH] = _T("<unknown>");

	if (!EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded )) return FALSE;
	for (int i = 0; i < (int)(cbNeeded / sizeof(DWORD)); i++)
	{
		ProcessInfo proc;
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
		//if (IsSysProcess(hProcess)) continue;
		bRet = EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbMNeeded);
		GetModuleBaseName(hProcess, hMods[0], szProcessName, sizeof(szProcessName));
		GetModuleFileNameEx(hProcess, hMods[0], szProcessPath, sizeof(szProcessName));
		proc.dwProcessID = aProcesses[i];
		proc.strProcName = szProcessName;
		proc.strProcPath = szProcessPath;
		list.push_back(proc);
	}

	return bRet;
}

int ProcessUtil::GetServiceState(LPCTSTR lpszServ)
{
	std::vector<std::wstring> vtProcess;
	WmiQueryHelper querier;
	if (querier.ExecQuery(L"select * from Win32_Process", L"Caption", vtProcess) && vtProcess.size())
	{
		for (unsigned int i = 0; i < vtProcess.size(); i++) {
			if (!_wcsicmp(vtProcess[i].c_str(), lpszServ))
				return SERVICESTATE::ACTIVATED;
		}

	} else {
		return SERVICESTATE::FAILED;
	}

	std::vector<std::wstring> vtService;
	if (querier.ExecQuery(L"select * from Win32_Service", L"Name", vtService) && vtService.size())
	{
		for (unsigned int i = 0; i < vtService.size(); i++) {
			if (!_wcsicmp(vtService[i].c_str(), lpszServ))
				return SERVICESTATE::INACTIVE;
		}
	} else {
		return SERVICESTATE::FAILED;
	}

	return SERVICESTATE::NOTEXIST;
}

BOOL ProcessUtil::RunAsLogon(LPCTSTR lpszRun)
{

	TCHAR szProcName[] = _T("EXPLORER.EXE");
	base::LaunchOptions lo;
	base::GetTokenByName(lo.as_user, szProcName);
#ifdef _UNICODE
	bool bRet = base::LaunchProcess((const wchar_t*)lpszRun, lo, 0);
#else
	bool bRet = base::LaunchProcess(base::stdcxx_s2ws(lpszRun), lo, 0);
#endif
	::CloseHandle(lo.as_user);
	lo.as_user = NULL;
	if (!bRet)
#ifdef _UNICODE
		bRet = base::LaunchProcess((const wchar_t*)lpszRun, lo, 0);
#else
		bRet = base::LaunchProcess(base::stdcxx_s2ws(lpszRun), lo, 0);
#endif
	return bRet;
}

// 查询服务启动方式
int ProcessUtil::CheckServStartMode(void)
{
	std::vector<std::wstring> vtService;
	WmiQueryHelper querier;
	PWCHAR pcSqlW = L"select * from Win32_Service where Name = 'CoolKanSvc'";
	if (querier.ExecQuery(pcSqlW, L"StartMode", vtService) && vtService.size()) {
		if (!_wcsicmp(vtService[0].c_str(), L"Manual"))			// 手动
			return SERVICE_STARTMODE::MANUAL;
		else if (!_wcsicmp(vtService[0].c_str(), L"Auto"))		// 自动
			return SERVICE_STARTMODE::AUTO;
		else if (!_wcsicmp(vtService[0].c_str(), L"Disabled"))	// 禁用
			return SERVICE_STARTMODE::DISABLED;
		else									// 未知
			return SERVICE_STARTMODE::UNKNOWN;
	}
	else {
		return SERVICE_STARTMODE::FAILED;		// 查询失败
	}
}

// 修复服务
BOOL ProcessUtil::RecoveryService(void)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szCmdline[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	::PathRemoveFileSpec(szPath);
	::PathAppend(szPath, _T("CoolKan_Svc.exe"));
	return StartElevatedProcess(szPath, L"-r");
}
