#include "SCMHandler.h"
#include "../types.h"


#ifdef _UNICODE
#define T_FLAG		"W"
#else
#define T_FLAG		"A"
#endif

#define T_API(_N_,_E_)		_N_ ## _E_
#define OpenSCManager_APIName		T_API("OpenSCManager",T_FLAG)
#define OpenService_APIName			T_API("OpenService",T_FLAG)
#define StartService_APIName		T_API("StartService",T_FLAG)
#define QueryServiceConfig_APIName  T_API("QueryServiceConfig",T_FLAG)
#define CloseServiceHandle_APIName	"CloseServiceHandle"


namespace scm
{
	typedef SC_HANDLE (WINAPI* OpenSCManagerType)(LPCSTR,LPCTSTR,DWORD);
	typedef SC_HANDLE (WINAPI* OpenServiceType)(SC_HANDLE,LPCTSTR,DWORD);
	typedef BOOL (WINAPI* StartServiceType)(SC_HANDLE,DWORD,LPCTSTR *);
	typedef BOOL (WINAPI* QueryServiceConfigType)(SC_HANDLE,LPQUERY_SERVICE_CONFIG,DWORD,LPDWORD);
	typedef BOOL (WINAPI* CloseServiceHandleType)(SC_HANDLE);

	struct NC_HideApi
	{
		OpenSCManagerType	OpenSCManager;
		OpenServiceType		OpenService;
		StartServiceType	StartService;
		QueryServiceConfigType	QueryServiceConfig;
		CloseServiceHandleType	CloseServiceHandle;
	};

	bool InitHideApi(NC_HideApi * HideApi)
	{
		HMODULE hAdvapi32 = LoadLibrary(TEXT("Advapi32.dll"));
		bool Result = false;
		while(hAdvapi32!=NULL)
		{
			HideApi->OpenSCManager = (OpenSCManagerType)GetProcAddress(hAdvapi32,OpenSCManager_APIName);
			if(NULL==HideApi->OpenSCManager)
				break;
			HideApi->OpenService = (OpenServiceType)GetProcAddress(hAdvapi32,OpenService_APIName);
			if(NULL==HideApi->OpenService)
				break;
			HideApi->StartService = (StartServiceType)GetProcAddress(hAdvapi32,StartService_APIName);
			if(NULL==HideApi->StartService)
				break;
			HideApi->QueryServiceConfig = (QueryServiceConfigType)GetProcAddress(hAdvapi32,QueryServiceConfig_APIName);
			if(NULL==HideApi->QueryServiceConfig)
				break;
			HideApi->CloseServiceHandle = (CloseServiceHandleType)GetProcAddress(hAdvapi32,CloseServiceHandle_APIName);
			if(NULL==HideApi->CloseServiceHandle)
				break;
			Result = true;
			break;
		}
		return Result;
	}

	int GetServiceState(LPCTSTR lpszServName)
	{
		int ret = SERVICESTATE::FAILED;
		NC_HideApi HideApi={0};
		if(!InitHideApi(&HideApi)) return SERVICESTATE::SCM_LOADAPI_FAILED;
		//打开服务管理数据库
		BOOL Result = FALSE;
		SC_HANDLE hSCM = HideApi.OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT|SC_MANAGER_ENUMERATE_SERVICE);
		if(hSCM)
		{
			SC_HANDLE hService = HideApi.OpenService(hSCM, lpszServName, SERVICE_ALL_ACCESS);
			if (hService)
			{
				// 默认查询失败
				ret = SERVICESTATE::SERVICE_QUERY_FAILED;

				LPQUERY_SERVICE_CONFIG lpsc = NULL;
				DWORD dwBytesNeeded = 0;
				DWORD cbBufSize = 0;
				DWORD dwError = 0;
				if (!HideApi.QueryServiceConfig(hService, NULL, 0, &dwBytesNeeded))
				{
					dwError = GetLastError();
					if (ERROR_INSUFFICIENT_BUFFER == dwError)
					{
						cbBufSize = dwBytesNeeded;
						lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, cbBufSize);
					}
				}

				if (lpsc && QueryServiceConfig(hService, lpsc, cbBufSize, &dwBytesNeeded)) 
				{
					switch (lpsc->dwStartType)
					{
					case SERVICE_BOOT_START : ret = SERVICESTATE::SERVICE_BOOTSTART; break;
					case SERVICE_SYSTEM_START : ret = SERVICESTATE::SERVICE_SYSTEMSTART; break;
					case SERVICE_AUTO_START : ret = SERVICESTATE::SERVICE_AUTOSTART; break;
					case SERVICE_DEMAND_START : ret = SERVICESTATE::SERVICE_MANUAL; break;
					case SERVICE_DISABLED : ret = SERVICESTATE::SERVICE_DISABLE; break;
					default: break;
					}
				}

				if (lpsc) LocalFree(lpsc);
				HideApi.CloseServiceHandle(hService);
			}
			else
			{
				ret = SERVICESTATE::SERVICE_OPEN_FAILED;
			}
			HideApi.CloseServiceHandle(hSCM);
		}
		else
		{
			ret = SERVICESTATE::SCM_OPEN_FAILED;
		}

		return ret;
	}
}

