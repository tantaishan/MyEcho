
// Client.h : Client 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
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
