#include "stdafx.h"
#include "DulibEx.h"
#include <Windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

namespace DulibEx
{
	BOOL SetPluginPic(duWindowManager* pWindMgr, duPlugin* pPlugin, LPCTSTR lpPic)
	{
		TCHAR _szResName[MAX_PATH];
		::StrCpy(_szResName, ::PathFindFileName(lpPic));
		::CharLower(_szResName);
		TCHAR szResName[MAX_PATH] = { 0 };
		_stprintf_s(szResName, _countof(szResName), _T("avatar_%s"), _szResName);

		if (!pWindMgr)
			return FALSE;
		duResManager* pResMgr = pWindMgr->GetResManager();
		if (!pResMgr)
			return FALSE;
		duImage* pResImage = (duImage*)pResMgr->CreateResObj(szResName, DU_RES_IMAGE);

		if (!pResImage)
			return FALSE;
		if (pResImage->LoadFromFile(lpPic))
		{
			if (pResMgr->AddResObj(pResImage))
			{
				duStyleGroup* pStyleGroup = (duStyleGroup *)pResMgr->GetResObj(pPlugin->GetStyle(), DU_RES_STYLEGROUP);
				duImageStyle *pImageStyle = (duImageStyle *)pStyleGroup->GetStyle(0);
				if (!pImageStyle)
					return FALSE;
				pImageStyle->SetPicFile(szResName);
				Plugin_Redraw(pPlugin, FALSE);
			}
		}
		else
		{
			//pResImage->Release();
			return FALSE;
		}
		return TRUE;
	}
}

