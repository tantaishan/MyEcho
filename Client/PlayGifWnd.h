#pragma once

#include "afxwin.h"
#include "gif/PictureEx.h"

class CPlayGifWnd : public CWnd
{
public:
	CPlayGifWnd(COLORREF clr, int nGifId = -1, int nGifWidth = 0, int nGifHeight = 0);
	~CPlayGifWnd(void);

	void Show(BOOL bShow);

protected:
	int m_nGifResId;
	int m_nGifWidth;
	int m_nGifHeight;
	CBrush m_brBackground;
	CPictureEx m_Animation;
	COLORREF m_clr;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};

