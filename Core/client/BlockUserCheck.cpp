#include "BlockUserCheck.h"
#include <tlhelp32.h>
#include <atlconv.h>
#include <Sddl.h>
#include <Wtsapi32.h>
#include <shlobj.h>
#include <algorithm>
#include <xfunctional>

#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Advapi32.lib")
#include "DefaultBlockRule.h"
#include "str/stdcxx.h"
#include "../parser/INIReader.h"
#include "../wmi/WmiQueryHelper.h"
#include "../string/base64.h"

BlockUserCheck* BlockUserCheck::instance_ = 0;
Locker BlockUserCheck::singletonLocker;
std::vector<std::wstring> g_VtBlockWnd;

namespace
{
	// 去除前后空格
	inline std::wstring trim(std::wstring& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun(::isspace))));
		str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(::isspace))).base(), str.end());
		return str;
	}

	// 字符串分割函数
	inline std::vector<std::wstring> split(std::wstring str, std::wstring pattern)
	{
		std::wstring::size_type pos;
		std::vector<std::wstring> result;
		str += pattern;
		unsigned int size = str.size();

		for (unsigned int i = 0; i < size; i++)
		{
			pos=str.find(pattern, i);
			if (pos < size)
			{
				std::wstring s = str.substr(i, pos-i);
				result.push_back(trim(s));
				i = pos + pattern.size() - 1;
			}
		}

		return result;
	}
}

BlockUserCheck::BlockUserCheck()
{
	std::string blockRule = base64::base64_decode(g_blockRule);

	INIReader iniReader(blockRule.c_str());
	std::string strBlockWnd = iniReader.Get("blockRule", "blockWnd", "");
	std::string strBlockProcess = iniReader.Get("blockRule", "blockProcess", "");
	std::string strBlockProgram = iniReader.Get("blockRule", "blockStartMenu", "");
	std::string strBlockStartMenu = iniReader.Get("blockRule", "blockStartMenu", "");
	std::string strblockDeskTop = iniReader.Get("blockRule", "blockDeskTop", "");
	std::string strBlockHardDisc = iniReader.Get("blockRule", "blockHardDisc", "");

	g_VtBlockWnd = split(base::stdcxx_s2ws(strBlockWnd), L",");
	m_VtBlockProcess = split(base::stdcxx_s2ws(strBlockProcess), L",");
	m_VtBlockProgram = split(base::stdcxx_s2ws(strBlockProgram), L",");
	m_VtBlockStartMenu = split(base::stdcxx_s2ws(strBlockStartMenu), L",");
	m_VtBlockDeskTop = split(base::stdcxx_s2ws(strblockDeskTop), L",");
	m_VtBlockHardDisc = split(base::stdcxx_s2ws(strBlockHardDisc), L",");
}

BlockUserCheck* BlockUserCheck::Instance()
{
	if (!instance_)
	{
		singletonLocker.Lock();
		if (!instance_)
		{
			instance_ = new BlockUserCheck;
		}

		singletonLocker.Unlock();
	}

	return instance_;
}


class WndEnumRes
{
public:
	HWND _hwnd;
	BlockRes* _blockRes;

	WndEnumRes() : _hwnd(NULL), _blockRes(NULL)
	{
	}
};

//To continue enumeration, the callback function must return TRUE; to stop enumeration, it must return FALSE.
BOOL CALLBACK EnumWindowProc(HWND hwnd,LPARAM lParam)
{
	WCHAR szText[MAX_PATH] = { 0 };
	GetWindowTextW(hwnd, szText, MAX_PATH );
	if (wcslen(szText) == 0) return TRUE;

	WndEnumRes* pRes = (WndEnumRes*)lParam;
	CharLowerW(szText);

	for (unsigned int i = 0; i < g_VtBlockWnd.size(); i++)
	{
		if (wcsstr(szText, g_VtBlockWnd[i].c_str())) 
		{ 
			pRes->_hwnd = hwnd;

			//统计
			if (pRes->_blockRes) pRes->_blockRes->set(i);

			return (NULL != pRes->_blockRes); 
		}
	}

	return TRUE;

}

