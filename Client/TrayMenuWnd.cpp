// TrayMenuWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "TrayMenuWnd.h"
#include "typedef.h"

// CTrayMenuWnd

IMPLEMENT_DYNAMIC(CTrayMenuWnd, CWnd)

CTrayMenuWnd::CTrayMenuWnd()
{

}

CTrayMenuWnd::~CTrayMenuWnd()
{
}


BEGIN_MESSAGE_MAP(CTrayMenuWnd, CWnd)
	ON_WM_NCDESTROY()
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
END_MESSAGE_MAP()

BOOL CTrayMenuWnd::ShowWindow(int nCmdShow)
{
	POINT pt;
	::GetCursorPos(&pt);
	RECT rc = { 0 };
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	::GetClientRect(*this, &rc);
	int nWidth = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;
	int posX = pt.x;
	int posY = pt.y;
	if (posX + nWidth > cx)
	{
		posX = posX - nWidth;
	}
	if (posY + nHeight > cy)
	{
		posY = posY - nHeight;
	}

	SetForegroundWindow();
	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, posX, posY, 0, 0, SWP_NOSIZE);
	return __super::ShowWindow(nCmdShow);
}

// CTrayMenuWnd 消息处理程序

void CTrayMenuWnd::OnNcDestroy()
{
	CWnd::OnNcDestroy();

	if (this)
		delete this;
	// TODO:  在此处添加消息处理程序代码
}


int CTrayMenuWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);


	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("tray_window")))
		return -1;
	// TODO:  在此添加您专用的创建代码
	m_shadow_win.create(GetSafeHwnd());

	return 0;
}


BOOL CTrayMenuWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)::DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = ::AfxGetInstanceHandle();
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("TrayMenuWnd");
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);
	return CWnd::PreCreateWindow(cs);
}

LRESULT CTrayMenuWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CTrayMenuWnd, wParam, lParam)
		duON_MESSAGE(_T("mainwnd_button"), DUM_BTNCLICK, &CTrayMenuWnd::OnMainWndBtnClick)
		duON_MESSAGE(_T("clean_button"), DUM_BTNCLICK, &CTrayMenuWnd::OnClaenBtnClick)
		duON_MESSAGE(_T("update_button"), DUM_BTNCLICK, &CTrayMenuWnd::OnUpdateBtnClick)
		duON_MESSAGE(_T("logout_button"), DUM_BTNCLICK, &CTrayMenuWnd::OnLogoutBtnClick)
		duON_MESSAGE(_T("exit_button"), DUM_BTNCLICK, &CTrayMenuWnd::OnExitBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CTrayMenuWnd::OnMainWndBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_SHOWMAINWND, 0, 0);
	DestroyWindow();
	return 0;
}

LRESULT CTrayMenuWnd::OnClaenBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CLEAN, 0, 0);
	DestroyWindow();
	return 0;
}

LRESULT CTrayMenuWnd::OnUpdateBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHECK_UPDATE, 0, 0);
	DestroyWindow();
	return 0;
}


LRESULT CTrayMenuWnd::OnLogoutBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_LOGOUT, 0, 0);
	DestroyWindow();
	return 0;
}

LRESULT CTrayMenuWnd::OnExitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_EXIT_APP, 0, 0);
	DestroyWindow();
	return 0;
}

void CTrayMenuWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	DestroyWindow();
	// TODO:  在此处添加消息处理程序代码
}


void CTrayMenuWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CTrayMenuWnd::OnDestroy()
{
	m_shadow_win.follow();
	CWnd::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}
