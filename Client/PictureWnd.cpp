// PicturePreviewWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "PictureWnd.h"
#include "../Core/file/FileManager.h"
#include "typedef.h"

// CPicturePreviewWnd

IMPLEMENT_DYNAMIC(CPictureWnd, CWnd)

BEGIN_MESSAGE_MAP(CPictureWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_IMG_ROTATE, OnImgRotate)
	ON_MESSAGE(WM_ZOOMIN, OnZoomin)
	ON_MESSAGE(WM_ZOOMOUT, OnZoomout)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CPictureWnd::CPictureWnd() : m_bLBPressed(FALSE), m_hdcBk(NULL)
{
}

CPictureWnd::~CPictureWnd()
{
}

CRect CPictureWnd::AdjustDrawRect(CRect* rcScreen, CSize sizePicture, BOOL bCenter)
{
	CRect rect(rcScreen);
	double dWidth = rcScreen->Width();
	double dHeight = rcScreen->Height();
	double dAspectRatio = dWidth / dHeight;

	double dPictureWidth = sizePicture.cx;
	double dPictureHeight = sizePicture.cy;
	double dPictureAspectRatio = dPictureWidth / dPictureHeight;

	//If the aspect ratios are the same then the screen rectangle
	// will do, otherwise we need to calculate the new rectangle

	if (dPictureAspectRatio > dAspectRatio)
	{
		int nNewHeight = (int)(dWidth / dPictureWidth*dPictureHeight);
		int nCenteringFactor = (rcScreen->Height() - nNewHeight) / 2;
		rect.SetRect(0,
			nCenteringFactor,
			(int)dWidth,
			nNewHeight + nCenteringFactor);

	}
	else if (dPictureAspectRatio < dAspectRatio)
	{
		int nNewWidth = (int)(dHeight / dPictureHeight*dPictureWidth);
		int nCenteringFactor = (rcScreen->Width() - nNewWidth) / 2;
		rect.SetRect(nCenteringFactor,
			0,
			nNewWidth + nCenteringFactor,
			(int)(dHeight));
	}

	return rect;
};

BOOL CPictureWnd::LoadImage(const CString& sPath)
{
	if (!m_xImage.Load(sPath))
		return -1;
	m_sImagePath = sPath;

	int nWidth = m_xImage.GetWidth();
	int nHeight = m_xImage.GetHeight();
	CSize size(nWidth, nHeight);
	CPaintDC dc(this); // device context for painting
	CRect rcClient;
	GetClientRect(&rcClient);
	m_rcDraw = rcClient;
	CRect rc(0, 0, rcClient.Width(), rcClient.Height());
	m_rcDraw = AdjustDrawRect(&rc, size, TRUE);
	m_xImage.Draw2(dc, m_rcDraw);

	//CreateToolTip(GetSafeHwnd(), m_rcDraw, _T("双击打开图片"));

	return TRUE;
}

int CPictureWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

// 	if (!WindowManager_Attach(theApp.GetWindowManager(), GetSafeHwnd(), _T("picture_preview_window")))
// 		return -1;
	//SetLayeredWindowAttributes(0, 200, |LWA_ALPHA);

//	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) ^ WS_EX_LAYERED);
//	SetLayeredWindowAttributes(RGB(0, 0, 0), 200, LWA_ALPHA);

	ModifyStyle(WS_CAPTION | WS_SYSMENU, 0, SWP_FRAMECHANGED);

	return 0;
}

BOOL CPictureWnd::PreCreateWindow(CREATESTRUCT& cs)
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
	wcex.lpszClassName = _T("PictureWnd");
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);
	return CWnd::PreCreateWindow(cs);
}

void CPictureWnd::CreateToolTip(HWND hwndParent, RECT rect, LPTSTR lpszText)
{
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hwndParent,
		NULL,
		AfxGetInstanceHandle(),
		NULL);

	::SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	TOOLINFO ti = { 0 };
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwndParent;
	ti.hinst = AfxGetInstanceHandle();
	ti.lpszText = lpszText;
	ti.rect = rect;

	::SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}

afx_msg LRESULT CPictureWnd::OnImgRotate(WPARAM wParam, LPARAM lParam)
{
	m_xImage.Rotate(90, NULL);
	CRect rcClient;
	GetClientRect(&rcClient);
	
	CRect rc(0, 0, rcClient.Width(), rcClient.Height());
	CSize size(m_rcDraw.Height(), m_rcDraw.Width());
	m_rcDraw = AdjustDrawRect(&rc, size, TRUE);
	Invalidate(TRUE);
	return 0;
}