BOOL BlockUserCheck::TestWndBlock(BlockRes* pRes/* = NULL*/)
{
	WndEnumRes res;
	res._blockRes = pRes;
	EnumWindows(EnumWindowProc, (LPARAM)&res);
	return res._hwnd ? TRUE : FALSE;
}

BOOL BlockUserCheck::TestProcessBlock(BlockRes* pRes/* = NULL*/)
{
	BOOL bReslut = FALSE;

	HANDLE hToolhelpSnapshot = NULL;
	BOOL bProcessRet = FALSE;
	PROCESSENTRY32 process32 = { 0 };

	hToolhelpSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	process32.dwSize = sizeof(PROCESSENTRY32);
	bProcessRet = Process32First(hToolhelpSnapshot, &process32);
	while(bProcessRet)
	{
		CharLowerW(process32.szExeFile);
		for (unsigned int i = 0; i < m_VtBlockProcess.size(); i++)
		{
			if (wcsstr(process32.szExeFile, m_VtBlockProcess[i].c_str()))
			{
				//统计
				if (pRes) pRes->set(i);

				bReslut = TRUE;
				break;
			}
		}

		//不需要统计并且已经找到，不用继续
		if (bReslut && !pRes) break;

		bProcessRet = Process32Next(hToolhelpSnapshot, &process32);
	}

	if (hToolhelpSnapshot)
		CloseHandle(hToolhelpSnapshot);

	return bReslut;
}

