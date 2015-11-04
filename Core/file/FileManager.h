#pragma once
#include <vector>
#include "../string/tstring.h"

class FileManager
{
public:
	static BOOL CreateFolder(LPCTSTR lpszPath);
	static BOOL DeleteFolder(LPCTSTR lpszPath);
	static tstring GetBasePath();
	static tstring GetTempPath();
	static tstring GetCachePath();
	static tstring GetConfigPath();
	static tstring GetCrashPath();
	static tstring GetUpdatePath();
	static tstring GetHistoryPath();
	static tstring GetFavoritePath();
	static tstring GetThumbPath();
	static tstring GetReportPath();
	static tstring AutoRename(const tstring& old);
	static DWORD GetFileSize(LPCTSTR lpszFile);
	static tstring GetFileLastWriteTime(LPCTSTR lpszFile);
	static HICON GetFileIcon(LPCTSTR lpszFile);
	static BOOL IsArchive(const tstring& strFile);
	static BOOL ShowFileProperties(const tstring& strFile);
	static BOOL OpenFile(const tstring& strFile);
	static BOOL OpenInExplorer(const tstring& strFile);
	static BOOL CreateNewDirectory(const tstring& strPath, tstring& strOutPath);
	static BOOL CopyFilesToClipboard(const std::vector<tstring>& vtFiles, BOOL bMove);
	static BOOL CheckClipboard();
	static BOOL PasteFromClipboard(HWND hWnd, LPCTSTR strPath, UINT uNotify);

};