afx_msg LRESULT CPictureWnd::OnZoomin(WPARAM wParam, LPARAM lParam)
{

	CRect rcOld = m_rcDraw;
	double x = rcOld.left;
	double y = rcOld.top;
	double cx = rcOld.right - rcOld.left;
	double cy = rcOld.bottom - rcOld.top;

	double fy = cy / cx;

	double zoom = 100;
	cx += zoom;

	cy = cx * fy;
	int nWidth = m_xImage.GetWidth();
	int nHeight = m_xImage.GetHeight();
	CSize size(nWidth, nHeight);
	CRect rc(0, 0, (int)cx, (int)cy);
	m_rcDraw = AdjustDrawRect(&rc, size, TRUE);
	int x1 = (int)x - (m_rcDraw.Width() - rcOld.Width()) / 2;
	int y1 = (int)y - (m_rcDraw.Height() - rcOld.Height()) / 2;
	m_rcDraw.MoveToXY(x1, y1);
	Invalidate(TRUE);
	return 0;
}

afx_msg LRESULT CPictureWnd::OnZoomout(WPARAM wParam, LPARAM lParam)
{
	CRect rcOld = m_rcDraw;
	double x = rcOld.left;
	double y = rcOld.top;
	double cx = rcOld.right - rcOld.left;
	double cy = rcOld.bottom - rcOld.top;

	double fy = cy / cx;

	double zoom = -100;
	cx += zoom;

	cy = cx * fy;
	int nWidth = m_xImage.GetWidth();
	int nHeight = m_xImage.GetHeight();
	CSize size(nWidth, nHeight);
	CRect rc(0, 0, (int)cx, (int)cy);
	m_rcDraw = AdjustDrawRect(&rc, size, TRUE);
	int x1 = (int)x - (m_rcDraw.Width() - rcOld.Width()) / 2;
	int y1 = (int)y - (m_rcDraw.Height() - rcOld.Height()) / 2;
	m_rcDraw.MoveToXY(x1, y1);
	Invalidate(TRUE);
	return 0;
}

BOOL CPictureWnd::ShowWindow(int nCmdShow)
{
	return CWnd::ShowWindow(nCmdShow);
}

void CPictureWnd::OnPaint()
{
	if (m_xImage.IsValid())
	{
		CPaintDC dc(this); // device context for painting
		m_xImage.Draw2(dc, m_rcDraw);
	}
}

LRESULT CPictureWnd::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return HTCLIENT;
	//return CWnd::OnNcHitTest(point);
}

BOOL CPictureWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	
	CRect rcOld = m_rcDraw;
	double x = rcOld.left;
	double y = rcOld.top;
	double cx = rcOld.right - rcOld.left;
	double cy = rcOld.bottom - rcOld.top;

	double fy = cy / cx;

	double zoom = zDelta * 0.3;
	cx += zoom;

	cy = cx * fy;
	int nWidth = m_xImage.GetWidth();
	int nHeight = m_xImage.GetHeight();
	CSize size(nWidth, nHeight);
	CRect rc(0, 0, (int)cx, (int)cy);
	m_rcDraw = AdjustDrawRect(&rc, size, TRUE);
	int x1 = (int)x - (m_rcDraw.Width() - rcOld.Width()) / 2;
	int y1 = (int)y - (m_rcDraw.Height() - rcOld.Height()) / 2;
	m_rcDraw.MoveToXY(x1, y1);
	Invalidate(TRUE);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CPictureWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默--++认值
// 	if (PtInRect(&m_rcDraw, point))
// 	{
// 		FileManager::OpenFile((LPCTSTR)m_sImagePath);
// 	}
	
	CWnd::OnLButtonDblClk(nFlags, point);
}


void CPictureWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	SetFocus();
	if (PtInRect(&m_rcDraw, point))
	{
		m_bLBPressed = TRUE;
		m_ptLast = point;
	}
	
	CWnd::OnLButtonDown(nFlags, point);
} 


void CPictureWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_bLBPressed = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}


void CPictureWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	if (m_bLBPressed)
	{
		int x = m_rcDraw.left;
		int y = m_rcDraw.top;
		int x1 = point.x - (m_ptLast.x - m_rcDraw.left);
		int y1 = point.y - (m_ptLast.y - m_rcDraw.top);
		m_ptLast = point;
		m_rcDraw.MoveToXY(x1, y1);
		Invalidate(TRUE);
	}
	CWnd::OnMouseMove(nFlags, point);
}
