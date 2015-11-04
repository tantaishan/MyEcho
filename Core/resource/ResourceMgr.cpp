#include "ResourceMgr.h"

BOOL ResourceMgr::ReleaseFile(int nId, LPCTSTR lpszType, LPCTSTR lpszPath)
{
	HRSRC hRes = ::FindResource(NULL, MAKEINTRESOURCE(nId), lpszType);
	if (hRes == NULL)
		return FALSE;
	DWORD dwSize = ::SizeofResource(NULL, hRes);
	if (dwSize == 0)
		return FALSE;
	HGLOBAL hGlobal = ::LoadResource(NULL, hRes);
	if (hGlobal == NULL)
		return FALSE;
	LPVOID lpVoid = ::LockResource(hGlobal);
	if (lpVoid == NULL)
		return FALSE;
	HANDLE hFile = ::CreateFile(lpszPath, GENERIC_WRITE | GENERIC_READ,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD dwWritten = 0;
	::WriteFile(hFile, lpVoid, dwSize, &dwWritten, NULL);
	::FlushFileBuffers(hFile);
	::CloseHandle(hFile);

	UnlockResource(hGlobal);
	::FreeResource(hGlobal);
	return TRUE;
}
