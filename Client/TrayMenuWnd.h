#pragma once
#include "../base/win/shadow_window.h"

// CTrayMenuWnd

class CTrayMenuWnd : public CWnd
{
	DECLARE_DYNAMIC(CTrayMenuWnd)

public:
	CTrayMenuWnd();
	virtual ~CTrayMenuWnd();
	BOOL ShowWindow(int nCmdShow);
protected:
	DECLARE_MESSAGE_MAP()

public:
	LRESULT OnClaenBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnLogoutBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMainWndBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

private:
	base::win::ShadowWindow m_shadow_win;

};


