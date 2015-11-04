#include "SystemInfo.h"
#include <iptypes.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

tstring SystemInfo::GetSystemVersion()
{
	OSVERSIONINFO os;
	ZeroMemory(&os, sizeof(os));
	os.dwOSVersionInfoSize = sizeof(os);
	TCHAR szVersion[MAX_PATH] = { 0 };
	if (::GetVersionEx(&os))
	{
		_stprintf_s(szVersion, _countof(szVersion),
			_T("%d.%d"), os.dwMajorVersion, os.dwMinorVersion);
	}
	return szVersion;
}

tstring SystemInfo::GetIEVersion()
{
	TCHAR szVersion[MAX_PATH] = { 0 };
	HKEY hKey = NULL;
	LONG lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Internet Explorer"), 0, KEY_READ, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		DWORD dwSize = _countof(szVersion);
		::RegQueryValueEx(hKey, _T("Version"), 0, NULL, (PBYTE)szVersion, &dwSize);
		::RegCloseKey(hKey);
	}
	return szVersion;
}

BOOL SystemInfo::GetDiskPartition(std::vector<tstring>& drivers)
{
	DWORD dwSize = ::GetLogicalDriveStrings(0, NULL);
	PTSTR lpBuff = (PTSTR)::HeapAlloc(GetProcessHeap(), 0, dwSize * sizeof(TCHAR));
	PTSTR lpBegin = lpBuff;
	if (!::GetLogicalDriveStrings(dwSize, lpBuff))
		return FALSE;
	do 
	{
		drivers.push_back(lpBuff);
		lpBuff += (lstrlen(lpBuff) + 1);
	} while (*lpBuff != '\x00');

	::HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, lpBegin);
	return TRUE;
}

tstring SystemInfo::GetUnixTimeStamp()
{
	FILETIME ft = { 0 };
	SYSTEMTIME st = { 0 };
	ULARGE_INTEGER ull = { 0 };
	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, &ft);
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	__int64 ts = (ull.QuadPart - 116444736000000000ULL) / 10000000ULL;
	TCHAR szTime[MAX_PATH] = { 0 };
	_i64tot_s(ts, szTime, _countof(szTime), 10);
	return szTime;
}

tstring SystemInfo::GetMACAddress()
{
	TCHAR szMac[MAX_PATH] = { 0 };
	do 
	{
		PIP_ADAPTER_INFO pInfo = NULL;
		DWORD dwInfoSize = 0;
		DWORD dwRet = ::GetAdaptersInfo(NULL, &dwInfoSize);
		if ((dwRet != 0) && (dwRet != ERROR_BUFFER_OVERFLOW))
			break;
		pInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, dwInfoSize);
		if (!pInfo) break;
		if (::GetAdaptersInfo(pInfo, &dwInfoSize))
		{
			::GlobalFree(pInfo);
			break;
		}
		_stprintf_s(szMac, _countof(szMac), _T("%02X%02X%02X%02X%02X%02X"),
			pInfo->Address[0], pInfo->Address[1], pInfo->Address[2],
			pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);

	} while (FALSE);

	return szMac;
}

BOOL SystemInfo::GetMemorySize(__int64* i64Total, __int64* i64Avail)
{
	MEMORYSTATUSEX ms;
	ZeroMemory(&ms, sizeof(ms));
	ms.dwLength = sizeof(ms);
	if (::GlobalMemoryStatusEx(&ms))
	{
		const int kMegabyte = 1024 * 1024;
		*i64Total = ms.ullTotalPhys / kMegabyte;
		*i64Avail = ms.ullAvailPhys / kMegabyte;
		return TRUE;
	}
	return FALSE;
}

tstring SystemInfo::GetSystemBit()
{
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	::GetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
		|| si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
		return _T("64");
	else
		return _T("32");
}

