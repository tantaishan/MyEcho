#include "FileManager.h"
#include <shlobj.h>
#include <shellapi.h>
#include <Shlwapi.h>
#include <CommonControls.h>
#include <io.h>
#include "../config.h"

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

BOOL FileManager::CreateFolder(LPCTSTR lpszPath)
{
	return (::SHCreateDirectoryEx(NULL, lpszPath, NULL) == ERROR_SUCCESS);
}

BOOL FileManager::DeleteFolder(LPCTSTR lpszPath)
{
	SHFILEOPSTRUCT op;
	op.fFlags = FOF_NOCONFIRMATION;
	op.hNameMappings = NULL;
	op.hwnd = NULL;
	op.lpszProgressTitle = NULL;
	op.pFrom = lpszPath;
	op.pTo = NULL;
	op.wFunc = FO_DELETE;
	return (::SHFileOperation(&op) == 0);

}

tstring FileManager::GetBasePath()
{
	TCHAR szPath[MAX_PATH] = {0};
	TCHAR szAppDataPath[MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0 , szAppDataPath);
	_stprintf_s(szPath, _T("%s\\%s"), szAppDataPath, MAIN_APPNAME);
	return szPath;
}

tstring FileManager::GetTempPath()
{
	tstring path = GetBasePath() + _T("\\Temp");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetCachePath()
{
	tstring path = GetBasePath() + _T("\\Cache");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetConfigPath()
{
	CreateFolder(GetBasePath().c_str());
	return GetBasePath() + _T("\\config.ini");
}

tstring FileManager::GetCrashPath()
{
	tstring path = GetBasePath() + _T("\\Crash");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetUpdatePath()
{
	tstring path = GetBasePath() + _T("\\Update");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetHistoryPath()
{
	tstring path = GetBasePath() + _T("\\History");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetFavoritePath()
{
	tstring path = GetBasePath() + _T("\\Favorite");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetThumbPath()
{
	tstring path = GetBasePath() + _T("\\Thumb");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::GetReportPath()
{
	tstring path = GetBasePath() + _T("\\Report");
	CreateFolder(path.c_str());
	return path;
}

tstring FileManager::AutoRename(const tstring& old)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	::StrCpy(szPath, old.c_str());
	LPTSTR lpszExt = ::PathFindExtension(szPath);
	::PathRemoveFileSpec(szPath);
	TCHAR szOld[MAX_PATH] = { 0 };
	::StrCpy(szOld, old.c_str());
	::PathStripPath(szOld);
	::PathRemoveExtension(szOld); 
	INT nIndex = 0;
	do
	{
		TCHAR tsz[MAX_PATH] = { 0 };
		ZeroMemory(tsz, sizeof(MAX_PATH));
		_stprintf_s(tsz, MAX_PATH, TEXT("%s(%d)%s"), szOld, ++nIndex, lpszExt); 
		::PathAppend(szPath, tsz);
	} while (PathFileExists(szPath));

	return szPath;
}

DWORD FileManager::GetFileSize(LPCTSTR lpszFile)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(lpszFile, &fd);
	FindClose(hFind);
	return fd.nFileSizeLow;
}

tstring FileManager::GetFileLastWriteTime(LPCTSTR lpszFile)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(lpszFile, &fd);
	SYSTEMTIME st = { 0 };
	TCHAR tsz[MAX_PATH] = { 0 };
	FILETIME ft;
	::FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ft);
	FileTimeToSystemTime(&ft, &st);
	_stprintf_s(tsz, MAX_PATH, _T("%4d-%02d-%02d %02d:%02d:%2d"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return tsz;
}

HICON FileManager::GetFileIcon(LPCTSTR lpszFile)
{
	if (INVALID_FILE_ATTRIBUTES == ::GetFileAttributes(lpszFile))
		return false;
	HICON hIcon = NULL;
	SHFILEINFO	sfi = { 0 };
	//HIMAGELIST	hil;
	PIDLIST_ABSOLUTE pidl = ::ILCreateFromPath(lpszFile);
	if (pidl)
	{
		::SHGetFileInfo((TCHAR*)pidl /*t_wszActionPath*/, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi),
			SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_LARGEICON | SHGFI_PIDL);
		ILFree(pidl);
		IImageList *lpImgList = NULL;
		::SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&lpImgList);
		if (lpImgList)
		{
			lpImgList->GetIcon(sfi.iIcon, ILD_NORMAL, &hIcon);//ILD_TRANSPARENT | ILD_BLEND50 | ILD_BLEND25
			lpImgList->Release();
		}
		::DestroyIcon(sfi.hIcon);
	}

	return hIcon;
}

static const LPTSTR s_ArchiveFormat[] =
{
	TEXT(".zip")
	,TEXT(".7z")
	,TEXT(".rar")
	,TEXT(".tar")
	,TEXT(".iso")
	,TEXT(".cab")
	,TEXT(".jar")
	,TEXT(".bz2")
	,TEXT(".gz")	
	,TEXT(".gzip")
	,TEXT(".arj")
	,TEXT(".z")	
	,TEXT(".lzma")
};

BOOL FileManager::IsArchive(const tstring& strFile)
{
	TCHAR szFile[MAX_PATH] = { 0 };
	::StrCpy(szFile, strFile.c_str());
	LPTSTR strExt = ::PathFindExtension(szFile);
	if (!_tcslen(strExt))
		return FALSE;
	::CharLower(strExt);
	// compare item
	for (UINT i = 0; i < _countof(s_ArchiveFormat); ++i)
	{
		if (!_tcscmp(strExt, s_ArchiveFormat[i]))
		{
			DWORD dwAttr = ::GetFileAttributes(strFile.c_str());
			if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY)/*&& Attribute&FILE_ATTRIBUTE_ARCHIVE*/) return FALSE;//快捷方式不属于 FILE_ATTRIBUTE_ARCHIVE
			return TRUE;
		}
	}
	return FALSE;
}

BOOL FileManager::ShowFileProperties(const tstring& strFile)
{
	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_INVOKEIDLIST;
	sei.hwnd = NULL;
	sei.lpVerb = _T("properties");
	sei.lpFile = strFile.c_str();
	sei.lpParameters = _T("");
	sei.lpDirectory = NULL;
	sei.nShow = SW_SHOW;
	sei.hInstApp = NULL;
	return ::ShellExecuteEx(&sei);
}

BOOL FileManager::OpenFile(const tstring& strFile)
{
	SHELLEXECUTEINFO sei;
	ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(sei);
	sei.lpVerb = _T("open");
	sei.lpFile = strFile.c_str();
	sei.lpParameters = _T("");
	sei.nShow = SW_SHOW;
	return ::ShellExecuteEx(&sei);
}

BOOL FileManager::OpenInExplorer(const tstring& strFile)
{
	tstring strPara;
	strPara += _T("/select,");
	strPara += strFile;
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), strPara.c_str(), NULL, SW_SHOWNORMAL);
	return GetLastError() == ERROR_SUCCESS;
}

