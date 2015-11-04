// TextTransDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "Client.h"
#include "TextTransDlg.h"
#include "afxdialogex.h"
#include "biz/TransnBizApi.h"
#include "PromptDialog.h"

// CTextTransDlg 对话框

extern USER_DATA2::DATA g_LoginData;

IMPLEMENT_DYNAMIC(CTextTransDlg, CDialogEx)

CTextTransDlg::CTextTransDlg(CWnd* pParent, const TASK_INFO2& task)
: CDialogEx(CTextTransDlg::IDD, pParent), m_Task(task), m_bClaim(FALSE)
{

}

CTextTransDlg::~CTextTransDlg()
{
}

void CTextTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTextTransDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
END_MESSAGE_MAP()

// CTextTransDlg 消息处理程序

BOOL CTextTransDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

int CTextTransDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("text_trans_window")))
		return -1;

	if (!m_editInput.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP /* WS_VSCROLL | WS_HSCROLL */| ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, CRect(0, 0, 0, 0), this, 9999))
		return -1;

	if (!m_editView.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP /* WS_VSCROLL | WS_HSCROLL */ | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, CRect(0, 0, 0, 0), this, 9999))
		return -1;

	SetWindowText(_T("文字翻译"));
	m_shadow_win.create(GetSafeHwnd());

	m_editInput.SetPlaceHolderColor(RGB(217, 217, 217));
	m_editInput.SetTextColor(RGB(102, 102, 102));
	m_editInput.SetBackColor(RGB(255, 255, 255));

	m_editView.SetReadOnly(TRUE);
	m_editView.SetPlaceHolderColor(RGB(217, 217, 217));
	m_editView.SetTextColor(RGB(102, 102, 102));
	m_editView.SetBackColor(RGB(235, 235, 235));

	duHwndObj* editHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("edit_hwndobj"));
	if (editHwndObj)
		editHwndObj->Attach(m_editInput.GetSafeHwnd());

	duHwndObj* pText = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("text_hwndobj"));
	if (pText)
	{
		m_strText = conv::s2wcs(m_Task.context);
		pText->Attach(m_editView.GetSafeHwnd());
		m_editView.SetWindowText(m_strText);
	}
	// TODO:  在此添加您专用的创建代码

	if (m_Task.is_unfinish)
	{
		m_bClaim = TRUE;
		duPlugin* caimLayout = GetPluginByName(GetSafeHwnd(), _T("claim_layout"));
		duPlugin* submitLayout = GetPluginByName(GetSafeHwnd(), _T("submit_layout"));
		if (caimLayout && submitLayout)
		{
			caimLayout->SetVisible(FALSE);
			submitLayout->SetVisible(TRUE);
			Plugin_Redraw(caimLayout, FALSE);
			Plugin_Redraw(submitLayout, FALSE);
			m_editInput.ShowWindow(SW_SHOW);
		}
	}

	return 0;
}

LRESULT CTextTransDlg::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	if (m_bClaim)
	{
		CPromptDialog dialog(this, _T("提醒"), _T("是否放弃当前任务？"), CPromptDialog::PROMPT_ABORT_MICROTASK);
		if (dialog.DoModal() == CPromptDialog::PROMPT_IDABORT)
		{
			// 			TASK_OPERATE op;
			// 			op.op_type = OPERATE_TYPE::OT_GIVEUP;
			// 			op.user_id = g_LoginData.userId;
			// 			op.task_id = base::stdcxx_ws2s(m_Task.taskId);
			// 			Json::Value ret;
			// 			TransnBizApi::OperateTask(op, ret);
			TASK_OPERATE2 op;
			op.taskId = m_Task.task_id;
			op.userId = g_LoginData.userId;
			op.type = OT_GIVEUP;
			OT_RESULT res;
			TransnBizApi::OperateTask2(op, res);

			EndDialog(0);
		}
	}
	else
	{
		EndDialog(0);
	}
	return 0;
	
	return 0;
}