tstring SystemInfo::GetDisplayPixel()
{
	TCHAR tsz[MAX_PATH] = { 0 };
	_stprintf_s(tsz, MAX_PATH,
		_T("%d*%d"), ::GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	return tsz;
}

tstring SystemInfo::GetFormatTime(LPCTSTR fmt)
{
	time_t t;
	time(&t);
	tm time = { 0 };
	localtime_s(&time, &t);
	//"2011-07-18 23:03:01 ";
	if (fmt == NULL)
		fmt = _T("%Y-%m-%d %H:%M:%S");
	TCHAR tsz[MAX_PATH] = { 0 };
	_tcsftime(tsz, MAX_PATH, fmt, &time);
	return tsz;
}

tstring SystemInfo::GetCpuInfo()
{
	tstring strCpu;
	HKEY hKey;
	const int kBuffSize = MAX_PATH;
	TCHAR szCpuInfo[kBuffSize];
	DWORD szCpuFre = 0;
	DWORD dwBuffSize = kBuffSize;

	LONG lRet = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_QUERY_VALUE, &hKey);
	if (lRet != ERROR_SUCCESS) {
		strCpu = _T("Unknown");
	} else {
		lRet = ::RegQueryValueEx(hKey, _T("ProcessorNameString"), NULL, NULL,
			(LPBYTE)szCpuInfo, &dwBuffSize);
		if ((lRet != ERROR_SUCCESS) || (dwBuffSize > kBuffSize)) {
			strCpu = _T("Unknown");
		} else {
			strCpu = szCpuInfo;
			lRet = ::RegQueryValueEx(hKey, _T("~MHz"), NULL, NULL, (LPBYTE) &szCpuFre, &dwBuffSize);
		}
		::RegCloseKey(hKey);
	}

	return strCpu;
}

tstring SystemInfo::GetSpVersion()
{
	OSVERSIONINFOEX os;
	ZeroMemory(&os, sizeof(os));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	TCHAR szVer[MAX_PATH] = { 0 };
	if (::GetVersionEx((OSVERSIONINFO*) & os)) {
		_stprintf_s(szVer, MAX_PATH,
			_T("%d.%d"), os.wServicePackMajor, os.wServicePackMinor);
	}
	return szVer;
}

tstring SystemInfo::GetBuildNumber()
{
	OSVERSIONINFOEX os;
	ZeroMemory(&os, sizeof(os));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	TCHAR szVer[MAX_PATH] = { 0 };
	if (::GetVersionEx((OSVERSIONINFO*) & os)) {
		_stprintf_s(szVer,
			MAX_PATH, _T("%d"), os.dwBuildNumber);
	}
	return szVer;
}

BOOL SystemInfo::GetNetworkAdapters(std::vector<NetworkAdapterInfo>& adapters)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD AdapterInfoSize;
	DWORD Err;

	AdapterInfoSize = 0;
	Err = GetAdaptersInfo(NULL, &AdapterInfoSize);

	if((Err != 0) && (Err != ERROR_BUFFER_OVERFLOW)) return FALSE;

	pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, AdapterInfoSize);

	if (pAdapterInfo == NULL) return FALSE;

	if (GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize) != 0)
	{
		GlobalFree(pAdapterInfo);  
		return FALSE;
	}

	while (pAdapterInfo)
	{
		NetworkAdapterInfo adapter;
		std::vector<_IpAddressInfo> ipList;
		adapter.adapterName = pAdapterInfo->AdapterName;
		adapter.description = pAdapterInfo->Description;

		switch (pAdapterInfo->Type)
		{
		case MIB_IF_TYPE_OTHER:
			adapter.type = "OTHER";
			break;
		case MIB_IF_TYPE_ETHERNET:
			adapter.type = "ETHERNET";
			break;
		case MIB_IF_TYPE_TOKENRING:
			adapter.type = "TOKENRING";
			break;
		case MIB_IF_TYPE_FDDI:
			adapter.type = "FDDI";
			break;
		case MIB_IF_TYPE_PPP:
			adapter.type = "PPP";
			break;
		case MIB_IF_TYPE_LOOPBACK:
			adapter.type = "LOOPBACK";
			break;
		case MIB_IF_TYPE_SLIP:
			adapter.type = "SLIP";
			break;
		default:
			break;
		}

		TCHAR szMac[MAX_PATH] = {0};
		_stprintf_s(szMac, _T("%02X%02X%02X%02X%02X%02X"),
			pAdapterInfo->Address[0], pAdapterInfo->Address[1],
			pAdapterInfo->Address[2], pAdapterInfo->Address[3],
			pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

		adapter.address = szMac;

		IP_ADDR_STRING* pIpAddrString = &(pAdapterInfo->IpAddressList);
		do 
		{
			IpAddressInfo ipInfo;
			ipInfo.ipAddress = pIpAddrString->IpAddress.String;
			ipInfo.ipMask = pIpAddrString->IpMask.String;
			ipInfo.gateway = pAdapterInfo->GatewayList.IpAddress.String;
			pIpAddrString=pIpAddrString->Next;
			ipList.push_back(ipInfo);

		} while (pIpAddrString);

		adapter.ipList = ipList;
		adapters.push_back(adapter);
		pAdapterInfo = pAdapterInfo->Next;
	}

	return TRUE;
}

