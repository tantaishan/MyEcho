#pragma once
#include "typedef.h"
#include "biz/TransnBizApi.h"
// CCtxFeedbackWnd

class CCtxFeedbackWnd : public CWnd
{
	DECLARE_DYNAMIC(CCtxFeedbackWnd)

public:
	CCtxFeedbackWnd();
	virtual ~CCtxFeedbackWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNcDestroy();
	LRESULT OnFeedbackBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL ShowWindow(int nCmdShow);

private:
	std::vector<FEEDBACK_ITEM> m_vtFeedbackItems;
	
};


