#pragma once
#include "../types.h"
#include "../string/tstring.h"
#include <vector>

class SystemInfo
{
public:
	static tstring GetSystemVersion();
	static tstring GetIEVersion();
	static BOOL GetDiskPartition(std::vector<tstring>& drivers);
	static tstring GetUnixTimeStamp();
	static tstring GetMACAddress();
	static BOOL GetMemorySize(__int64* i64Total, __int64* i64Avail);
	static tstring GetSystemBit();
	static tstring GetDisplayPixel();
	static tstring GetFormatTime(LPCTSTR fmt);
	static tstring GetCpuInfo();
	static tstring GetSpVersion();
	static tstring GetBuildNumber();
	static BOOL GetNetworkAdapters(std::vector<NetworkAdapterInfo>& adapters);
	static BOOL GetDriversInfo(std::vector<DriverInfo>& drivers);
	static tstring GetDriverType(LPCTSTR szDriver);
	static tstring GetInstallDate();
	static tstring GetCurrTime(__in_opt LPCTSTR format = NULL);

};

