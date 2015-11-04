// CTransChatWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "TransChatWnd.h"

// CTransChatWnd

IMPLEMENT_DYNAMIC(CTransChatWnd, CWnd)

CTransChatWnd::CTransChatWnd() : m_pChatList(NULL)
{

}

CTransChatWnd::~CTransChatWnd()
{
}

BEGIN_MESSAGE_MAP(CTransChatWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

// CTransChatWnd 消息处理程序

int CTransChatWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	// TODO:  在此添加您专用的创建代码
	BOOL bRet = WindowManager_Attach(theApp.GetWindowManager(), GetSafeHwnd(), _T("trans_chat_window"));
	if (!bRet)
		return -1;

	m_shadow_win.create(GetSafeHwnd());
	m_Recorder.Attach(GetSafeHwnd());

	CRect rcDummy;
	rcDummy.SetRectEmpty();

	if (!m_editView.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL, rcDummy, this, IDC_RICHEDIT_VIEW))
	{
		TRACE(_T("Create RichEdit View Failed\n"));
		return -1;
	}

	if (!m_editInput.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL, rcDummy, this, IDC_RICHEDIT_INPUT))
	{
		TRACE(_T("Create RichEdit Input Failed\n"));
		return -1;
	}

	m_editView.SetReadOnly(TRUE);
	InitCharFormat();
	FormatInputEdit();

	duHwndObj *pHwndObj_editView = (duHwndObj *)GetPluginByName(m_hWnd, _T("talk_richedit_view"));
	pHwndObj_editView->Attach(m_editView.GetSafeHwnd());

	duHwndObj *pHwndObj_editInput = (duHwndObj *)GetPluginByName(m_hWnd, _T("talk_richedit_input"));
	pHwndObj_editInput->Attach(m_editInput.GetSafeHwnd());

// 	m_pChatList = (duListCtrl*)GetPluginByName(GetSafeHwnd(), _T("chat_listctrl"));
// 	if (!m_pChatList)
// 		return -1;
// 
// 	duLayout* pUserMsgTemplate = (duLayout*)GetPluginByName(GetSafeHwnd(), _T("user_msg_template_layout"));
// 	if (!pUserMsgTemplate)
// 		return -1;
// 	duLayout* pCustomerMsgTemplate = (duLayout*)GetPluginByName(GetSafeHwnd(), _T("customer_msg_template_layout"));
// 	if (!pCustomerMsgTemplate)
// 		return -1;
// 
// 	if (!InitChatAvatar())
// 		return -1;

	return 0;
}

void CTransChatWnd::InitCharFormat()
{
	ZeroMemory(&m_cfNow, sizeof(CHARFORMAT));
	m_cfNow.cbSize = sizeof(CHARFORMAT);

// 	duComboBox *pFontCombo = (duComboBox *)GetPluginByName(m_hWnd, _T("talk_combobox_font"));
// 	duComboBox *pFontSizeCombo = (duComboBox *)GetPluginByName(m_hWnd, _T("talk_combobox_fontsize"));

// 	if (pFontCombo == NULL || pFontSizeCombo == NULL)
// 		return;

	m_cfNow.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_ITALIC;
	m_cfNow.yHeight = 12 * 20;

#if _MSC_VER >= 1400
	_tcscpy_s(m_cfNow.szFaceName, _T("微软雅黑"));
#else
	_tcscpy(m_cfNow.szFaceName, pFontCombo->GetText());
#endif

	m_cfNow.crTextColor = RGB(0, 0, 0);  // default color

	// view time-line char format
	ZeroMemory(&m_cfViewTime, sizeof(CHARFORMAT));
	m_cfViewTime.cbSize = sizeof(CHARFORMAT);

	m_cfViewTime.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_SIZE | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_ITALIC;
	m_cfViewTime.crTextColor = RGB(0, 0, 0);
	m_cfViewTime.yHeight = 12 * 20;

#if _MSC_VER >= 1400
	_tcscpy_s(m_cfViewTime.szFaceName, _T("微软雅黑"));
#else
	_tcscpy(m_cfViewTime.szFaceName, _T("Arial"));
#endif
}

