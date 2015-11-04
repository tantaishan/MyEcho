// CtxFeedbackWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "CtxFeedbackWnd.h"
#include "biz/TransnBizApi.h"
#include "ImgTransDlg.h"
#include "PromptDialog.h"

// CCtxFeedbackWnd
extern USER_DATA2::DATA g_LoginData;


IMPLEMENT_DYNAMIC(CCtxFeedbackWnd, CWnd)

CCtxFeedbackWnd::CCtxFeedbackWnd()
{

}

CCtxFeedbackWnd::~CCtxFeedbackWnd()
{
}


BEGIN_MESSAGE_MAP(CCtxFeedbackWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
END_MESSAGE_MAP()


// CCtxFeedbackWnd 消息处理程序


int CCtxFeedbackWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("feedback_wnd")))
		return -1;

	// TODO:  在此添加您专用的创建代码

	TransnBizApi::GetFeedbackItems(TT_IMG, m_vtFeedbackItems);
	
	for (unsigned int i = 0; i < m_vtFeedbackItems.size(); i++)
	{
		TCHAR szBtnName[MAX_PATH] = { 0 };
		_stprintf_s(szBtnName, MAX_PATH, _T("%s%d"), _T("feedback_btn"), i);
		duButton* pBtn = (duButton*)GetPluginByName(GetSafeHwnd(), szBtnName);
		if (pBtn)
		{
			pBtn->SetText(base::stdcxx_s2ws(m_vtFeedbackItems[i].msg).c_str());
			pBtn->SetParam(m_vtFeedbackItems[i].code);
			Plugin_Redraw(pBtn, FALSE);
		}
	}

	return 0;
}

BOOL CCtxFeedbackWnd::ShowWindow(int nCmdShow)
{
	POINT pt = { 50, 295 };
	::ClientToScreen(::GetParent(m_hWnd), &pt);
	SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE);
	return __super::ShowWindow(nCmdShow);
}

void CCtxFeedbackWnd::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	DestroyWindow();
	// TODO:  在此处添加消息处理程序代码
}

LRESULT CCtxFeedbackWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CCtxFeedbackWnd, wParam, lParam)
		duON_MESSAGE(_T("feedback_btn*"), DUM_BTNCLICK, &CCtxFeedbackWnd::OnFeedbackBtnClick)
	duEND_MESSAGE_MAP()
}

void CCtxFeedbackWnd::OnNcDestroy()
{
	CWnd::OnNcDestroy();
	delete this;
	// TODO:  在此处添加消息处理程序代码
}

LRESULT CCtxFeedbackWnd::OnFeedbackBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	TASK_FEEDBACK feedback;
	feedback.userId = g_LoginData.userId;
	CImgTransDlg* pParent = (CImgTransDlg*)GetParent();
	feedback.task_id = pParent->m_Task.task_id;
	feedback.code = pPlugin->GetParam();

	OT_RESULT res;
	TransnBizApi::FeedbackTask(feedback, res);
	if (res.result == 1)
	{
		CPromptDialog dialog(this, _T("提示"), _T("反馈成功。"), CPromptDialog::PROMPT_OK);
		CDialogEx* pParent = (CDialogEx*)GetParent();
		dialog.DoModal();
		pParent->EndDialog(0);
	}
	return 0;
}

BOOL CCtxFeedbackWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类
	WNDCLASS wndclass;
	cs.hMenu = NULL;
	cs.lpszClass = AfxRegisterWndClass(0);
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndclass);
	wndclass.lpszClassName = _T("CtxFeedbackWnd");
	VERIFY(AfxRegisterClass(&wndclass));
	cs.lpszClass = wndclass.lpszClassName;
	return TRUE;
}