BOOL BlockUserCheck::TestProgramBlock(BlockRes* pRes/* = NULL*/)
{
	BOOL bReslut = FALSE;

	HKEY hKey,IndexKey;
	LONG KeyIndex = 0;
	DWORD IndexSize = 0;
	WCHAR KeyName[MAX_PATH] = {0};
	WCHAR IndexDisplay[MAX_PATH] = {0};
	WCHAR IndexKeyName[MAX_PATH] = {0};
	DWORD size = sizeof(KeyName) / sizeof(WCHAR);

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
		0, KEY_READ | KEY_WOW64_32KEY | KEY_ENUMERATE_SUB_KEYS | KEY_ENUMERATE_SUB_KEYS, &hKey))
		return FALSE;

	while (RegEnumKeyEx(hKey, KeyIndex, KeyName, &size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		swprintf_s(IndexKeyName, sizeof(IndexKeyName) / sizeof(WCHAR),
			L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%ws", KeyName);

		if (RegOpenKey(HKEY_LOCAL_MACHINE, IndexKeyName, &IndexKey) == ERROR_SUCCESS)
		{
			IndexSize = sizeof(IndexDisplay);
			RegQueryValueEx(IndexKey, L"DisplayName", NULL, NULL, (PBYTE)IndexDisplay, &IndexSize);
			CharLowerW(IndexDisplay);
			for (unsigned int i = 0; i < m_VtBlockProgram.size(); i++)
			{
				if (wcsstr(IndexDisplay, m_VtBlockProgram[i].c_str()))
				{
					//统计
					if (pRes) pRes->set(i);

					bReslut = TRUE;
					break;
				}
			}

			RegCloseKey(IndexKey);
		}

		//不需要统计并且已经找到，不用继续
		if (bReslut && !pRes) break;

		ZeroMemory(KeyName, sizeof(KeyName));
		size = sizeof(KeyName) / sizeof(WCHAR);
		KeyIndex++;
	}

	RegCloseKey(hKey);
	return bReslut;
}

BOOL BlockUserCheck::TestStartMenuBlock(BlockRes* pRes/* = NULL*/)
{
	WCHAR szPath[MAX_PATH] = { 0 };
	wstring msUserStartMenuPath;
	wstring msUserSid;
	vector<std::wstring> VtStartMenuList;

	BOOL LookupResult = FALSE;
	for(ULONG LookupIndex = 0; LookupIndex < 10; LookupIndex++)
	{
		if(LookupExplorerUserSid(msUserSid))
		{
			LookupResult = TRUE;
			break;
		}
		else
		{
			Sleep(1000);
		}
	}
	if(!LookupResult)
	{
		return (FALSE);
	}

	if(!GetCurrentUserFolder(msUserSid, msUserStartMenuPath, L"Start Menu"))
	{
		return FALSE;
	}

	EnumFolderFile(msUserStartMenuPath.c_str(), 0, VtStartMenuList);
	SHGetFolderPath(NULL, CSIDL_COMMON_STARTMENU, NULL, 0, szPath);
	EnumFolderFile(szPath, 0, VtStartMenuList);
	SHGetFolderPath(NULL, CSIDL_STARTMENU, NULL, 0, szPath);
	EnumFolderFile(szPath,0, VtStartMenuList);

	BOOL bReslut = FALSE;
	for (UINT i = 0; i < VtStartMenuList.size(); i++)
	{
		for (UINT j = 0; j < m_VtBlockStartMenu.size(); j++)
		{
			if(wcsstr(VtStartMenuList[i].c_str(), m_VtBlockStartMenu[j].c_str()))
			{
				//统计
				if (pRes) pRes->set(j);

				bReslut = TRUE;
				break;
			}
		}

		//不需要统计并且已经找到，不用继续
		if (bReslut && !pRes) break;
	}
	return bReslut;
}

BOOL BlockUserCheck::TestDeskTopBlock(BlockRes* pRes/* = NULL*/)
{
	wstring msUserDesktopPath;
	wstring msUserSid;
	vector<std::wstring> VtDeskTopList;

	BOOL LookupResult = FALSE;
	for (ULONG LookupIndex = 0; LookupIndex < 10; LookupIndex++)
	{
		if (LookupExplorerUserSid(msUserSid))
		{
			LookupResult = TRUE;
			break;
		}
		else
		{
			Sleep(1000);
		}
	}
	if (!LookupResult)
	{
		return (FALSE);
	}


	if (!GetCurrentUserFolder(msUserSid, msUserDesktopPath, L"Desktop"))
	{
		return FALSE;
	}

	EnumFolderFile(msUserDesktopPath.c_str(), 0, VtDeskTopList);

	BOOL bReslut = FALSE;
	for (UINT i = 0; i < VtDeskTopList.size(); i++)
	{
		for (UINT j = 0; j < m_VtBlockDeskTop.size(); j++)
		{
			if(wcsstr(VtDeskTopList[i].c_str(), m_VtBlockDeskTop[j].c_str()))
			{
				//统计
				if (pRes) pRes->set(j);

				bReslut = TRUE;
				break;
			}
		}

		//不需要统计并且已经找到，不用继续
		if (bReslut && !pRes) break;
	}
	return bReslut;
}

VOID BlockUserCheck::EnumFolderFile(LPCTSTR lpFolderPath, ULONG NumberOfRecursion, vector<std::wstring>& VtDeskTopList)
{
	ULONG	RecursionCount = 0;
	WIN32_FIND_DATA wfd = {0};
	HANDLE hFileHandle = NULL;
	WCHAR cFilePath[MAX_PATH] = { 0 };
	WCHAR cFindFile[MAX_PATH] = { 0 };

	if(NumberOfRecursion >= 10)
		return;

	swprintf_s(cFindFile, sizeof(cFindFile) / sizeof(WCHAR), L"%ws\\*.*", lpFolderPath);

	hFileHandle = FindFirstFile(cFindFile, &wfd);
	if (hFileHandle == INVALID_HANDLE_VALUE)
		return;

	do 
	{
		if (_wcsicmp(wfd.cFileName,L".")==0 || _wcsicmp(wfd.cFileName,L"..")==0
			|| _wcsicmp(wfd.cFileName,L"desktop.ini")==0 || _wcsicmp(wfd.cFileName,L"index.dat") == 0)
			continue;

		swprintf_s(cFilePath, sizeof(cFilePath) / sizeof(WCHAR), L"%ws\\%ws", lpFolderPath, wfd.cFileName);

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//警告，此递归可能导致栈溢出,所以限制最大次数为10次。
			RecursionCount = NumberOfRecursion;
			RecursionCount++;
			EnumFolderFile(cFilePath, RecursionCount, VtDeskTopList);
		}
		else
		{
			if (wcsstr(cFilePath, L".exe") || wcsstr(cFilePath, L".lnk"))
			{
				CharLowerW(cFilePath);
				VtDeskTopList.push_back(cFilePath);
			}
		}

	} while (FindNextFile(hFileHandle, &wfd));

	FindClose(hFileHandle);
	return;
}

