#include "stdafx.h"
#include "FileMgr.h"

#include <shlobj.h>
#include <shellapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

static const CString kAppName = _T("MyEcho");

CString FileMgr::GetAppPath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	::PathAppend(szPath, kAppName);
	return szPath;
}
BOOL FileMgr::DeleteDirectory(LPCTSTR Directory)
{
	CFileFind tempFind;
	TCHAR sTempFileFind[MAX_PATH] = { 0 };
	_stprintf_s(sTempFileFind,MAX_PATH, _T("%s//*.*"), Directory);
	BOOL IsFinded = tempFind.FindFile(sTempFileFind);
	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();
		if (!tempFind.IsDots())
		{
			TCHAR sFoundFileName[MAX_PATH] = { 0 };
			_tcscpy_s(sFoundFileName, MAX_PATH, tempFind.GetFileName().GetBuffer(200));
			if (tempFind.IsDirectory())
			{
				TCHAR sTempDir[MAX_PATH] = { 0 };
				_stprintf_s(sTempDir, MAX_PATH, _T("%s//%s"), Directory, sFoundFileName);
				DeleteDirectory(sTempDir);
			}
			else
			{
				TCHAR sTempFileName[MAX_PATH] = { 0 };
				_stprintf_s(sTempFileName, MAX_PATH, _T("%s//%s"), Directory, sFoundFileName);
				DeleteFile(sTempFileName);
			}
		}
	}
	tempFind.Close();
	if (!RemoveDirectory(Directory))
	{
		return FALSE;
	}
	return TRUE;
}

CString FileMgr::GetAppPathById(const CString& strId, const CString& strSub)
{
	CString strPath = GetAppPath();
	TCHAR szPath[MAX_PATH] = { 0 };
	StrCpy(szPath, strPath);
	PathAppend(szPath, strId);
	PathAppend(szPath, strSub);
	strPath = szPath;
	if (!PathFileExists(szPath))
	{
		SHCreateDirectoryEx(NULL, szPath, NULL);
	}
	return strPath;
}

BOOL FileMgr::ClearAppPath()
{
	return DeleteDirectory(GetAppPath());
}