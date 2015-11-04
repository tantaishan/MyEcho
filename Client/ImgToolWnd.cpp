// ImgToolWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ImgToolWnd.h"
#include "typedef.h"

// CImgToolWnd

IMPLEMENT_DYNAMIC(CImgToolWnd, CWnd)

CImgToolWnd::CImgToolWnd()
{

}

CImgToolWnd::~CImgToolWnd()
{
}


BEGIN_MESSAGE_MAP(CImgToolWnd, CWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
END_MESSAGE_MAP()



// CImgToolWnd 消息处理程序




BOOL CImgToolWnd::PreCreateWindow(CREATESTRUCT& cs)
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
	wcex.lpszClassName = _T("ImgToolWnd");
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);
	return CWnd::PreCreateWindow(cs);
}


int CImgToolWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("imgtool_wnd")))
		return -1;
	// TODO:  在此添加您专用的创建代码

	return 0;
}

LRESULT CImgToolWnd::OnZoominBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(m_hAttach, WM_ZOOMIN, 0, 0);
	return 0;
}

LRESULT CImgToolWnd::OnZoomoutBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(m_hAttach, WM_ZOOMOUT, 0, 0);
	return 0;
}

LRESULT CImgToolWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CImgToolWnd, wParam, lParam)
		duON_MESSAGE(_T("zoom_in_btn"), DUM_BTNCLICK, &CImgToolWnd::OnZoominBtnClick)
		duON_MESSAGE(_T("zoom_out_btn"), DUM_BTNCLICK, &CImgToolWnd::OnZoomoutBtnClick)
	duEND_MESSAGE_MAP()
}