// ImgTransDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ImgTransDlg.h"
#include "afxdialogex.h"
#include "CtxFeedbackWnd.h"
#include "typedef.h"
#include "FileMgr.h"
#include "biz/TransnBizApi.h"
#include "PromptDialog.h"

// CImgTransDlg 对话框
extern USER_DATA2::DATA g_LoginData;
extern CDownloader g_Downloader;

IMPLEMENT_DYNAMIC(CImgTransDlg, CDialogEx)

CImgTransDlg::CImgTransDlg(CWnd* pParent /*=NULL*/, const TASK_INFO2& task)
: CDialogEx(CImgTransDlg::IDD, pParent), m_Task(task), m_bClaim(FALSE)
{

}

CImgTransDlg::~CImgTransDlg()
{
}

void CImgTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImgTransDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_MESSAGE(WM_DOWNLOAD_NOTIFY, OnDownloadNotify)
	ON_WM_SYSCOMMAND()
	ON_WM_MOVING()
	ON_WM_SIZE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CImgTransDlg 消息处理程序


int CImgTransDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("图片任务"));

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("img_trans_window")))
		return -1;

	m_shadow_win.create(GetSafeHwnd());
	// TODO:  在此添加您专用的创建代码


	if (!m_editInput.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP /* WS_VSCROLL | WS_HSCROLL */ | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, CRect(0, 0, 0, 0), this, 9999))
		return -1;

	m_editInput.SetPlaceHolderColor(RGB(217, 217, 217));
	m_editInput.SetTextColor(RGB(102, 102, 102));
	m_editInput.SetBackColor(RGB(255, 255, 255));

	duHwndObj* editHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("input_hwndobj"));
	if (editHwndObj)
		editHwndObj->Attach(m_editInput.GetSafeHwnd());

	if (!m_ImageWnd.CreateEx(0, _T("PictureWnd"), NULL, WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0, 0))
		return -1;

	duHwndObj* pImgboxHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("imgbox_hwndobj"));
	if (pImgboxHwndObj)
	{
		pImgboxHwndObj->Attach(m_ImageWnd.GetSafeHwnd());
	}


	if (!m_ImgToolWnd.Create(_T("ImgToolWnd"), NULL, WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), GetDesktopWindow(), 0, 0))
		return -1;

	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 160, rc.top + 400, 0, 0, SWP_NOSIZE);
	m_ImgToolWnd.SetAttach(m_ImageWnd.GetSafeHwnd());

	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szFile[MAX_PATH] = { 0 };
	StrCpy(szFile, conv::s2wcs(m_Task.originalUrl));
	PathRemoveArgs(szFile);
	_stprintf_s(szPath, MAX_PATH, _T("%s\\%s"),
		FileMgr::GetAppPathById(conv::s2wcs(g_LoginData.userId), _T("task")).GetBuffer(),
		PathFindFileName(szFile));
	g_Downloader.Add(conv::s2wcs(m_Task.originalUrl), szPath, GetSafeHwnd());

	//m_ImageWnd.LoadImage(conv::s2wcs(m_Task.hdpicUrl));

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


void CImgTransDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	::InvalidateRect(m_ImageWnd.GetSafeHwnd(), NULL, TRUE);
	CDialogEx::OnSysCommand(nID, lParam);
}

LRESULT CImgTransDlg::OnDownloadNotify(WPARAM wParam, LPARAM lParam)
{
	LPTSTR pPath = (LPTSTR)lParam;
	m_ImageWnd.LoadImage(pPath);
	free(pPath);
	return 0;
}

LRESULT CImgTransDlg::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
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


LRESULT CImgTransDlg::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CImgTransDlg::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CImgTransDlg, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CImgTransDlg::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CImgTransDlg::OnMinBtnClick)
		duON_MESSAGE(_T("submit_button"), DUM_BTNCLICK, &CImgTransDlg::OnSubmitBtnClick)
		duON_MESSAGE(_T("feedback_button"), DUM_BTNCLICK, &CImgTransDlg::OnFeedBtnClick)
		duON_MESSAGE(_T("claim_button"), DUM_BTNCLICK, &CImgTransDlg::OnClaimBtnClick)
		duON_MESSAGE(_T("rotate_button*"), DUM_BTNCLICK, &CImgTransDlg::OnRotateBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CImgTransDlg::OnFeedBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	CCtxFeedbackWnd* pWnd = new CCtxFeedbackWnd;
	if (!pWnd->CreateEx(0, _T("CtxFeedbackWnd"), NULL, WS_POPUP | WS_CHILD , CRect(0, 0, 0, 0), this, 0, 0))
		return -1;

	pWnd->ShowWindow(SW_SHOW);
	pWnd->UpdateWindow();
	return 0;
}

LRESULT CImgTransDlg::OnSubmitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
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

LRESULT CImgTransDlg::OnClaimBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{

// 	TASK_OPERATE op;
// 	op.op_type = OPERATE_TYPE::OT_RECEIVE;
// 	op.user_id = g_LoginData.userId;
// 	op.task_id = base::stdcxx_ws2s(m_Task.taskId);
// 	Json::Value ret;
// 	TransnBizApi::OperateTask(op, ret);

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
		duButton* pFeedbackBtn= (duButton*)GetPluginByName(GetSafeHwnd(), _T("feedback_button"));
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

LRESULT CImgTransDlg::OnRotateBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	::PostMessage(m_ImageWnd.GetSafeHwnd(), WM_IMG_ROTATE, 0, 0);
	return 0;
}

void CImgTransDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CImgTransDlg::OnDestroy()
{
	m_shadow_win.follow();
	CDialogEx::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


void CImgTransDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnMoving(fwSide, pRect);
	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 160, rc.top + 400, 0, 0, SWP_NOSIZE);

	// TODO:  在此处添加消息处理程序代码
}


void CImgTransDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 160, rc.top + 400, 0, 0, SWP_NOSIZE);

	// TODO:  在此处添加消息处理程序代码
}


void CImgTransDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	RECT rc = { 0 };
	GetWindowRect(&rc);
	::SetWindowPos(m_ImgToolWnd, NULL, rc.left + 160, rc.top + 400, 0, 0, SWP_NOSIZE);
	// TODO:  在此处添加消息处理程序代码
}
