#pragma once
#include "TransEdit.h"
#include "../base/win/shadow_window.h"
#include "biz/TransnBizApi.h"

// CTextTransDlg 对话框
class CTextTransDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTextTransDlg)

public:
	CTextTransDlg(CWnd* pParent, const TASK_INFO2& task);
	virtual ~CTextTransDlg();

	enum { IDD = IDD_DUIDIALOG };

	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnSubmitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnClaimBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	

private:
	base::win::ShadowWindow m_shadow_win;
	CTransEdit	m_editInput;
	CTransEdit	m_editView;
	CString		m_strText;
	TASK_INFO2	m_Task;
	BOOL		m_bClaim;
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
};
