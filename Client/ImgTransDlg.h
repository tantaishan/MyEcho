#pragma once
#include "PictureWnd.h"
#include "../base/win/shadow_window.h"
#include "TransEdit.h"
#include "biz/TransnBizApi.h"
#include "ImgToolWnd.h"

// CImgTransDlg 对话框

class CImgTransDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImgTransDlg)

public:

	CImgTransDlg(CWnd* pParent, const TASK_INFO2& task);   // 标准构造函数
	virtual ~CImgTransDlg();

// 对话框数据
	enum { IDD = IDD_DUIDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnFeedBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnClaimBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnRotateBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnSubmitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDownloadNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	void SetTask(const TASK_INFO2& task) { m_Task = task; }
private:
	friend class CCtxFeedbackWnd;
	CPictureWnd	m_ImageWnd;
	base::win::ShadowWindow m_shadow_win;
	CTransEdit	m_editInput;
	BOOL		m_bClaim;
	CImgToolWnd m_ImgToolWnd;
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	TASK_INFO2 m_Task;
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
