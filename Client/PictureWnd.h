#pragma once

// CPicturePreviewWnd
class CPictureWnd : public CWnd
{
	DECLARE_DYNAMIC(CPictureWnd)

public:
	CPictureWnd();
	virtual ~CPictureWnd();
	CRect AdjustDrawRect(CRect* rcScreen, CSize sizePicture, BOOL bCenter);
	BOOL LoadImage(const CString& sPath);

protected:
	DECLARE_MESSAGE_MAP()

public:
	void CreateToolTip(HWND hwndParent, RECT rect, LPTSTR lpszText);
	BOOL ShowWindow(int nCmdShow);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg LRESULT OnImgRotate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnZoomin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnZoomout(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CxImage& GetImage() { return m_xImage; }


private:
	CString m_sImagePath;
	CxImage m_xImage;
	CRect m_rcDraw;
	BOOL	m_bLBPressed;
	POINT m_ptLast;
	HDC		m_hdcBk;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