BOOL SystemInfo::GetDriversInfo(std::vector<DriverInfo>& drivers)
{
	std::vector<std::wstring> drivs;
	GetDiskPartition(drivs);
	
	for (unsigned int i = 0; i < drivs.size(); i++)
	{
		TCHAR szNameBuffer[MAX_PATH] = {0};
		TCHAR szFileSysName[MAX_PATH] = {0};
		DWORD szSerialNumber = 0;
		ULARGE_INTEGER freeSpace = { 0 };
		ULARGE_INTEGER totalSpace = { 0 };
		ULARGE_INTEGER userSpace = { 0 };
		//如果软驱内没有软盘，总会弹出一个对话框，提示要插入软盘，这个函数禁用它
		UINT nPreErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
		BOOL ret = ::GetDiskFreeSpaceEx(drivs[i].c_str(), &userSpace, &totalSpace, &freeSpace);
		::SetErrorMode(nPreErrorMode);
		if (ret)
		{
			::GetVolumeInformation(drivs[i].c_str(), szNameBuffer, MAX_PATH, &szSerialNumber, NULL, 0, szFileSysName, MAX_PATH);
		}

		const DOUBLE kGiga = 1024 * 1024 * 1024;
		double dTotal = (double)(totalSpace.QuadPart) / kGiga;
		double dUse = (double)(userSpace.QuadPart) / kGiga;
		double dFree = (double)(freeSpace.QuadPart) / kGiga;

		DriverInfo drv;
		drv.strPartition = drivs[i];
		drv.dwSerialNumber = szSerialNumber;
		drv.strFileSysName = GetDriverType(drivs[i].c_str()) + _T(" : ") + szFileSysName;
		drv.totalSpace = dTotal;
		drv.userSpace = dUse;
		drv.freeSpace = dFree;

		drivers.push_back(drv);
	}

	return TRUE;
}

tstring SystemInfo::GetDriverType(LPCTSTR szDriver)
{
	tstring str = _T("Unknown");
	if (!szDriver) return str;

	UINT type = GetDriveType(szDriver);
	switch (type)
	{
	case DRIVE_FIXED: str = _T("Fixed"); break;
	case DRIVE_REMOVABLE: str = _T("Removable"); break;
	case DRIVE_CDROM: str = _T("CDRom"); break;
	case DRIVE_RAMDISK: str = _T("RamDisk"); break;
	default: break;
	}

	return str;
}

tstring SystemInfo::GetInstallDate()
{
	HKEY hKey = NULL;
	TCHAR tsz[MAX_PATH] = { 0 };
	DWORD dwSize = sizeof(tsz);
	
	LONG lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, KEY_QUERY_VALUE|KEY_WOW64_64KEY, &hKey);
	if (lRet != ERROR_SUCCESS)
		return tsz;
	lRet = ::RegQueryValueEx(hKey, _T("InstallDate"), 0, NULL, (PBYTE)tsz, &dwSize);
	if (ERROR_SUCCESS == lRet)
	{
		DWORD dwTime = 0;
		::memcpy(&dwTime, tsz, sizeof(DWORD));
		_stprintf_s(tsz, _T("%d"), dwTime);
	}
	::RegCloseKey(hKey);

	return tsz;
}

std::wstring SystemInfo::GetCurrTime(__in_opt LPCTSTR format)
{
	time_t t;
	time(&t);
	tm time = { 0 };
	localtime_s(&time, &t);
	//"2011-07-18 23:03:01 ";
	if (format == NULL)
		format = _T("%Y-%m-%d %H:%M:%S");
	WCHAR str[MAX_PATH] = { 0 };
	_tcsftime(str, MAX_PATH, format, &time);
	return str;
}
