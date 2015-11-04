// PromptDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "PromptDialog.h"
#include "afxdialogex.h"


// CPromptDialog 对话框

IMPLEMENT_DYNAMIC(CPromptDialog, CDialogEx)

CPromptDialog::CPromptDialog(CWnd* pParent /*= NULL*/, LPCTSTR lpszTitle /*= NULL*/, LPCTSTR lpszText /*= NULL*/, UINT uType /*= PROMPT_OK*/)
: CDialogEx(CPromptDialog::IDD, pParent), m_strTitle(lpszTitle), m_strText(lpszText), m_uType(uType)
{

}

CPromptDialog::~CPromptDialog()
{
}

void CPromptDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPromptDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
//	ON_WM_NCDESTROY()
ON_WM_DESTROY()
END_MESSAGE_MAP()

int CPromptDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("prompt_window")))
		return FALSE;

	RECT rcWnd;
	SIZE sz;
	::GetTextExtentPoint32(GetDC()->m_hDC, (LPCTSTR)m_strText, m_strText.GetLength(), &sz);
	GetWindowRect(&rcWnd);
	if (sz.cx + 60 > 300)
		SetWindowPos(NULL, 0, 0, sz.cx + 100 , rcWnd.bottom - rcWnd.top, SWP_NOMOVE);

	m_shadow_win.create(this->GetSafeHwnd());

	duPlugin* pWnd = GetPluginByName(GetSafeHwnd(), _T("prompt_window"));
	duPlugin* pText = GetPluginByName(GetSafeHwnd(), _T("text_static"));

	if (pWnd && pText)
	{
		pWnd->SetText(m_strTitle);
		pText->SetText(m_strText);
		Plugin_Redraw(pWnd, FALSE);
		Plugin_Redraw(pText, FALSE);
	}

	ShowCtrlsGroup(m_uType);

	return 0;
}

LRESULT CPromptDialog::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CPromptDialog, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CPromptDialog::OnCloseBtnClick)
		duON_MESSAGE(_T("ok_button"), DUM_BTNCLICK, &CPromptDialog::OnOkBtnClick)
		duON_MESSAGE(_T("abort_button*"), DUM_BTNCLICK, &CPromptDialog::OnAbortBtnClick)
		duON_MESSAGE(_T("continue_button*"), DUM_BTNCLICK, &CPromptDialog::OnContinueBtnClick)
		duON_MESSAGE(_T("min2tray_button"), DUM_BTNCLICK, &CPromptDialog::OnMin2TrayBtnClick)
		duON_MESSAGE(_T("exit_button"), DUM_BTNCLICK, &CPromptDialog::OnExitBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CPromptDialog::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(PROMPT_IDCLOSE);
	return 0;
}

LRESULT CPromptDialog::OnOkBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(PROMPT_IDOK);
	return 0;
}

LRESULT CPromptDialog::OnAbortBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(PROMPT_IDABORT);
	return 0;
}

LRESULT CPromptDialog::OnContinueBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(PROMPT_IDCONTINUE);
	return 0;
}

LRESULT CPromptDialog::OnMin2TrayBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(PROMPT_IDMIN2TRAY);
	return 0;
}

LRESULT CPromptDialog::OnExitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(PROMPT_IDEXIT);
	return 0;
}

void CPromptDialog::ShowCtrlsGroup(UINT uType)
{
	std::map<int, duPlugin*> mapLayout;
	{
		mapLayout.insert(std::make_pair(PROMPT_OK, GetPluginByName(GetSafeHwnd(), _T("ok_layout"))));
		mapLayout.insert(std::make_pair(PROMPT_ABORT_MICROTASK, GetPluginByName(GetSafeHwnd(), _T("abort_microtask_layout"))));
		mapLayout.insert(std::make_pair(PROMPT_ABORT_SPECIALIST, GetPluginByName(GetSafeHwnd(), _T("abort_specialist_layout"))));
		mapLayout.insert(std::make_pair(PROMPT_EXIT, GetPluginByName(GetSafeHwnd(), _T("exit_layout"))));
		//mapLayout.insert(std::make_pair(PROMPT_UPDATE, GetPluginByName(GetSafeHwnd(), _T("prompt_update"))));
	}

	std::map<int, duPlugin*>::iterator itIdx = mapLayout.find(uType);
	for (std::map<int, duPlugin*>::iterator it = mapLayout.begin(); it != mapLayout.end(); ++it)
	{
		if (it != itIdx)
		{
			it->second->SetVisible(FALSE);
			Plugin_Redraw(it->second, FALSE);
		}
	}
	itIdx->second->SetVisible(TRUE);
	Plugin_Redraw(itIdx->second, FALSE);
}

BOOL CPromptDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			EndDialog(PROMPT_IDCLOSE);
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPromptDialog::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
}

void CPromptDialog::OnDestroy()
{
	m_shadow_win.follow();
	CDialogEx::OnDestroy();
}
