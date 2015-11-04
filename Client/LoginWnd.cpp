// LoginWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "LoginWnd.h"
#include "PictureWnd.h"
#include "biz/TransnBizApi.h"
#include "PromptDialog.h"
#include "TaskNotifyWnd.h"
#include "typedef.h"
#include "MainFrm.h"
#include <atlconv.h>
#include "../Core/conv/conv.h"
#include "biz/TransnBizApi.h"
#include <regex>

using namespace conv;
// CLoginWnd

USER_DATA2::DATA g_LoginData;

IMPLEMENT_DYNAMIC(CLoginWnd, CDialogEx)

CLoginWnd::CLoginWnd(CWnd* pParent/* = NULL*/)
: CDialogEx(CLoginWnd::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLoginWnd::~CLoginWnd()
{
}

BEGIN_MESSAGE_MAP(CLoginWnd, CDialogEx)
	ON_WM_CREATE()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_RESTORE_STYLE, OnRestoreStyle)
END_MESSAGE_MAP()

int CLoginWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	BOOL bRet = WindowManager_Attach(theApp.GetWindowManager(), GetSafeHwnd(), _T("login_window"));
	if (!bRet)
		return -1;

	SetWindowText(_T("MyEcho"));

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_shadow_win.create(this->GetSafeHwnd());

	if (!m_userEdit.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0, 0, 0, 0), this, 9999))
		return -1;
	m_userEdit.SetPlaceHolderColor(RGB(217, 217, 217));
	m_userEdit.SetTextColor(RGB(102, 102, 102));
	m_userEdit.SetBackColor(RGB(255, 255, 255));
	m_userEdit.SetPlaceHolder(_T(" 账号"));
	m_userEdit.ShowPlaceHolder(TRUE);
	duHwndObj* userEditHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("user_edit_hwndobj"));
	if (userEditHwndObj)
		userEditHwndObj->Attach(m_userEdit.GetSafeHwnd());
	
	if (!m_pwdEdit.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, CRect(0, 0, 0, 0), this, 8888))
		return -1;
	m_pwdEdit.SetPlaceHolderColor(RGB(217, 217, 217));
	m_pwdEdit.SetTextColor(RGB(102, 102, 102));
	m_pwdEdit.SetBackColor(RGB(255, 255, 255));
	m_pwdEdit.SetPassword(TRUE);
	m_pwdEdit.SetPlaceHolder(_T(" 密码"));
	m_pwdEdit.ShowPlaceHolder(TRUE);
	duHwndObj* pwdEditHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("pwd_edit_hwndobj"));
	if (pwdEditHwndObj)
		pwdEditHwndObj->Attach(m_pwdEdit.GetSafeHwnd());

	if (check_login())
	{
		m_userEdit.ShowPlaceHolder(FALSE);
		m_pwdEdit.ShowPlaceHolder(FALSE);
		m_userEdit.SetWindowText(conv::s2wcs(login_name));
		m_pwdEdit.SetWindowText(conv::s2wcs(login_pwd));
		
	}

	return 0;
}

