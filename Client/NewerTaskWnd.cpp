// NewerTaskWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "NewerTaskWnd.h"
#include "config.h"
#include "../../Core/conv/conv.h"
#include "biz/TransnBizApi.h"
// CNewerTaskWnd

extern USER_DATA2::DATA g_LoginData;

IMPLEMENT_DYNAMIC(CNewerTaskWnd, CWnd)

CNewerTaskWnd::CNewerTaskWnd()
{

}

CNewerTaskWnd::~CNewerTaskWnd()
{
}


BEGIN_MESSAGE_MAP(CNewerTaskWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CNewerTaskWnd 消息处理程序

BOOL CNewerTaskWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	return CWnd::PreTranslateMessage(pMsg);
}


BOOL CNewerTaskWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类
	WNDCLASS wndclass;
	cs.hMenu = NULL;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndclass);
	wndclass.lpszClassName = _T("NewerTaskWnd");
	VERIFY(AfxRegisterClass(&wndclass));
	cs.lpszClass = wndclass.lpszClassName;
	return CWnd::PreCreateWindow(cs);
}

std::wstring CNewerTaskWnd::GetRequestUrl()
{
	const int kLen = 1024;
	char szUrl[kLen] = { 0 };
	Json::Value languages;
	for (unsigned int i = 0; i < g_LoginData.languages.size(); i++)
	{
		Json::Value lang;
		lang["langId"] = conv::i2s(g_LoginData.languages[i].langId);
		lang["langName"] = g_LoginData.languages[i].langName;
		lang["right"] = g_LoginData.languages[i].right ? "true" : "false";
		languages.append(lang);
	}

	sprintf_s(szUrl, _countof(szUrl), "%s?userId=%s&&languages=%s&&sysType=%d",
		client_config_newer_task.c_str(), g_LoginData.userId.c_str(), languages.toStyledString().c_str(), 3);
	return base::stdcxx_s2ws(szUrl);
}

int CNewerTaskWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("newer_task_wnd")))
		return -1;

	m_shadow_win.create(GetSafeHwnd());

	CefRefPtr<CefWebClient> client(new CefWebClient());
	m_cWebClient = client;

	CefSettings cSettings;
	CefSettingsTraits::init(&cSettings);
	cSettings.multi_threaded_message_loop = true;
	CefRefPtr<CefApp> spApp;
	CefInitialize(cSettings, spApp);

	duHwndObj* pWebHwndobj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("web_hwndobj"));
	RECT rc = { 0, 0, 0, 0 };
	pWebHwndobj->GetRect(&rc);
	CefWindowInfo info;
	info.SetAsChild(GetSafeHwnd(), rc);

	
	CefBrowserSettings browserSettings;
	CefBrowser::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(client),
		GetRequestUrl(), browserSettings);

	return 0;
}

LRESULT CNewerTaskWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CNewerTaskWnd, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CNewerTaskWnd::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CNewerTaskWnd::OnMinBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CNewerTaskWnd::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();
	return 0;
}

LRESULT CNewerTaskWnd::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

void CNewerTaskWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CNewerTaskWnd::OnDestroy()
{
	m_shadow_win.follow();
	CWnd::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


void CNewerTaskWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	duHwndObj* pWebHwndobj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("web_hwndobj"));
	RECT rc = { 0, 0, 0, 0 };
	pWebHwndobj->GetRect(&rc);

	if (m_cWebClient.get())
	{
		CefRefPtr<CefBrowser> browser = m_cWebClient->GetBrowser();
		if (browser)
		{
			CefWindowHandle hwnd = browser->GetWindowHandle();
			// ::SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, SWP_NOZORDER);  
			::MoveWindow(hwnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
		}
	}
}