LRESULT CTextTransDlg::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CTextTransDlg::OnSubmitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
// 	TASK_OPERATE op;
// 	op.op_type = OPERATE_TYPE::OT_COMPLETE;
// 	op.user_id = g_LoginData.userId;
// 	op.task_id = base::stdcxx_ws2s(m_Task.taskId);
// 	op.manuscript.type = 2;
// 	op.manuscript.file_type = 2;
// 	op.manuscript.status = 1;
// 	CString strInput;
// 	m_editInput.GetWindowText(strInput);
// 	op.manuscript.content = base::stdcxx_ws2s((LPCTSTR)strInput);
// 	Json::Value ret;
// 	TransnBizApi::OperateTask(op, ret);

	TASK_OPERATE2 op;
	op.userId = g_LoginData.userId;
	op.taskId = m_Task.task_id;
	op.type = OT_COMPLETE;
	CString strInput;
	m_editInput.GetWindowText(strInput);
	op.answer = base::stdcxx_ws2s((LPCTSTR)strInput);
	OT_RESULT res;
	TransnBizApi::OperateTask2(op, res);
	if (res.result == 0)
	{
		CPromptDialog dialog(this, _T("提醒"), conv::s2wcs(res.msg), CPromptDialog::PROMPT_OK);
		dialog.DoModal();
	}
	else
	{
		CPromptDialog dialog(this, _T("提醒"), _T("提交成功。"), CPromptDialog::PROMPT_OK);
		dialog.DoModal();
		EndDialog(0);
	}

	return 0;
}

LRESULT CTextTransDlg::OnClaimBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	TASK_OPERATE2 op;
	op.taskId = m_Task.task_id;
	op.userId = g_LoginData.userId;
	op.type = OT_RECEIVE;
	OT_RESULT res;
	TransnBizApi::OperateTask2(op, res);
// 	if (res.result == 0)
// 	{
// 		m_bClaim = FALSE;
// 		CPromptDialog dialog(this, _T("提醒"), conv::s2wcs(res.msg), CPromptDialog::PROMPT_OK);
// 		dialog.DoModal();
// // 		duButton* pClaimBtn = (duButton*)GetPluginByName(GetSafeHwnd(), _T("claim_button"));
// // 		duButton* pFeedbackBtn = (duButton*)GetPluginByName(GetSafeHwnd(), _T("feedback_button"));
// // 		pClaimBtn->SetDisable(TRUE);
// // 		pFeedbackBtn->SetDisable(TRUE);
// // 		Plugin_Redraw(pClaimBtn, FALSE);
// // 		Plugin_Redraw(pFeedbackBtn, FALSE);
// 		EndDialog(0);
// 	}
// 	else
	{
		m_bClaim = TRUE;
		duPlugin* caimLayout = GetPluginByName(GetSafeHwnd(), _T("claim_layout"));
		duPlugin* submitLayout = GetPluginByName(GetSafeHwnd(), _T("submit_layout"));
		if (caimLayout && submitLayout)
		{
			caimLayout->SetVisible(FALSE);
			submitLayout->SetVisible(TRUE);
			Plugin_Redraw(caimLayout, FALSE);
			Plugin_Redraw(submitLayout, FALSE);
			m_editInput.ShowWindow(SW_SHOW);
		}
	}

	return 0;
}

LRESULT CTextTransDlg::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CTextTransDlg, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CTextTransDlg::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CTextTransDlg::OnMinBtnClick)
		duON_MESSAGE(_T("submit_button"), DUM_BTNCLICK, &CTextTransDlg::OnSubmitBtnClick)
		duON_MESSAGE(_T("claim_button"), DUM_BTNCLICK, &CTextTransDlg::OnClaimBtnClick)
	duEND_MESSAGE_MAP()
}

void CTextTransDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CTextTransDlg::OnDestroy()
{
	m_shadow_win.follow();
	CDialogEx::OnDestroy();
	// TODO:  在此处添加消息处理程序代码
}
