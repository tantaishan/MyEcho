
// Client.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Client.h"
#include "config.h"

#ifndef _DEBUG
#include "skin.h"
#endif // _DEBUG


#include "MainFrm.h"
#include "LoginWnd.h"
#include "TransChatWnd.h"
#include "typedef.h"
#include "DulibEx.h"
#include "CrashHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
// CrashRpt
// #ifndef _DEBUG
// #include "CrashRpt.h"
// crash_rpt::CrashRpt g_crashRpt("d2ea885b-a99f-4989-9bdb-57bf48dffcde", _T("MyEcho"), _T("Wuhan Trans Co., Ltd."));
// #endif // !_DEBUG

// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
END_MESSAGE_MAP()

// CClientApp ����
CClientApp::CClientApp() : m_pWinManager(NULL)
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���: 
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Client.AppID.NoVersion"));

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CClientApp ����

CClientApp theApp;
CDownloader g_Downloader(WM_DOWNLOAD_NOTIFY, 1);
// CClientApp ��ʼ��

BOOL CClientApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	//MessageBox(0, 0, 0, 0);
	// ����������ֹ��������ڴ���
// 	m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MAIN_MUTEX_NAME);
// 	if (GetLastError() == ERROR_FILE_NOT_FOUND)
// 	{
// 		m_hMutex = CreateMutex(NULL, TRUE, MAIN_MUTEX_NAME);
// 	}
// 	else
// 	{
// 		HWND hMainWnd = FindWindow(_T("MyEchoLoginWnd"), NULL);
// 		if (hMainWnd == NULL)
// 			hMainWnd = FindWindow(_T("MyEchoMainFrame"), NULL);
// 		if (hMainWnd)
// 		{
// 			::ShowWindow(hMainWnd, IsIconic(hMainWnd) ? SW_RESTORE : SW_SHOW);
// 			::SetForegroundWindow(hMainWnd);
// 		}
// 		return FALSE;
// 	}
	
	CrashHandler::getInstance()->handleUncaughtException(TRUE);

	if (!config_init())
		return FALSE;

	Dulib_Init();

	TCHAR tzAppPath[MAX_PATH] = { 0 };
	::GetModuleFileName(GetModuleHandle(NULL), tzAppPath, _countof(tzAppPath));
	::PathRemoveFileSpec(tzAppPath);

#ifdef _DEBUG
	::PathAppend(tzAppPath, _T("\\..\\skin\\config.xml"));
	m_pWinManager = LoadStyle(tzAppPath);
#else
	//::PathAppend(tzAppPath, _T("skin.skn"));
	m_pWinManager = LoadStyleZipMemory(g_rsrcData, g_dataLen);
#endif

	if (!m_pWinManager)
		return FALSE;

	CLoginWnd pLoginWnd;
	pLoginWnd.DoModal();

	// ��ͼ���ع��� MDI �˵��Ϳ�ݼ���
	//TODO:  ��Ӹ��ӳ�Ա�����������ض�
	//	Ӧ�ó��������Ҫ�ĸ��Ӳ˵����͵ĵ���
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ClientTYPE));

	return TRUE;
}

int CClientApp::ExitInstance()
{
	//TODO:  �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);
	if (m_pWinManager)
		m_pWinManager->FinalRelease();

	Dulib_Release();

	return CWinApp::ExitInstance();
}
