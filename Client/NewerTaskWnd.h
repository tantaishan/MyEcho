#pragma once
#include "../base/win/shadow_window.h"
#include "include/cef_app.h"
#include "CefWebClient.h"

// CNewerTaskWnd

class CNewerTaskWnd : public CWnd
{
	DECLARE_DYNAMIC(CNewerTaskWnd)

public:
	CNewerTaskWnd();
	virtual ~CNewerTaskWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	std::wstring GetRequestUrl();

private:
	base::win::ShadowWindow m_shadow_win;
	CefRefPtr<CefWebClient> m_cWebClient;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