BOOL FileManager::CreateNewDirectory(const tstring& strPath, tstring& strOutPath)
{
	TCHAR szCurr[MAX_PATH] = { 0 };
	TCHAR szNew[MAX_PATH] = { 0 };
	StrCpy(szCurr, strPath.c_str());
	StrCpy(szNew, strPath.c_str());
	PathAppend(szNew, _T("新建文件夹"));
	INT nIndex = 0;
	while (!_waccess(szNew, 0))
	{
		ZeroMemory(szNew, _countof(szNew));
		TCHAR tsz[MAX_PATH] = { 0 };
		_stprintf_s(tsz, MAX_PATH, TEXT("新建文件夹 (%d)"), ++nIndex);
		StrCpy(szNew, szCurr);
		::PathAppend(szNew, tsz);
	} 

	strOutPath = szNew;
	int ret = ::SHCreateDirectoryEx(NULL, szNew, NULL);
	return (ERROR_SUCCESS == ret);
}

BOOL FileManager::CopyFilesToClipboard(const std::vector<tstring>& vtFiles, BOOL bMove)
{
	if (vtFiles.empty())
		return FALSE;

	tstring sBuff;
	UINT uBuffLen = 0;
	for (unsigned int i = 0; i < vtFiles.size(); i++)
	{
		sBuff += vtFiles[i] + TEXT('\0');
		uBuffLen += vtFiles[i].length() * sizeof(TCHAR) + sizeof(TEXT('\0'));
	}
		
	UINT uDropEffect = 0;
	HGLOBAL hGblEffect = NULL;
	uDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
	PDWORD pdwDropEffect = NULL;
	pdwDropEffect = (PDWORD)GlobalLock(hGblEffect);
	if (bMove)
		*pdwDropEffect = DROPEFFECT_MOVE;
	else
		*pdwDropEffect = DROPEFFECT_COPY;

	GlobalUnlock(hGblEffect);

	DROPFILES dropFiles;
	ZeroMemory(&dropFiles, sizeof(DROPFILES));
	UINT uDropFilesLen = 0;
	uDropFilesLen = sizeof(DROPFILES);
	dropFiles.pFiles = uDropFilesLen;
	dropFiles.pt.x = 0;
	dropFiles.pt.y = 0;
	dropFiles.fNC = FALSE;
#ifdef _UNICODE
	dropFiles.fWide = TRUE;
#else
	dropFiles.fWide = FALSE;
#endif

	UINT uGblLen = uDropFilesLen + uBuffLen + sizeof(LPVOID)*2;
	HGLOBAL hGblFiles = NULL;
	hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGblLen);
	char* sData = NULL;
	sData = (char*)GlobalLock(hGblFiles);
	ZeroMemory(sData, uGblLen);
	memcpy(sData, (LPVOID)(&dropFiles), uDropFilesLen);
	memcpy(sData + uDropFilesLen, (LPVOID)sBuff.c_str(), uBuffLen);
	GlobalUnlock(hGblFiles);
	if (!OpenClipboard(NULL))
		return FALSE;

	EmptyClipboard();
	SetClipboardData(CF_HDROP, hGblFiles);
	SetClipboardData(uDropEffect, hGblEffect);
	CloseClipboard();

	return TRUE;
}

