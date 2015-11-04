#pragma once

#include "../base/win/shadow_window.h"
// CTopMenuWnd

class CTopMenuWnd : public CWnd
{
	DECLARE_DYNAMIC(CTopMenuWnd)
public:
	CTopMenuWnd();
	virtual ~CTopMenuWnd();

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL ShowWindow(int nCmdShow);
	LRESULT OnCleanBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnLogoutBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

private:
	base::win::ShadowWindow m_shadow_win;
};