BOOL BlockUserCheck::GetCurrentUserFolder(wstring& msUserSid, wstring& msFolder, LPCTSTR lpFolderName)
{
	HKEY hKey;
	LONG nReslut = 0;
	WCHAR szDeskTopPath[MAX_PATH] = { 0 };
	WCHAR szKeyRoot[MAX_PATH] = { 0 };

	swprintf_s(szKeyRoot,sizeof(szKeyRoot) / sizeof(WCHAR),
		L"%s\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", msUserSid.c_str());

	nReslut = RegOpenKeyEx(HKEY_USERS, szKeyRoot, 0, KEY_READ, &hKey);
	if(nReslut != ERROR_SUCCESS)
		return FALSE;

	DWORD dwSize = sizeof(szDeskTopPath);
	RegQueryValueEx(hKey, lpFolderName, NULL, NULL, (PBYTE)szDeskTopPath, &dwSize);
	msFolder = szDeskTopPath;
	RegCloseKey(hKey);

	return msFolder.size() ? TRUE : FALSE;
}

BOOL BlockUserCheck::LookupExplorerUserSid(wstring &msUserSid)
{
	BOOL bReslut = FALSE;
	DWORD dwCount = 0;
	SID_NAME_USE SidNameUse = SidTypeUser; 
	CHAR szUserName[128] = { 0 };
	DWORD dwSize = sizeof(szUserName);
	PWTS_PROCESS_INFO pi =  { 0 };

	if (WTSEnumerateProcesses(NULL, 0, 1, &pi, &dwCount))
	{
		for(unsigned int i = 0; i < dwCount; i++ )
		{
			if(_wcsicmp(pi[i].pProcessName, L"explorer.exe") == 0)
			{
				if(LookupAccountSidA(NULL, pi[i].pUserSid, szUserName, &dwSize, NULL, &dwSize, &SidNameUse))
				{
					PWCHAR pUserSid = (PWCHAR)LocalAlloc(LPTR, 128);
					ConvertSidToStringSid(pi[i].pUserSid, &pUserSid);
					msUserSid = pUserSid;
					LocalFree((HLOCAL)pUserSid);
					bReslut = TRUE;
					break;
				}
			}
		}
		if (pi)
		{
			WTSFreeMemory(pi);
		}
	}

	return bReslut;
}

BOOL BlockUserCheck::TestVMBlock(BlockRes* pRes/* = NULL*/)
{
	std::vector<std::wstring> vtDrivers;
	WmiQueryHelper querier;

	if (querier.ExecQuery(L"select * from Win32_DiskDrive", L"Caption", vtDrivers) && vtDrivers.size())
	{
		//MessageBox(NULL, vtDrivers[0].c_str(), L"HD Caption", MB_OK);
		std::transform(vtDrivers[0].begin(), vtDrivers[0].end(), vtDrivers[0].begin(), tolower);
		for (unsigned int i = 0; i < m_VtBlockHardDisc.size(); i++)
		{
			if (wcsstr(vtDrivers[0].c_str(), m_VtBlockHardDisc[i].c_str()))
			{
				//统计
				if (pRes) pRes->set(i);

				//MessageBox(NULL, m_VtBlockHardDisc[i].c_str(), L"Blocked!", MB_OK);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL BlockUserCheck::Check()
{
	BOOL bBlock = FALSE;
	try
	{
		bBlock = TestVMBlock() || TestWndBlock() || TestProcessBlock() || TestProgramBlock()
			|| TestStartMenuBlock() || TestDeskTopBlock();
	}
	catch (...)
	{
		return FALSE;
	}

	return bBlock;
}

BOOL BlockUserCheck::CheckAll(BlockRes& wndRes, BlockRes& procRes, BlockRes& destopRes, BlockRes& progRes, BlockRes& menuRes, BlockRes& vmRes)
{
	BOOL ret1 = TestVMBlock(&vmRes);
	BOOL ret2 = TestWndBlock(&wndRes);
	BOOL ret3 = TestProcessBlock(&procRes);
	BOOL ret4 = TestProgramBlock(&progRes);
	BOOL ret5 = TestStartMenuBlock(&menuRes);
	BOOL ret6 = TestDeskTopBlock(&destopRes);

	return (ret1 || ret2 || ret3 || ret4 || ret5 || ret6);
}
