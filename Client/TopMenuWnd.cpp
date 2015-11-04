// TopMenuWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "TopMenuWnd.h"
#include "typedef.h"

// CTopMenuWnd

IMPLEMENT_DYNAMIC(CTopMenuWnd, CWnd)

CTopMenuWnd::CTopMenuWnd()
{

}

CTopMenuWnd::~CTopMenuWnd()
{
}

BEGIN_MESSAGE_MAP(CTopMenuWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
END_MESSAGE_MAP()

int CTopMenuWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("top_menu_window")))
		return -1;


	m_shadow_win.create(GetSafeHwnd());

	return 0;
}

BOOL CTopMenuWnd::ShowWindow(int nCmdShow)
{
	POINT point = { 0 };
	::GetCursorPos(&point);
	RECT rc;
	GetWindowRect(&rc);
	int x = point.x - (rc.right - rc.left) / 2;
	int y = point.y + 10;
	this->SetWindowPos(NULL, x , y, NULL, NULL, SWP_NOSIZE);
	return CWnd::ShowWindow(nCmdShow);
}

LRESULT CTopMenuWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CTopMenuWnd, wParam, lParam)
		duON_MESSAGE(_T("clean_button"), DUM_BTNCLICK, &CTopMenuWnd::OnCleanBtnClick)
		duON_MESSAGE(_T("update_button"), DUM_BTNCLICK, &CTopMenuWnd::OnUpdateBtnClick)
		duON_MESSAGE(_T("logout_button"), DUM_BTNCLICK, &CTopMenuWnd::OnLogoutBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CTopMenuWnd::OnCleanBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CLEAN, 0, 0);
	DestroyWindow();
	return 0;
}

LRESULT CTopMenuWnd::OnUpdateBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CHECK_UPDATE, 0, 0);
	DestroyWindow();
	return 0;
}

LRESULT CTopMenuWnd::OnLogoutBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_LOGOUT, 0, 0);
	DestroyWindow();
	return 0;
}

BOOL CTopMenuWnd::PreCreateWindow(CREATESTRUCT& cs)
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
	wcex.lpszClassName = _T("TopMenuWnd");
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);
	return CWnd::PreCreateWindow(cs);
}


void CTopMenuWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	DestroyWindow();

	// TODO:  在此处添加消息处理程序代码
}


void CTopMenuWnd::OnNcDestroy()
{
	m_shadow_win.follow();
	CWnd::OnNcDestroy();
	if (this)
		delete this;
}


void CTopMenuWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
}
