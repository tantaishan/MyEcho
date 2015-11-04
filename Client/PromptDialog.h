#pragma once

#include "../base/win/shadow_window.h"

// CPromptDialog 对话框

class CPromptDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPromptDialog)

public:
	CPromptDialog(CWnd* pParent = NULL, LPCTSTR lpszTitle = NULL, LPCTSTR lpszText = NULL, UINT uType = PROMPT_OK);   // 标准构造函数
	virtual ~CPromptDialog();

// 对话框数据
	enum { IDD = IDD_DUIDIALOG };
	enum // Prompt Types
	{
		PROMPT_OK = 0x00000000L,
		PROMPT_ABORT_MICROTASK = 0x00000001L,
		PROMPT_ABORT_SPECIALIST = 0x00000002L,
		PROMPT_EXIT = 0x00000003L,
		PROMPT_UPDATE = 0x00000004L,

	};
	enum // Control Ids
	{
		PROMPT_IDCLOSE = 0,
		PROMPT_IDOK = 1,
		PROMPT_IDABORT = 2,
		PROMPT_IDCONTINUE = 3,
		PROMPT_IDMIN2TRAY = 4,
		PROMPT_IDEXIT = 5,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnContinueBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnAbortBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMin2TrayBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnOkBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void ShowCtrlsGroup(UINT uType);

private:
	base::win::ShadowWindow m_shadow_win;
	UINT	m_uType;
	CString m_strTitle;
	CString m_strText;
};
