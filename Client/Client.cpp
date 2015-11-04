
// Client.cpp : 定义应用程序的类行为。
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

// CClientApp 构造
CClientApp::CClientApp() : m_pWinManager(NULL)
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Client.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CClientApp 对象

CClientApp theApp;
CDownloader g_Downloader(WM_DOWNLOAD_NOTIFY, 1);
// CClientApp 初始化

BOOL CClientApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	//MessageBox(0, 0, 0, 0);
	// 进程锁，防止多个主窗口创建
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

	// 试图加载共享 MDI 菜单和快捷键表
	//TODO:  添加附加成员变量，并加载对
	//	应用程序可能需要的附加菜单类型的调用
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ClientTYPE));

	return TRUE;
}

int CClientApp::ExitInstance()
{
	//TODO:  处理可能已添加的附加资源
	AfxOleTerm(FALSE);
	if (m_pWinManager)
		m_pWinManager->FinalRelease();

	Dulib_Release();

	return CWinApp::ExitInstance();
}
