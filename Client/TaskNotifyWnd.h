#pragma once

#include "../base/win/shadow_window.h"

// CMicroTaskNotifyWnd 对话框
class CTaskNotifyWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskNotifyWnd)
	DECLARE_MESSAGE_MAP()

public:
	CTaskNotifyWnd(CWnd* pParent = NULL, UINT uType = TASKNOTIFY_MICROTASK);   // 标准构造函数
	virtual ~CTaskNotifyWnd();

	enum { IDD = IDD_DUIDIALOG };
	enum
	{
		TASKNOTIFY_MICROTASK,
		TASKNOTIFY_TRANS,
		
	};
	enum
	{
		TASKNOTIFY_IDIGNORE = 1,
		TASKNOTIFY_IDVIEW = 2,
	};

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnIgnoreBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnViewBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	base::win::ShadowWindow m_shadow_win;
	UINT m_uType;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
