#pragma once

//////////////////////////////////////////////////////////////////////////
// DulibEx

//#include "stdafx.h"
//#include "Client.h"


#ifdef _UNICODE
#define LoadStyle LoadStyleW
#else
#define LoadStyle LoadStyleA
#endif // _UNICODE

namespace DulibEx
{
	BOOL SetPluginPic(duWindowManager* pWindMgr, duPlugin* pPlugin, LPCTSTR lpPic);
};

