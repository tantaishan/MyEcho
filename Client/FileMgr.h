#pragma once
class FileMgr
{
public:
	static CString GetAppPath();
	static CString GetAppPathById(const CString& strId, const CString& strSub);
	static BOOL DeleteDirectory(LPCTSTR Directory);
	static BOOL ClearAppPath();
};

