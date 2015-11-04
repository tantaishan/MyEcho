#pragma once


// CImgToolWnd

class CImgToolWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgToolWnd)

public:
	CImgToolWnd();
	virtual ~CImgToolWnd();
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);

	LRESULT OnZoominBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnZoomoutBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	void SetAttach(HWND hWnd) { m_hAttach = hWnd; }

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	HWND	m_hAttach;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


