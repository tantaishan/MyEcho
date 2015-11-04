
// Client.h : Client Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "downloader.h"

class CClientApp : public CWinApp
{
	DECLARE_MESSAGE_MAP()
public:
	CClientApp();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	duWindowManager* GetWindowManager() { return m_pWinManager; }

protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	duWindowManager* m_pWinManager;
	HANDLE m_hMutex;
};

extern CClientApp theApp;
extern CDownloader g_Downloader;
