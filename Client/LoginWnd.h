#pragma once
#include "TransEdit.h"
#include "../base/win/shadow_window.h"
#include "../Core/json/json.h"
#include "biz/TransnBizApi.h"

// CLoginWnd

extern USER_DATA2::DATA g_LoginData;

class CLoginWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginWnd)
	DECLARE_MESSAGE_MAP()
public:
	CLoginWnd(CWnd* pParent = NULL);
	virtual ~CLoginWnd();
	enum { IDD = IDD_DUIDIALOG };

public:
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnLoginBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMinimizeBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnRestoreStyle(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void SetErrStyle(BOOL bErr);
	

private:
	HICON m_hIcon;
	base::win::ShadowWindow m_shadow_win;
	CTransEdit m_userEdit;
	CTransEdit m_pwdEdit;
public:
	virtual BOOL OnInitDialog();
};


