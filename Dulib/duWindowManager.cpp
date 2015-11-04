//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duWindowManager.cpp
//
//  History:    Nov-10-95   DavePl  Created
//
//--------------------------------------------------------------------------
#include "duWindowManager.h"
#include "duResManager.h"
#include <shlwapi.h>
#include <WindowsX.h>

extern HINSTANCE g_hInstance;
extern set<duPlugin *> *g_setValid;
extern ULONG_PTR g_gdiplusToken;
extern GdiplusStartupInput g_gdiplusStartupInput;
BOOL g_bYaheiExist;


//////////////////////////////////////////////////////////////////////////
// Constructor/Distructor
duWindowManager::duWindowManager() :
	 m_pDocXml(NULL)
	,m_pResManager(NULL)
	,m_fZip(FALSE)
	,m_hZip(NULL)
	,m_pTrialImage(NULL)
	
{
	ZeroMemory(m_szSkinPath, MAX_PATH * sizeof(TCHAR));
	m_mapCtrlManager.clear();
}

duWindowManager::~duWindowManager()
{
	if (m_pDocXml)
	{
		delete m_pDocXml;
		m_pDocXml = NULL;
	}
	
	if (m_hZip)
	{
		CloseZip(m_hZip);
		m_hZip = NULL;
	}
	
	if (m_pTrialImage)
	{
		m_pTrialImage->FinalRelease();
		m_pTrialImage = NULL;
	}

	SAFE_DELETE(m_pResManager);
	m_mapCtrlManager.clear();
}

//////////////////////////////////////////////////////////////////////////
// Interface
BOOL duWindowManager::Attach(HWND hWnd, LPCTSTR lpWindowName)
{
	if (hWnd == NULL || !::IsWindow(hWnd))
		return FALSE;

	TiXmlElement *pElement = GetWindowXmlElement(lpWindowName);
	if (pElement == NULL)
		return FALSE;
	
	duCtrlManager *pCtrlManager = new duCtrlManager(this);
	if (pCtrlManager->Create(hWnd, pElement))
	{
		m_mapCtrlManager[hWnd] = pCtrlManager;
		return TRUE;
	}

	return FALSE;
}

LPCTSTR duWindowManager::GetSkinPath()
{
	return m_szSkinPath;
}

duPlugin *duWindowManager::CreateControl(LPCTSTR lpszType, LPCTSTR lpszName, duPlugin *pParent)
{
	return NULL;
}

int duWindowManager::GetWindowCount()
{
	return (int)m_mapCtrlManager.size();
}

HWND duWindowManager::GetWindowAt(int nIndex)
{
	int i = 0;
	map<HWND, duCtrlManager *>::iterator iter = m_mapCtrlManager.begin();
	for (; iter != m_mapCtrlManager.end(); iter++)
	{
		if (i == nIndex)
			return iter->first;
		i++;
	}
	
	return NULL;
}

BOOL duWindowManager::RemoveHwnd(HWND hWnd)
{
	m_mapCtrlManager.erase(hWnd);
	return TRUE;
}

