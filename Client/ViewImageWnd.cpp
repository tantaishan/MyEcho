// ViewImageWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ViewImageWnd.h"


// CViewImageWnd

IMPLEMENT_DYNAMIC(CViewImageWnd, CWnd)

CViewImageWnd::CViewImageWnd(const CString& strImage)
{
	m_strImage = strImage;
}

CViewImageWnd::~CViewImageWnd()
{
}


BEGIN_MESSAGE_MAP(CViewImageWnd, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_WM_NCDESTROY()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_WM_MOVING()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()



// CViewImageWnd 消息处理程序


LRESULT CViewImageWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CViewImageWnd, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CViewImageWnd::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CViewImageWnd::OnMinBtnClick)
	
	duEND_MESSAGE_MAP()
}

LRESULT CViewImageWnd::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CViewImageWnd::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();
	return 0;
}

int CViewImageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("图片预览"));

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("view_image_wnd")))
		return -1;

	m_shadow_win.create(GetSafeHwnd());

	// TODO:  在此添加您专用的创建代码
	if (!m_ImageWnd.CreateEx(0, _T("PictureWnd"), NULL, WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0, 0))
		return -1;

	if (!m_ImgToolWnd.Create(_T("ImgToolWnd"), NULL, WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), GetDesktopWindow(), 0, 0))
		return -1;

	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 200, rc.top + 400, 0, 0, SWP_NOSIZE);

	m_ImgToolWnd.SetAttach(m_ImageWnd.GetSafeHwnd());

	duHwndObj* pImgboxHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("imgbox_hwndobj"));
	if (pImgboxHwndObj)
	{
		pImgboxHwndObj->Attach(m_ImageWnd.GetSafeHwnd());
	}

	m_ImageWnd.LoadImage(m_strImage);

	//SetWindowPos(NULL, 0, 0, m_ImageWnd.GetImage().GetWidth(), m_ImageWnd.GetImage().GetHeight(), SWP_NOMOVE);

	return 0;
}


void CViewImageWnd::OnNcDestroy()
{
	CWnd::OnNcDestroy();
	delete this;
	// TODO:  在此处添加消息处理程序代码
}


BOOL CViewImageWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)::DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = ::AfxGetInstanceHandle();
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("ViewImageWnd");
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);
	return CWnd::PreCreateWindow(cs);
}


void CViewImageWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CViewImageWnd::OnDestroy()
{
	m_shadow_win.follow();
	CWnd::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


void CViewImageWnd::OnMoving(UINT fwSide, LPRECT pRect)
{
	CWnd::OnMoving(fwSide, pRect);
	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 200, rc.top + 400, 0, 0, SWP_NOSIZE);

	// TODO:  在此处添加消息处理程序代码
}


void CViewImageWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CWnd::OnSysCommand(nID, lParam);
}


void CViewImageWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 200, rc.top + 400, 0, 0, SWP_NOSIZE);
	// TODO:  在此处添加消息处理程序代码
}


void CViewImageWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 200, rc.top + 400, 0, 0, SWP_NOSIZE);
	// TODO:  在此处添加消息处理程序代码
}
