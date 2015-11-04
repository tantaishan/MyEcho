#pragma once
#include "PictureWnd.h"
#include "../base/win/shadow_window.h"
#include "ImgToolWnd.h"
// CViewImageWnd

class CViewImageWnd : public CWnd
{
	DECLARE_DYNAMIC(CViewImageWnd)

public:
	CViewImageWnd(const CString& strImage);
	virtual ~CViewImageWnd();
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()

private:
	base::win::ShadowWindow m_shadow_win;
	CPictureWnd	m_ImageWnd;
	CString		m_strImage;
	CImgToolWnd m_ImgToolWnd;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};


