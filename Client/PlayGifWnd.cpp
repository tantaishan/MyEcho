#include "StdAfx.h"
#include "PlayGifWnd.h"


CPlayGifWnd::CPlayGifWnd(COLORREF clr, int nGifId/* = -1*/, int nGifWidth/* = 0*/, int nGifHeight/* = 0*/)
	: m_clr(clr), m_nGifResId(nGifId), m_nGifWidth(nGifWidth), m_nGifHeight(nGifHeight)
{
	m_brBackground.CreateSolidBrush(clr);
}


CPlayGifWnd::~CPlayGifWnd(void)
{
	m_brBackground.DeleteObject();
}

void CPlayGifWnd::Show(BOOL bShow)
{
	ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	if (::IsWindow(m_Animation.GetSafeHwnd()))
	{
		if (bShow && !m_Animation.IsPlaying())
			m_Animation.Draw();
		else if (!bShow && m_Animation.IsPlaying())
			m_Animation.Stop();
	}
}

BEGIN_MESSAGE_MAP(CPlayGifWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL CPlayGifWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

	//return CWnd::OnEraseBkgnd(pDC);
}


void CPlayGifWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	CRect rc;
	GetClientRect(&rc);
	dc.FillRect(&rc, &m_brBackground);
}

int CPlayGifWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (m_nGifResId != -1)
	{
		m_Animation.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, m_nGifWidth, m_nGifHeight), this, 9992);
		m_Animation.Load(m_nGifResId);
		m_Animation.SetBkColor(m_clr);
		m_Animation.Draw();
	}

	return 0;
}


void CPlayGifWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	int nx = (cx - m_nGifWidth) / 2;
	int ny = (cy - m_nGifHeight) / 2;
	if (::IsWindow(m_Animation.m_hWnd) && (nx > 0) && (ny > 0))
		m_Animation.SetWindowPos(NULL, nx, ny, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}


void CPlayGifWnd::OnDestroy()
{
	if (::IsWindow(m_Animation.m_hWnd))
		m_Animation.UnLoad();
	CWnd::OnDestroy();
}
