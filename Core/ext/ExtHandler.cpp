#include "ExtHandler.h"
#include "../types.h"
#include "../registry/registry.h"
#include <stdlib.h>
#include <ShlWapi.h>
#include "../config.h"

namespace ext
{
	#define MAX_STRING  1024

	const TCHAR * g_NC_FileTypes[]=
	{
		TEXT("*"),							//存档文件
		TEXT("Folder"),						//文件夹
		TEXT("Directory")					//目录
		//,TEXT("Drive")					//驱动器
	};

	int GetExtState()
	{
		TCHAR szGUIDString[MAX_PATH]={0};
		TCHAR SubKey[MAX_STRING]={0};
		_sntprintf_s(SubKey,_countof(SubKey),_countof(SubKey)-1,TEXT("CLSID\\%s\\InprocServer32"), NC_PRODUCT_GUID);
		Registry reg(HKEY_CLASSES_ROOT, SubKey, KEY_WOW64_64KEY);
		tstring str = reg.QueryValue(TEXT(""));
		if (str.empty() || !::PathFileExists(str.c_str()))
		{
			return 0;
		}

		//右键菜单附加
		for (int i=0;i<_countof(g_NC_FileTypes);i++)
		{
			TCHAR SubKeyEx[MAX_STRING]={0};
			_sntprintf_s(SubKeyEx,_countof(SubKeyEx),_countof(SubKeyEx),TEXT("%s\\shellex\\ContextMenuHandlers\\%s"), g_NC_FileTypes[i], REG_SHELL_EXT_NAME);
			Registry regEx(HKEY_CLASSES_ROOT, SubKeyEx, KEY_WOW64_64KEY);
			tstring strEx = regEx.QueryValue(TEXT(""));
			if (strEx.empty() || (strEx != NC_PRODUCT_GUID))
			{
				return 0;
			}
		}

		return 1;
	}
}