BOOL  CALLBACK FPC_EnumFontProc (LPLOGFONT lplf, LPTEXTMETRIC lptm, DWORD dwType, LPARAM lpData)	
{
	/*TCHAR szBuf[1024];
	_stprintf(szBuf, _T("%s\n"), lplf->lfFaceName);
	OutputDebugString(szBuf);*/

	if (_tcsstr(lplf->lfFaceName, _T("΢���ź�")))
	{
		g_bYaheiExist = TRUE;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
// Member
BOOL duWindowManager::OpenSkin(LPCTSTR lpszXmlFile)
{
	if (lpszXmlFile == NULL || *lpszXmlFile == 0)
		return FALSE;
	
	InitTrialBitmap();

	
	LPCTSTR lpszTypeName = GetTypeInfoName();

	_tcsncpy(m_szSkinPath, lpszXmlFile, MAX_PATH);
	TCHAR *lpszChar = _tcsrchr(m_szSkinPath, '\\');
	if (lpszChar == NULL)
	{
		ZeroMemory(m_szSkinPath, MAX_PATH * sizeof(TCHAR));
		return FALSE;
	}
	
	lpszChar++;
	*lpszChar = 0;
	
	IStream *pStream = NULL;
	HANDLE hMem = NULL;
	PBYTE pByte = GetResource(_T("config.xml"), hMem, pStream);
	if (pByte == NULL || pStream == NULL || hMem == NULL)
		return FALSE;
		
	m_pResManager = new duResManager();
	m_pResManager->SetWindowManager(this);
	m_pDocXml = new TiXmlDocument;

#ifdef _UNICODE
	int nSize = GlobalSize(hMem);
	char *p = new char[nSize + 2];
	memcpy(p, pByte, nSize);
	p[nSize] = _T('\0');
	p[nSize+1] = _T('\0');
	wchar_t *pW = (wchar_t *)p;
	pW++;
	m_pDocXml->Parse(pW, NULL);
	SAFE_DELETE_ARRAY(p);
#else
	m_pDocXml->Parse((LPCTSTR)pByte, NULL);
#endif

	ReleaseResource(hMem, pStream);
	
	TiXmlElement *pElement = NULL;
	TiXmlHandle xmlHandler(m_pDocXml->FirstChildElement(_T("directui")));
	pElement = xmlHandler.FirstChildElement(_T("theme")).Element();
	if (!m_pResManager->OnCreate(pElement))
		return FALSE;

	//
	//���ź�������жϣ����û���ź����壬�ͽ��źڼӴָ�Ϊ������ϸ��
	//
	g_bYaheiExist = FALSE;
	EnumFonts(::GetDC(NULL), NULL, (FONTENUMPROC)FPC_EnumFontProc, NULL);
	if (!g_bYaheiExist) //�źڲ�����
	{
		int i;
		int nFontCount = m_pResManager->GetResObjCount(DU_RES_FONT);
		for (i = 0;i < nFontCount; i++)
		{
			duFont *pFont = (duFont *)m_pResManager->GetResObjByIndex(i,DU_RES_FONT);
			if (pFont->GetWeight() != 400)
			{
				pFont->SetWeight(400);
				pFont->CreateFont();
			}
		}

	}

	hMem = NULL;
	pStream = NULL;
	pByte = GetResource(_T("DirectUI.js"), hMem, pStream);
	if (pByte == NULL || pStream == NULL || hMem == NULL)
		return TRUE;
	
	nSize = GlobalSize(hMem);
	p = new char[nSize + 128];
	ZeroMemory(p, nSize);
	memcpy(p, pByte, nSize);
	p[nSize] = _T('\0');
	p[nSize+1] = _T('\0');
	pW = (wchar_t *)p;
	pW++;
	lstrcat(pW, _T("\r\nCollectGarbage();\r\n"));
	SAFE_DELETE_ARRAY(p);

	lpszTypeName = m_pResManager->GetTypeInfoName();

	ReleaseResource(hMem, pStream);

	return TRUE;
}



BOOL duWindowManager::OpenSkinZip(LPCTSTR lpszZipFile, LPCSTR lpszZipPassword)
{
	if (lpszZipFile == NULL || *lpszZipFile == 0 || !::PathFileExists(lpszZipFile))
		return FALSE;
	
	m_fZip = TRUE;
	m_hZip = OpenZip(lpszZipFile, lpszZipPassword);
	if (m_hZip == NULL)
		return FALSE;
	
	return OpenSkin(lpszZipFile);
}

BOOL duWindowManager::OpenSkinMemory(BYTE *pData, int nSize)
{
	if (pData == NULL || nSize <= 0)
		return FALSE;
	
	m_fZip = TRUE;
	m_hZip = OpenZip(pData, nSize, NULL);
	if (m_hZip == NULL)
		return FALSE;
	
	return OpenSkin(_T("Memory\\"));
}

BOOL duWindowManager::ExecScript(LPCTSTR lpszFuncName, int nParamCount, VARIANT *pParams)
{
	return FALSE;
}

PBYTE duWindowManager::GetResource(LPCTSTR lpszResName, HANDLE &hMem, IStream *&pStream)
{
	if (lpszResName == NULL)
		return NULL;
	
	if (m_fZip)
		return GetZipResource(lpszResName, hMem, pStream);
	else
	{
		TCHAR szFilePath[MAX_PATH];
		_tcsncpy(szFilePath, m_szSkinPath, MAX_PATH);
		_tcscat(szFilePath, lpszResName);
		
		HANDLE hFile = CreateFile(szFilePath, 
			GENERIC_READ, 
			FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE, 
			NULL, 
			OPEN_EXISTING, 
			NULL, 
			NULL);
		if (hFile == INVALID_HANDLE_VALUE || hFile == NULL)
			return FALSE;
		
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);
		if (dwFileSize > 0)
		{
			hMem = ::GlobalAlloc(GMEM_FIXED, dwFileSize);
			::CreateStreamOnHGlobal(hMem, FALSE, &pStream);
			if (pStream == NULL)
			{
				::CloseHandle(hFile);
				::GlobalFree(hMem);
				hMem = NULL;
				return NULL;
			}
			
			PBYTE pByte = (BYTE *)::GlobalLock(hMem);
			DWORD dwByteRead = 0;
			if (::ReadFile(hFile, pByte, dwFileSize, &dwByteRead, NULL))
			{
				::CloseHandle(hFile);
				return pByte;
			}
		}
		
		::CloseHandle(hFile);
		if (pStream)
		{
			pStream->Release();
			pStream = NULL;
		}
		
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
		hMem = NULL;
		return NULL;
	}
	
	return NULL;
}

PBYTE duWindowManager::GetZipResource(LPCTSTR lpszResName, HANDLE &hMem, IStream *&pStream)
{
	if (lpszResName == NULL || m_hZip == NULL)
		return NULL;
	
	TCHAR szDir[1024];
	_tcsncpy(szDir, lpszResName, MAX_PATH);
	
	TCHAR *lpStartChar = szDir; 
	TCHAR *lpChar = _tcschr(lpStartChar, '\\');
	while (lpChar)
	{
		*lpChar = 0;
		if (SetUnzipBaseDir(m_hZip, lpStartChar) != ZR_OK)
			return NULL;
		
		lpChar++;
		lpStartChar = lpChar;
		lpChar = _tcschr(lpStartChar, '\\');
	}

	_tcsncpy(szDir, lpszResName, MAX_PATH);
	lpChar = szDir;
	while (*lpChar)
	{
		if (*lpChar == '\\') 
			*lpChar = '/';
		lpChar++;
	}
	
	int nIndex = 0;
	ZIPENTRY ze;
	if (FindZipItem(m_hZip, szDir, true, &nIndex, &ze) != ZR_OK || nIndex == -1)
		return NULL;
	
	hMem = ::GlobalAlloc(GMEM_FIXED, ze.unc_size);
	::CreateStreamOnHGlobal(hMem, FALSE, &pStream);
	if (pStream == NULL)
	{
		::GlobalFree(hMem);
		hMem = NULL;
		return NULL;
	}

	PBYTE pByte = (PBYTE)::GlobalLock(hMem);
	if (UnzipItem(m_hZip, nIndex, pByte, ze.unc_size) != ZR_OK)
	{
		pStream->Release();
		pStream = NULL;
		
		::GlobalFree(hMem);
		hMem = NULL;
		return NULL;
	}
	
	SetUnzipBaseDir(m_hZip, _T(""));
	return pByte;
}

void duWindowManager::ResetEditHFont()
{
	map<HWND, duCtrlManager *>::iterator iter = m_mapCtrlManager.begin();
	for (; iter != m_mapCtrlManager.end(); iter++)
	{
		duCtrlManager *  pCtrlMgr = iter->second;
		pCtrlMgr->ResetEditHFont();
	}
}


VOID duWindowManager::ReleaseResource(HANDLE hMem, IStream *pStream)
{
	if (pStream)
	{
		pStream->Release();
		pStream = NULL;
	}
	
	if (hMem)
	{
		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
	}
}

TiXmlElement *duWindowManager::GetWindowXmlElement(LPCTSTR lpszWindowName)
{
	if (lpszWindowName == NULL || m_pDocXml == NULL)
		return NULL;

	TiXmlElement *pElement = NULL;
	TiXmlHandle xmlHandler(m_pDocXml->FirstChildElement(_T("directui")));

	pElement = xmlHandler.FirstChildElement(_T("ducontrol")).FirstChildElement(_T("duwindow")).Element();
	while(pElement)
	{
		dustring strWindowName = ReadXmlAttributeText(pElement, _T("name"));
		if (strWindowName == lpszWindowName)
			return pElement;

		pElement = pElement->NextSiblingElement(_T("duwindow"));
	}

	return NULL;
}

void duWindowManager::OutputDebugString(LPCTSTR lpszOutput)
{
	::OutputDebugString(lpszOutput);
	::OutputDebugString(_T("\r\n"));
}

BOOL duWindowManager::ShowWindow(HWND hWnd, int nCmdShow)
{
	if ((nCmdShow == SW_SHOWMINIMIZED) || (nCmdShow == SW_MINIMIZE))
	{
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return TRUE;
	}
	else if ((nCmdShow == SW_SHOWMAXIMIZED) || (nCmdShow == SW_MAXIMIZE))
	{
		::PostMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		return TRUE;
	}

	return ::ShowWindow(hWnd, nCmdShow);
}

BOOL duWindowManager::DestroyWindow(HWND hWnd)
{
	return ::PostMessage(hWnd, WM_CLOSE, NULL, NULL);
}

void duWindowManager::PostQuitMessage(int nExitCode)
{
	::PostQuitMessage(nExitCode);
}

BOOL duWindowManager::SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	return ::SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

int duWindowManager::GetRed(int color)
{
	return GetRValue(color);
}

int duWindowManager::GetGreen(int color)
{
	return GetGValue(color);
}

int duWindowManager::GetBlue(int color)
{
	return GetBValue(color);
}

BOOL duWindowManager::ChangeSkin(LPCTSTR lpszFile)
{
	if (lpszFile == NULL || *lpszFile == 0 || !::PathFileExists(lpszFile))
		return FALSE;
	
	TCHAR szOldSkinPath[MAX_PATH];
	_tcsncpy(szOldSkinPath, m_szSkinPath, MAX_PATH);
	
	_tcsncpy(m_szSkinPath, lpszFile, MAX_PATH);
	TCHAR *pChar = _tcsrchr(m_szSkinPath, '\\');
	pChar++;
	*pChar = _T('\0');
	
	HANDLE hMem = NULL;
	IStream *pStream = NULL;
	PBYTE pXMLData = GetResource(_T("config.xml"), hMem, pStream);
	if (pXMLData == NULL || hMem == NULL || pStream == NULL)
	{
		_tcsncpy(m_szSkinPath, szOldSkinPath, MAX_PATH);
		return FALSE;
	}
	
	duResManager *pResManager = new duResManager;
	pResManager->SetWindowManager(this);
	TiXmlDocument *pDocXml = new TiXmlDocument;
	
	int nSize = ::GlobalSize(hMem);
	char *p = new char[nSize + 2];
	memcpy(p, pXMLData, nSize);
	p[nSize] = _T('\0');
	p[nSize+1] = _T('\0');
	wchar_t *pW = (wchar_t *)p;
	pW++;
	pDocXml->Parse(pW, NULL);
	SAFE_DELETE_ARRAY(p);
	ReleaseResource(hMem, pStream);
	
	TiXmlElement *pElement = NULL;
	TiXmlHandle xmlHandler(pDocXml->FirstChildElement(_T("directui")));
	pElement = xmlHandler.FirstChildElement(_T("theme")).Element();
	if (!pResManager->OnCreate(pElement))
	{
		delete pResManager;
		delete pDocXml;
		_tcsncpy(m_szSkinPath, szOldSkinPath, MAX_PATH);
		return FALSE;
	}
	
	hMem = NULL;
	pStream = NULL;
	PBYTE pByte = GetResource(_T("DirectUI.js"), hMem, pStream);
	if (pByte == NULL || pStream == NULL || hMem == NULL)
	{
		delete pResManager;
		delete pDocXml;
		_tcsncpy(m_szSkinPath, szOldSkinPath, MAX_PATH);
		return FALSE;
	}
	
	nSize = GlobalSize(hMem);
	p = new char[nSize + 128];
	ZeroMemory(p, nSize);
	memcpy(p, pByte, nSize);
	p[nSize] = _T('\0');
	p[nSize+1] = _T('\0');
	pW = (wchar_t *)p;
	pW++;
	lstrcat(pW, _T("\r\nCollectGarbage();\r\n"));
	SAFE_DELETE_ARRAY(p);

	if (m_pDocXml)
		delete m_pDocXml;
	if (m_pResManager)
		delete m_pResManager;

	//
	
	
	m_pDocXml = pDocXml;
	m_pResManager = pResManager;

	ResetEditHFont();
	return TRUE;
}

void duWindowManager::Redraw(duPlugin *pPlugin)
{
	Plugin_Redraw(pPlugin, TRUE);
}

void duWindowManager::InitTrialBitmap()
{

}