LRESULT CLoginWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CLoginWnd, wParam, lParam)
		duON_MESSAGE(_T("login_button"), DUM_BTNCLICK, &CLoginWnd::OnLoginBtnClick)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CLoginWnd::OnCloseBtnClick)
		duON_MESSAGE(_T("minimize_button"), DUM_BTNCLICK, &CLoginWnd::OnMinimizeBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CLoginWnd::OnRestoreStyle(WPARAM wParam, LPARAM lParam)
{
	SetErrStyle(FALSE);
	m_userEdit.SetWindowTextW(_T(""));
	m_pwdEdit.SetWindowTextW(_T(""));
	return 0;
}

void CLoginWnd::SetErrStyle(BOOL bErr)
{
	duPlugin* pUserEdit = GetPluginByName(GetSafeHwnd(), _T("user_edit_layout"));
	duPlugin* pPwdEdit = GetPluginByName(GetSafeHwnd(), _T("pwd_edit_layout"));
	if (pUserEdit && pPwdEdit)
	{
		pUserEdit->SetStyle(bErr ? _T("edit_err_layout") : _T("edit_layout"));
		pPwdEdit->SetStyle(bErr ? _T("edit_err_layout") : _T("edit_layout"));
		Plugin_Redraw(pUserEdit, FALSE);
		Plugin_Redraw(pPwdEdit, FALSE);
	}
}

static inline bool IsValidMail(const std::wstring& mail)
{
	std::wregex pattern(L"([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
	return std::regex_match(mail, pattern);
}

static inline bool IsValidPhone(const std::wstring& phone)
{
	std::wregex pattern(L"^[01][0-9]{10}$");
	return std::regex_match(phone, pattern);
}

LRESULT CLoginWnd::OnLoginBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	Json::Value jsnRes;
	int nErrCode = ERR_SUCCESS;
	CString strUser, strPwd;
	m_userEdit.GetWindowTextW(strUser);
	m_pwdEdit.GetWindowTextW(strPwd);

	if (strUser.IsEmpty() || strPwd.IsEmpty())
	{
		SetErrStyle(TRUE);
		m_userEdit.SetError(TRUE);
		m_pwdEdit.SetError(TRUE);
		return 0;
	}

	if (!IsValidMail((LPCTSTR)strUser) && !IsValidPhone((LPCTSTR)strUser))
	{
		SetErrStyle(TRUE);
		m_userEdit.SetError(TRUE);
		m_pwdEdit.SetError(TRUE);
		return 0;
	}

// 	strUser = _T("ter@163.com");
// 	strPwd = _T("123456");

	USES_CONVERSION;
	LOGIN_DATA loginData;
	loginData.loginName = conv::ws2s((LPCTSTR)strUser);
	loginData.password = conv::ws2s((LPCTSTR)strPwd);

	if (IsValidMail((LPCTSTR)strUser))
		loginData.loginType = LT_EMAIL;
	else
		loginData.loginType = LT_PHONE;

	USER_DATA2 userData;
	nErrCode = TransnBizApi::Login2(loginData, userData);
	if (nErrCode != ERR_SUCCESS)
	{
		if (nErrCode == ERR_INVALID || nErrCode == ERR_NOTEXIST)
		{
			// 登录错误
			SetErrStyle(TRUE);
			m_userEdit.SetError(TRUE);
			m_pwdEdit.SetError(TRUE);
			return 0;
		}
		else
		{
			
			CPromptDialog dialog(this, _T("提示"), _T("网络异常。"), CPromptDialog::PROMPT_OK);
			dialog.DoModal();
		}
	}
	else
	{
		g_LoginData = userData.data;

		login_name = loginData.loginName;
		login_pwd = loginData.password;

		save_login();

		ShowWindow(SW_HIDE);
		EndDialog(0);

		CMainFrame* pFrame = new CMainFrame;
		if (!pFrame)
			return FALSE;

		theApp.m_pMainWnd = pFrame;

		if (!pFrame->LoadFrame(IDR_MAINFRAME))
			return FALSE;
		pFrame->ShowWindow(SW_SHOW);
		pFrame->UpdateWindow();
		//pFrame->RunModalLoop(0);
	}

	return 0;
}

LRESULT CLoginWnd::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(0);
	return 0;
}

LRESULT CLoginWnd::OnMinimizeBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

BOOL CLoginWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类
	WNDCLASS wndclass;

	if (!__super::PreCreateWindow(cs))
		return FALSE;

	cs.hMenu = NULL;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndclass);
	wndclass.lpszClassName = _T("MyEchoLoginWnd");
	VERIFY(AfxRegisterClass(&wndclass));
	cs.lpszClass = wndclass.lpszClassName;

	return TRUE;
}

void CLoginWnd::OnDestroy()
{
	m_shadow_win.follow();
	__super::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}

void CLoginWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}

BOOL CLoginWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_TAB)
		{
			if (*GetFocus() == m_userEdit)
			{
				m_pwdEdit.SetFocus();
			}
			else if (*GetFocus() == m_pwdEdit)
			{
// 				duPlugin* pLoginBtn = GetPluginByName(GetSafeHwnd(), _T("login_button"));
// 				Plugin_SetFocus(pLoginBtn, TRUE);
			}
		}
		if (pMsg->wParam == VK_RETURN)
		{
			OnLoginBtnClick(NULL, NULL, NULL);
		}
	}

	return __super::PreTranslateMessage(pMsg);
}


BOOL CLoginWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetForegroundWindow();
	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
