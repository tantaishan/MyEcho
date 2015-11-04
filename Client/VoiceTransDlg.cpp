// VoiceTransDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "VoiceTransDlg.h"
#include "afxdialogex.h"
#include "FileMgr.h"
#include "typedef.h"
#include "biz/TransnBizApi.h"
#include "PromptDialog.h"

extern USER_DATA2::DATA g_LoginData;
extern CDownloader g_Downloader;

// CVoiceTransDlg 对话框

IMPLEMENT_DYNAMIC(CVoiceTransDlg, CDialogEx)

CVoiceTransDlg::CVoiceTransDlg(CWnd* pParent /*=NULL*/, const TASK_INFO2& task)
	: CDialogEx(CVoiceTransDlg::IDD, pParent),
	m_Task(task),
	m_bClaim(FALSE),
	m_bDownload(0),
	m_dwPlayTick(0)
{

}

CVoiceTransDlg::~CVoiceTransDlg()
{
	m_player.Stop();
}

void CVoiceTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVoiceTransDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_WM_SIZE()
	ON_MESSAGE(WM_DOWNLOAD_NOTIFY, OnDownloadNotify)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CVoiceTransDlg 消息处理程序


int CVoiceTransDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("语音任务"));

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("voice_trans_wnd")))
		return -1;

	if (!m_editInput.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP /* WS_VSCROLL | WS_HSCROLL */ | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, CRect(0, 0, 0, 0), this, 9999))
		return -1;

	m_editInput.SetPlaceHolderColor(RGB(217, 217, 217));
	m_editInput.SetTextColor(RGB(102, 102, 102));
	m_editInput.SetBackColor(RGB(255, 255, 255));

	duHwndObj* editHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("input_hwndobj"));
	if (editHwndObj)
		editHwndObj->Attach(m_editInput.GetSafeHwnd());

	m_shadow_win.create(GetSafeHwnd());


	TCHAR szPath[MAX_PATH] = { 0 };
	_stprintf_s(szPath, MAX_PATH, _T("%s\\%s"),
		FileMgr::GetAppPathById(conv::s2wcs(g_LoginData.userId), _T("task")).GetBuffer(),
		PathFindFileName(conv::s2wcs(m_Task.context)));

	g_Downloader.Add(conv::s2wcs(m_Task.context), szPath, GetSafeHwnd());
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

LRESULT CVoiceTransDlg::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CVoiceTransDlg, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CVoiceTransDlg::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CVoiceTransDlg::OnMinBtnClick)
		duON_MESSAGE(_T("claim_button"), DUM_BTNCLICK, &CVoiceTransDlg::OnClaimBtnClick)
		duON_MESSAGE(_T("play_button"), DUM_BTNCLICK, &CVoiceTransDlg::OnPlayBtnClick)
		duON_MESSAGE(_T("stop_button"), DUM_BTNCLICK, &CVoiceTransDlg::OnStopBtnClick)
		duON_MESSAGE(_T("submit_button"), DUM_BTNCLICK, &CVoiceTransDlg::OnSubmitBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CVoiceTransDlg::OnDownloadNotify(WPARAM wParam, LPARAM lParam)
{
	LPTSTR pPath = (LPTSTR)lParam;
	m_bDownload = TRUE;
	m_player.Open(pPath);
	SetTimer(TIMER_VOICE_PLAYEND, m_player.Length(), NULL);
	free(pPath);
	return 0;
}

LRESULT CVoiceTransDlg::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
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
}

LRESULT CVoiceTransDlg::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CVoiceTransDlg::OnSubmitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
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

LRESULT CVoiceTransDlg::OnClaimBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	TASK_OPERATE2 op;
	op.taskId = m_Task.task_id;
	op.userId = g_LoginData.userId;
	op.type = OT_RECEIVE;
	OT_RESULT res;
	TransnBizApi::OperateTask2(op, res);
	if (res.result == 0)
	{
		m_bClaim = FALSE;
		CPromptDialog dialog(this, _T("提醒"), conv::s2wcs(res.msg), CPromptDialog::PROMPT_OK);
		dialog.DoModal();
		duButton* pClaimBtn = (duButton*)GetPluginByName(GetSafeHwnd(), _T("claim_button"));
		duButton* pFeedbackBtn = (duButton*)GetPluginByName(GetSafeHwnd(), _T("feedback_button"));
		pClaimBtn->SetDisable(TRUE);
		pFeedbackBtn->SetDisable(TRUE);
		Plugin_Redraw(pClaimBtn, FALSE);
		Plugin_Redraw(pFeedbackBtn, FALSE);
		EndDialog(0);
	}
	else
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

LRESULT CVoiceTransDlg::OnPlayBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	if (m_bDownload)
	{
		m_player.Play();
		SetTimer(TIMER_VOICE_PLAYEND, m_player.Length(), NULL);
		duPlugin* pPlay = GetPluginByName(GetSafeHwnd(), _T("play_button"));
		duPlugin* pStop = GetPluginByName(GetSafeHwnd(), _T("stop_button"));
		if (pPlay && pStop)
		{
			pPlay->SetVisible(FALSE);
			pStop->SetVisible(TRUE);
			Plugin_Redraw(pPlay, FALSE);
			Plugin_Redraw(pStop, FALSE);
		}
	}

	return 0;
}

LRESULT CVoiceTransDlg::OnStopBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	if (m_bDownload)
	{
		m_player.Stop();

		duPlugin* pPlay = GetPluginByName(GetSafeHwnd(), _T("play_button"));
		duPlugin* pStop = GetPluginByName(GetSafeHwnd(), _T("stop_button"));
		if (pPlay && pStop)
		{
			pPlay->SetVisible(TRUE);
			pStop->SetVisible(FALSE);
			Plugin_Redraw(pPlay, FALSE);
			Plugin_Redraw(pStop, FALSE);
		}
	}
	return 0;
}


void CVoiceTransDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CVoiceTransDlg::OnDestroy()
{
	m_shadow_win.follow();
	CDialogEx::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


void CVoiceTransDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//m_wndGif.MoveWindow(0, 0, cx, cy);
	// TODO:  在此处添加消息处理程序代码
}


void CVoiceTransDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case TIMER_VOICE_PLAYEND:
	{
		KillTimer(nIDEvent);
		duPlugin* pPlay = GetPluginByName(GetSafeHwnd(), _T("play_button"));
		duPlugin* pStop = GetPluginByName(GetSafeHwnd(), _T("stop_button"));
		if (pPlay && pStop)
		{
			pPlay->SetVisible(TRUE);
			pStop->SetVisible(FALSE);
			Plugin_Redraw(pPlay, FALSE);
			Plugin_Redraw(pStop, FALSE);
		}
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