void CTransChatWnd::FormatInputEdit()
{
	m_editInput.SetSel(0, -1);
	m_editInput.SetSelectionCharFormat(m_cfNow);
	m_editInput.SetSel(0, 0);
}

BOOL CTransChatWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)::DefWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = ::AfxGetInstanceHandle();
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("TransChatWnd");
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);
	return CWnd::PreCreateWindow(cs);
}

BOOL CTransChatWnd::InitChatAvatar()
{
	CString sUserAvatar = _T("C:\\Users\\Administrator\\Desktop\\QQ截图20150826180050.jpg");
	CString sCustomerAvatar = _T("C:\\Users\\Administrator\\Desktop\\psb.jpg");
	duButton* pUserAvatarBtn = (duButton*)GetPluginByName(GetSafeHwnd(), _T("user_avatar_button"));
	duButton* pCustomerAvatarBtn = (duButton*)GetPluginByName(GetSafeHwnd(), _T("customer_avatar_button"));
	if (!pUserAvatarBtn || !pCustomerAvatarBtn)
		return FALSE;
	
	TCHAR szUserResName[MAX_PATH] = { 0 };
	TCHAR szCustomerResName[MAX_PATH] = { 0 };
	::StrCpy(szUserResName, ::PathFindFileName(sUserAvatar));
	::StrCpy(szCustomerResName, ::PathFindFileName(sCustomerAvatar));
	
	::CharLower(szUserResName);
	::CharLower(szCustomerResName);
	_stprintf_s(szUserResName, _countof(szUserResName), _T("avatar_%s"), szUserResName);
	_stprintf_s(szCustomerResName, _countof(szCustomerResName), _T("avatar_%s"), szCustomerResName);


	duWindowManager* pWindMgr = theApp.GetWindowManager();
	
	if (!pWindMgr)
		return FALSE;
	duResManager* pResMgr = pWindMgr->GetResManager();

	if (!pResMgr)
		return FALSE;
	duImage* pUserAvatarRes = (duImage*)pResMgr->CreateResObj(szUserResName, DU_RES_IMAGE);
	duImage* pCustomerAvatarRes = (duImage*)pResMgr->CreateResObj(szCustomerResName, DU_RES_IMAGE);
	if (!pUserAvatarRes || !pCustomerAvatarRes)
		return FALSE;
	if (pUserAvatarRes->LoadFromFile(sUserAvatar) && pCustomerAvatarRes->LoadFromFile(sCustomerAvatar))
	{
		if (pResMgr->AddResObj(pUserAvatarRes) && pResMgr->AddResObj(pCustomerAvatarRes))
		{
			duStyleGroup* pUserBtnStyleGroup = (duStyleGroup *)pResMgr->GetResObj(_T("user_avatar_button"), DU_RES_STYLEGROUP);
			duStyleGroup* pCustomerBtnStyleGroup = (duStyleGroup*)pResMgr->GetResObj(_T("customer_avatar_button"), DU_RES_STYLEGROUP);
			if (!pUserBtnStyleGroup || !pCustomerBtnStyleGroup)
				return FALSE;

			duImageStyle *pUserBtnStyle = (duImageStyle *)pUserBtnStyleGroup->GetStyle(0);
			duImageStyle* pCustomerBtnStyle = (duImageStyle*)pCustomerBtnStyleGroup->GetStyle(0);
			if (!pUserBtnStyle && !pCustomerBtnStyle)
				return FALSE;

			pUserBtnStyle->SetPicFile(szUserResName);
			pCustomerBtnStyle->SetPicFile(szCustomerResName);
		}
	}
	else
	{
// 		pUserAvatarRes->Release();
// 		pCustomerAvatarRes->Release();
		return FALSE;
	}

	return TRUE;
}

BOOL CTransChatWnd::StartRecord()
{
	return TRUE;
}

BOOL CTransChatWnd::StopRecord()
{
	return FALSE;
}

void CTransChatWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
}

void CTransChatWnd::OnDestroy()
{
	m_shadow_win.follow();
	CWnd::OnDestroy();
}

void CTransChatWnd::OnNcDestroy()
{
	CWnd::OnNcDestroy();
	delete this;
}