BOOL FileManager::CheckClipboard()
{
	BOOL bCheck = FALSE;
	if (OpenClipboard(NULL))
	{
		HDROP hDrop = (HDROP)GetClipboardData(CF_HDROP);
		if (hDrop)
		{
			UINT uFiles = 0;
			uFiles = DragQueryFile(hDrop, (UINT)-1, NULL, 0);
			if (uFiles > 0)
				bCheck = TRUE;
		}
		CloseClipboard();
	}
	return bCheck;
}

BOOL FileManager::PasteFromClipboard(HWND hWnd, LPCTSTR strPath, UINT uNotify)
{
	UINT uDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	UINT cFiles = 0;
	DWORD dwEffect = 0, *pdwEffect = NULL;
	if (OpenClipboard(hWnd))
	{
		HDROP hDrop = (HDROP)GetClipboardData(CF_HDROP);
		if (hDrop)
		{
			dwEffect = DROPEFFECT_COPY;
			pdwEffect = (PDWORD)GetClipboardData(uDropEffect);
			if (pdwEffect)
			{
				if (*pdwEffect & DROPEFFECT_MOVE)
					dwEffect = DROPEFFECT_MOVE;
			}

			cFiles = DragQueryFile(hDrop, (UINT)-1, NULL, 0);
			if (cFiles == 0)
				return FALSE;
			TCHAR szFile[MAX_PATH] = { 0 };
			for (UINT i = 0; i < cFiles; i++)
			{
				ZeroMemory(szFile, MAX_PATH);
				DragQueryFile(hDrop, i, szFile, sizeof(szFile));
				SHFILEOPSTRUCT op;
				ZeroMemory((void*)&op, sizeof(SHFILEOPSTRUCT));
				op.fFlags = FOF_NOCONFIRMATION ;
				op.hNameMappings = NULL;
				op.hwnd = hWnd;
				op.lpszProgressTitle = NULL;
				op.pFrom = szFile;
				op.pTo = strPath;
				if (dwEffect == DROPEFFECT_COPY)
					op.wFunc = FO_COPY;
				else
					op.wFunc -= FO_MOVE; 
				SHFileOperation(&op);
				PostMessage(hWnd, uNotify, 0, 0);
			}
		}
		CloseClipboard();
	}
	return TRUE;
}