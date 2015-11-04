
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Client.h"

#include "MainFrm.h"
#include "typedef.h"
#include "TrayMenuWnd.h"
#include "FileMgr.h"
#include "DulibEx.h"
#include "biz/TransnBizApi.h"
#include "../base/str/stdcxx.h"
#include "PromptDialog.h"
#include "LoginWnd.h"
#include "../base/files/file_path.h"
#include "../base/cmdline/command_line.h"
#include "../base/process/launch.h"
#include "TransChatWnd.h"
#include "TransChatWebWnd.h"
#include "config.h"

#include "TextTransDlg.h"
#include "ImgTransDlg.h"
#include "VoiceTransDlg.h"
#include "biz/TransnBizApi.h"
#include "TaskNotifyWnd.h"
#include "NewerTaskWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
extern USER_DATA2::DATA g_LoginData;
extern const PTCHAR LANG_ABBR_LIST[];

SESSION_DATA g_SessionData;

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND(ID_FILE_CLOSE, &CMainFrame::OnFileClose)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SYSTRAY, OnSystemTray)
	ON_MESSAGE(WM_DOWNLOAD_NOTIFY, OnDownloadNotify)
	ON_MESSAGE(WM_SHOWMAINWND, OnShowMainWnd)
	ON_MESSAGE(WM_CLEAN, OnClean)
	ON_MESSAGE(WM_CHECK_UPDATE, OnCheckUpdate)
	ON_MESSAGE(WM_LOGOUT, OnLogout)
	ON_MESSAGE(WM_EXIT_APP, OnExitApp)
	ON_WM_SYSCOMMAND()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_NCDESTROY()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

Locker CMainFrame::m_locker;

CMainFrame::CMainFrame() :
	//m_mqMicro(NULL),
	//m_queMicro(NULL),
	//m_mqTrans(NULL),
	//m_queTrans(NULL),
	m_bSendSwitch(FALSE)
{
	// TODO:  在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
	StopMqThread_Micro();
	StopMqThread_Trans();
	StopMqThread_TransEnd();
	m_Tray.Close();
	
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	WNDCLASS wndclass;
	cs.hMenu = NULL;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndclass);
	wndclass.lpszClassName = _T("MyEchoMainFrame");
	VERIFY(AfxRegisterClass(&wndclass));
	cs.lpszClass = wndclass.lpszClassName;
	return TRUE;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作
	if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	ModifyStyleEx(0, WS_EX_TOOLWINDOW);
	SetWindowText(_T("MyEcho"));

	CWinApp* pApp = AfxGetApp();
	if (pApp->m_pMainWnd == NULL)
		pApp->m_pMainWnd = this;

	return TRUE;
}

void CMainFrame::OnFileClose()
{
	DestroyWindow();
}

void CMainFrame::InitLangRights()
{
	for (unsigned int i = 0; i < g_LoginData.languages.size(); i++)
	{
		m_bsLangRights.set(g_LoginData.languages[i].langId, g_LoginData.languages[i].right);
	}

	for (UINT i = 0; i < m_bsLangRights.size(); i++)
	{
		if (m_bsLangRights[i])
		{
			m_vtLangSkills.Add(i);
		}
	}

}

void CMainFrame::UpdateLangSkill()
{
	duStatic* langSkill1 = (duStatic*)GetPluginByName(GetSafeHwnd(), _T("lang_skill_static1"));
	duStatic* langSkill2 = (duStatic*)GetPluginByName(GetSafeHwnd(), _T("lang_skill_static2"));
	if (langSkill1 && langSkill2)
	{
		int nCount = m_vtLangSkills.GetSize();
		switch (nCount)
		{
		case 0:
			break;
		case 1:
		{
			langSkill1->SetVisible(TRUE);
			LPCTSTR langAbbr1 = LANG_ABBR_LIST[m_vtLangSkills.GetAt(0)];
			langSkill1->SetText(langAbbr1);
			Plugin_Redraw(langSkill1, FALSE);
			break;
		}
		case 2:
		{
			langSkill1->SetVisible(TRUE);
			langSkill2->SetVisible(TRUE);
			LPCTSTR langAbbr1 = LANG_ABBR_LIST[m_vtLangSkills.GetAt(0)];
			LPCTSTR langAbbr2 = LANG_ABBR_LIST[m_vtLangSkills.GetAt(1)];
			langSkill1->SetText(langAbbr1);
			langSkill2->SetText(langAbbr2);
			Plugin_Redraw(langSkill1, FALSE);
			Plugin_Redraw(langSkill2, FALSE);
		}
		default:
			break;
		}
	}

}

void CMainFrame::InitLanguageCbx()
{
	duCheckBox* pLangCbx1 = (duCheckBox*)GetPluginByName(GetSafeHwnd(), _T("language_cbx1"));
	pLangCbx1->SetParam(LAN_ENGLISH);
	duCheckBox* pLangCbx2 = (duCheckBox*)GetPluginByName(GetSafeHwnd(), _T("language_cbx2"));
	pLangCbx2->SetParam(LAN_JAPANESE);
	duCheckBox* pLangCbx3 = (duCheckBox*)GetPluginByName(GetSafeHwnd(), _T("language_cbx3"));
	pLangCbx3->SetParam(LAN_KOREAN);
	if (pLangCbx1 && pLangCbx2 && pLangCbx3)
	{
		m_vtLangCbx.push_back(pLangCbx1);
		m_vtLangCbx.push_back(pLangCbx2);
		m_vtLangCbx.push_back(pLangCbx3);
	}
	
	for (INT_PTR i = 0; i < m_vtLangSkills.GetSize(); i++)
	{
		for (UINT j = 0; j < m_vtLangCbx.size(); j++)
		{
			if (m_vtLangCbx[j]->GetParam() == m_vtLangSkills[i])
			{
				m_vtLangCbx[j]->SetChecked(TRUE);
				Plugin_Redraw(m_vtLangCbx[j], FALSE);
				break;
			}
		}
	}

// 	const int MAX_SELECT = 2;
// 	int nSelected = 0;
// 	for (UINT i = 0; i < m_vtLangCbx.size(); i++)
// 	{
// 		nSelected += m_vtLangCbx[i]->IsChecked() ? 1 : 0;
// 	}
// 	if (nSelected >= MAX_SELECT)
// 	{
// 		for (UINT i = 0; i < m_vtLangCbx.size(); i++)
// 		{
// 			m_vtLangCbx[i]->SetDisable(!m_vtLangCbx[i]->IsChecked());
// 			Plugin_Redraw(m_vtLangCbx[i], FALSE);
// 		}
// 	}
}

void CMainFrame::InitInstantCbx()
{
	duCheckBox* pCheckBox = (duCheckBox*)GetPluginByName(GetSafeHwnd(), _T("instant_trans_cbx"));
	if (pCheckBox)
	{
		pCheckBox->SetDisable(m_bSendSwitch == -1);
		Plugin_Redraw(pCheckBox, FALSE);
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	if (!WindowManager_Attach(theApp.GetWindowManager(), GetSafeHwnd(), _T("main_window")))
		return -1;

	duStatic* pNickname = (duStatic*)GetPluginByName(GetSafeHwnd(), _T("nickname_static"));
	if (pNickname)
	{
		pNickname->SetText(conv::s2wcs(g_LoginData.nickName));
		Plugin_Redraw(pNickname, FALSE);
	}

	if (g_LoginData.headPhoto == "" || g_LoginData.headPhoto.empty())
	{
		duPlugin* pAvatarBtn = GetPluginByName(GetSafeHwnd(), _T("avatar_button"));
		duResManager* pResMgr = theApp.GetWindowManager()->GetResManager();
		duStyleGroup* pStyleGroup = (duStyleGroup *)pResMgr->GetResObj(pAvatarBtn->GetStyle(), DU_RES_STYLEGROUP);
		duImageStyle *pImageStyle = (duImageStyle *)pStyleGroup->GetStyle(0);
		if (!pImageStyle)
			return FALSE;
		pImageStyle->SetPicFile(_T("default_avatar"));
		Plugin_Redraw(pAvatarBtn, FALSE);

	}
	else
	{
		CString strAvatar = FileMgr::GetAppPathById(conv::s2wcs(g_LoginData.userId), _T("avatar"));
		LPTSTR lpFileName = PathFindFileName(conv::s2wcs(g_LoginData.headPhoto));
		strAvatar.Append(_T("\\"));
		strAvatar.Append(lpFileName);
		g_Downloader.Add(conv::s2wcs(g_LoginData.headPhoto), strAvatar, GetSafeHwnd());
	}
	
	m_shadow_win.create(this->GetSafeHwnd());

	m_Tray.Create(GetSafeHwnd(), IDR_MAINFRAME, WM_SYSTRAY, _T("MyEcho"));
	m_Tray.Show();


// 	TCHAR szHtml[MAX_PATH] = { 0 };
// 	::GetModuleFileName(GetModuleHandle(NULL), szHtml, _countof(szHtml));
// 	::PathRemoveFileSpec(szHtml);
// 	::PathAppend(szHtml, _T("/html\\index.html"));

	m_pWebBrowser = (CWebBrowser*)RUNTIME_CLASS(CWebBrowser)->CreateObject();
	RECT rectDummy = { 0 };
	if (!m_pWebBrowser->Create(NULL, _T(""), WS_CHILD | WS_OVERLAPPED | WS_VISIBLE/* | WS_CLIPCHILDREN*/, rectDummy, this, 9990))
		return -1;
	m_pWebBrowser->Navigate2(base::stdcxx_s2ws(client_config_faq_web).c_str());
	duHwndObj* faqBrowserHwndobj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("browser_hwndobj"));
	if (faqBrowserHwndobj)
	{
		faqBrowserHwndobj->Attach(m_pWebBrowser->GetSafeHwnd());
	}
	
	InitLangRights();

	m_bSendSwitch = g_LoginData.send_switch;

	UpdateLangSkill();

	InitLanguageCbx();
	InitInstantCbx();

	duCheckBox* pInstantCbx = (duCheckBox*)GetPluginByName(GetSafeHwnd(), _T("instant_trans_cbx"));
	if (pInstantCbx)
	{
		pInstantCbx->SetChecked(m_bSendSwitch == TRUE);
		Plugin_Redraw(pInstantCbx, FALSE);
	}

	// create check unfinish task thread.
	CreateChkUnfinishThread();

	// ceraet mq thread
	if (pInstantCbx->IsChecked())
	{
		InitMqThreads();
	}
		

// 	CefRefPtr<CefWebClient> client(new CefWebClient());
// 	m_faqWebBrowser = client;
// 
// 	CefSettings cSettings;
// 	CefSettingsTraits::init(&cSettings);
// 	cSettings.multi_threaded_message_loop = true;
// 	CefRefPtr<CefApp> spApp;
// 	CefInitialize(cSettings, spApp);
// 
// 	duHwndObj* pWebHwndobj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("browser_hwndobj"));
// 
// 	if (!pWebHwndobj)
// 		return -1;
// 	RECT rc = { 0, 0, 0, 0 };
// 	pWebHwndobj->GetRect(&rc);
// 	CefWindowInfo info;
// 	info.SetAsChild(GetSafeHwnd(), rc);
// 
// 	CefBrowserSettings browserSettings;
// 	CefBrowser::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(client),
// 		szHtml, browserSettings);

	//pWebHwndobj->Attach(m_faqWebBrowser->GetBrowser()->GetWindowHandle());

	return 0;
}

DWORD WINAPI CMainFrame::S_ChkUnfinishTask(void* lpVoid)
{
	//m_locker.Lock();
	CMainFrame* pThis = (CMainFrame*)lpVoid;
	UNFINISH_TASK task;
	TransnBizApi::GetUnfinishTask2(g_LoginData.userId, task);
	if (task.result)
	{
		if (!task.is_trans)
		{
			switch (task.task_type)
			{
			case TT_IMG:
			{
				CImgTransDlg transDlg(GetDesktopWindow(), task);
				transDlg.DoModal();
			}
			break;
			case TT_TEXT:
			{
				CTextTransDlg transDlg(GetDesktopWindow(), task);
				transDlg.DoModal();
			}
			break;
			case TT_VOICE:
			{
				CVoiceTransDlg transDlg(GetDesktopWindow(), task);
				transDlg.DoModal();
			}
			break;
			default:
				break;
			}
		}
		else
		{
			g_SessionData.conversation_id = task.conversation_id;
			g_SessionData.customer_id = task.customer_id;
			g_SessionData.first_sentence = "抱歉，客户端出现异常，现在恢复会话。";
			CTransChatWebWnd transDlg(GetDesktopWindow());
			transDlg.DoModal();
		}
		
	}
	//m_locker.Unlock();
	return 0;
}

void CMainFrame::CreateChkUnfinishThread()
{
	m_tpChkUnfinish.hEventHandle_ = CreateEvent(0, TRUE, FALSE, 0);
	m_tpChkUnfinish.hThreadHandle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CMainFrame::S_ChkUnfinishTask,
		reinterpret_cast<void*>(this), 0, NULL);
}

void CMainFrame::CreateMqThread_Mirco()
{
	//base::TAutoLock lock(m_tpMqMicro.lock_);
	m_tpMqMicro.hEventHandle_ = CreateEvent(0, TRUE, m_bSendSwitch, 0);
	m_tpMqMicro.hThreadHandle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CMainFrame::S_MqThread_Micro,
		reinterpret_cast<void*>(this), 0, NULL);
}

void CMainFrame::CreateMqThread_Trans()
{
	//base::TAutoLock lock(m_tpMqTrans.lock_);
	m_tpMqTrans.hEventHandle_ = CreateEvent(0, TRUE, m_bSendSwitch, 0);
	m_tpMqTrans.hThreadHandle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CMainFrame::S_MqThread_Trans,
		reinterpret_cast<void*>(this), 0, NULL);
}

void CMainFrame::CreateMqThread_TransEnd()
{
	//base::TAutoLock lock(m_tpMqTrans.lock_);
	m_tpMqTransEnd.hEventHandle_ = CreateEvent(0, TRUE, m_bSendSwitch, 0);
	m_tpMqTransEnd.hThreadHandle_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CMainFrame::S_MqThread_TransEnd,
		reinterpret_cast<void*>(this), 0, NULL);
}

void CMainFrame::StopMqThread_Trans()
{
	//base::TAutoLock lock(m_tpMqTrans.lock_);
	if (m_tpMqTrans.hEventHandle_ && m_tpMqTrans.hThreadHandle_)
	{
		SetEvent(m_tpMqTrans.hEventHandle_);
		WaitForSingleObject(m_tpMqTrans.hThreadHandle_, 500);
		TerminateThread(m_tpMqTrans.hThreadHandle_, 0);
	}
	m_tpMqTrans.Release();

}

void CMainFrame::StopMqThread_Micro()
{
	//base::TAutoLock lock(m_tpMqMicro.lock_);
	if (m_tpMqMicro.hEventHandle_ && m_tpMqMicro.hThreadHandle_)
	{
		SetEvent(m_tpMqMicro.hEventHandle_);
		WaitForSingleObject(m_tpMqMicro.hThreadHandle_, 500);
		TerminateThread(m_tpMqMicro.hThreadHandle_, 0);
	}
	m_tpMqMicro.Release();

}

void CMainFrame::StopMqThread_TransEnd()
{
	if (m_tpMqTransEnd.hEventHandle_ && m_tpChkUnfinish.hThreadHandle_)
	{
		SetEvent(m_tpMqTransEnd.hEventHandle_);
		WaitForSingleObject(m_tpMqTransEnd.hThreadHandle_, 500);
		TerminateThread(m_tpMqTransEnd.hThreadHandle_, 0);
	}
	m_tpMqTransEnd.Release();
}

void CMainFrame::StopChkUnfinishThread()
{
	if (m_tpChkUnfinish.hEventHandle_ && m_tpChkUnfinish.hThreadHandle_)
	{
		SetEvent(m_tpChkUnfinish.hEventHandle_);
		WaitForSingleObject(m_tpChkUnfinish.hThreadHandle_, 500);
		TerminateThread(m_tpChkUnfinish.hThreadHandle_, 0);
	}
	m_tpChkUnfinish.Release();
}


BOOL CMainFrame::FilterTask(const AMQP_TASK& task)
{
	for (unsigned int i = 0; i < task.user_list.size(); i++)
	{
		if (!strcmp(task.user_list[i].c_str(), g_LoginData.userId.c_str()))
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CMainFrame::DisplayTask_Micro(const TASK_INFO2& task)
{
	CTaskNotifyWnd notifyWnd(GetDesktopWindow(), CTaskNotifyWnd::TASKNOTIFY_MICROTASK);
	if (notifyWnd.DoModal() == CTaskNotifyWnd::TASKNOTIFY_IDVIEW)
	{
		switch (task.task_type)
		{
		case TT_IMG :
		{
			CImgTransDlg transDlg(GetDesktopWindow(), task);
			transDlg.DoModal();
		}
		break;
		case TT_TEXT:
		{
			CTextTransDlg transDlg(GetDesktopWindow(), task);
			transDlg.DoModal();
		}
		break;
		case TT_VOICE:
		{
			CVoiceTransDlg transDlg(GetDesktopWindow(), task);
			transDlg.DoModal();
		}
		break;
		default:
			break;
		}
	}
}

int CMainFrame::OnMqMessage_Micro(AMQPMessage* message)
{
	WaitForSingleObject(m_tpMqMicro.hEventHandle_, INFINITE);
	m_locker.Lock();
 	uint32_t len = 0;
	std::string header = message->getHeader("Content-encoding");
	std::string type = message->getHeader("Content-type");

	char* msg = message->getMessage(&len);
	std::string str_;
	str_.resize(len);
	memcpy((void*)str_.c_str(), msg, len);
	std::string str = base::stdcxx_utf8ts(str_);
	Json::Value value;
	Json::Reader reader;
	if (reader.parse(str, value))
	{
		AMQP_TASK_MICRO task;
		AMQP_TASK_MICRO::ParseFromJson(value, task);
		
		if (FilterTask(task))
		{
			TASK_INFO2 taskInfo;
			if (TransnBizApi::QueryTask2(task.task_id, taskInfo) == ERR_SUCCESS)
			{
				DisplayTask_Micro(taskInfo);
			}
		}
		
	}
	m_locker.Unlock();
	return 0;
}

void CMainFrame::DisplayTask_Trans(const AMQP_TASK_TRANS& task)
{
	CTaskNotifyWnd notifyWnd(GetDesktopWindow(), CTaskNotifyWnd::TASKNOTIFY_TRANS);
	if (notifyWnd.DoModal() == CTaskNotifyWnd::TASKNOTIFY_IDVIEW)
	{
		TRANS_OPERATE op;
		op.type = TOT_RECEIVE;
		op.userId = g_LoginData.userId;
		op.nick_name = g_LoginData.nickName;
		TRANS_RESULT result;
		if (TransnBizApi::OperateTrans(op, result) == ERR_SUCCESS)
		{
			if (result.result != 0)
			{
				g_SessionData.conversation_id = result.conversation_id;
				g_SessionData.customer_id = result.customer_id;
				g_SessionData.first_sentence = result.first_sentence;

				CTransChatWebWnd transDlg(GetDesktopWindow());
				transDlg.DoModal();

			}
			else
			{
				CPromptDialog dialog(this, _T("提醒"), conv::s2wcs(result.err_msg), CPromptDialog::PROMPT_OK);
				dialog.DoModal();
			}
			
		}
		else
		{
			CPromptDialog dialog(this, _T("提醒"), _T("网络异常"), CPromptDialog::PROMPT_OK);
			dialog.DoModal();
		}
	}
}

int CMainFrame::OnMqMessage_Trans(AMQPMessage* message)
{
	WaitForSingleObject(m_tpMqTrans.hEventHandle_, INFINITE);
	m_locker.Lock();

	uint32_t len = 0;
	std::string header = message->getHeader("Content-encoding");
	std::string type = message->getHeader("Content-type");

	char* msg = message->getMessage(&len);
	std::string str_;
	str_.resize(len);
	memcpy((void*)str_.c_str(), msg, len);
	std::string str = base::stdcxx_utf8ts(str_);
	Json::Value value;
	Json::Reader reader;
	if (reader.parse(str, value))
	{
		AMQP_TASK_TRANS task;
		AMQP_TASK_TRANS::ParseFromJson(value, task);
		if (FilterTask(task))
		{
			DisplayTask_Trans(task);
		}

	}

	m_locker.Unlock();
	return 0;
}

int CMainFrame::OnMqMessage_TransEnd(AMQPMessage* message)
{
	WaitForSingleObject(m_tpMqTransEnd.hEventHandle_, INFINITE);
	//m_locker.Lock();
	uint32_t len = 0;
	std::string header = message->getHeader("Content-encoding");
	std::string type = message->getHeader("Content-type");

	char* msg = message->getMessage(&len);
	std::string str_;
	str_.resize(len);
	memcpy((void*)str_.c_str(), msg, len);
	std::string str = base::stdcxx_utf8ts(str_);
	Json::Value value;
	Json::Reader reader;
	if (reader.parse(str, value))
	{
// 		AMQP_TASK_TRANS task;
// 		AMQP_TASK_TRANS::ParseFromJson(value, task);
// 		DisplayTask_Trans(task);
		AMQP_TRANS_END info;
		AMQP_TRANS_END::ParseFromJson(value, info);
		if (strcmp(info.to.c_str(), g_LoginData.userId.c_str()) == 0)
		{
			//HWND hWnd = ::FindWindow(_T("TransChatWebWnd"), NULL);
			//if (!g_SessionData.has_close_wnd)
			::PostMessage(g_SessionData.chat_hwnd, WM_TRANSEND, 0, 0);
		}

	}
	//m_locker.Unlock();
	return 0;
}

static Locker s_Locker;

int mq_callback_all(AMQPMessage * message)
{
	s_Locker.Lock();
	CMainFrame* pMainWnd = (CMainFrame*)theApp.m_pMainWnd;
	if (pMainWnd == NULL)
		return 0;
	std::string routingKey = message->getRoutingKey();

	if (lstrcmpiA(routingKey.c_str(), amqp_micro_key.c_str()) == 0)
	{
		return pMainWnd->OnMqMessage_Micro(message);
	}
	else if (lstrcmpiA(routingKey.c_str(), amqp_trans_key.c_str()) == 0)
	{
		return pMainWnd->OnMqMessage_Trans(message);
	}
	else if (lstrcmpiA(routingKey.c_str(), amqp_trans_close_key.c_str()) == 0)
	{
		return pMainWnd->OnMqMessage_TransEnd(message);
	}

	s_Locker.Unlock();
	return 0;
}

int mq_callback_micro(AMQPMessage * message)
{
	CMainFrame* pMainWnd= (CMainFrame*)theApp.m_pMainWnd;
	if (pMainWnd)
		return pMainWnd->OnMqMessage_Micro(message);
	else
		return -1;
}

int mq_callback_trans(AMQPMessage* message)
{
	CMainFrame* pMainWnd = (CMainFrame*)theApp.m_pMainWnd;
	if (pMainWnd)
		return pMainWnd->OnMqMessage_Trans(message);
	else
		return -1;
}

int mq_callback_trans_end(AMQPMessage* message)
{
	CMainFrame* pMainWnd = (CMainFrame*)theApp.m_pMainWnd;
	if (pMainWnd)
		return pMainWnd->OnMqMessage_TransEnd(message);
	else
		return -1;
}

// Peter J. Weinberger hash function
static inline long PJWHash(std::string str)
{
	long BitsInUnsignedInt = (long)(4 * 8);
	long ThreeQuarters = (long)((BitsInUnsignedInt * 3) / 4);
	long OneEighth = (long)(BitsInUnsignedInt / 8);
	long HighBits = (long)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	long hash = 0;
	long test = 0;
	for (unsigned int i = 0; i < str.length(); i++)
	{
		hash = (hash << OneEighth) + str.at(i);
		if ((test = hash & HighBits) != 0)
		{
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return hash;
}

void CMainFrame::InitMqThreads()
{
	const int kBufSize = 1024;
	char connStr[kBufSize] = { 0 };
	sprintf_s(connStr, kBufSize, "%s:%s@%s:%d", amqp_conn_pwd.c_str(), amqp_conn_user.c_str(),
		amqp_conn_host.c_str(), amqp_conn_port);

	try
	{
		std::unique_ptr<AMQP> ptrAMQP(new AMQP(connStr));
		m_ptrAMQP = std::move(ptrAMQP);


		std::unique_ptr<AMQP> ptrAMQP2(new AMQP(connStr));
		m_ptrAMQP2 = std::move(ptrAMQP2);

		std::unique_ptr<AMQP> ptrAMQP3(new AMQP(connStr));
		m_ptrAMQP3 = std::move(ptrAMQP3);

		std::string strQueName_Micro = g_LoginData.userId + "_Micro";
		std::string strQueName_Trans = g_LoginData.userId + "_Trans";
		std::string strQueName_TransEnd = g_LoginData.userId + "_TransEnd";

		std::unique_ptr<AMQPQueue> ptrQueMicro(m_ptrAMQP->createQueue(strQueName_Micro));
		m_ptrQueMicro = std::move(ptrQueMicro);
		m_ptrQueMicro->Declare(strQueName_Micro);
		m_ptrQueMicro->Bind(amqp_micro_exchange, amqp_micro_key);
		m_ptrQueMicro->addEvent(AMQP_MESSAGE, mq_callback_micro);

		std::unique_ptr<AMQPQueue> ptrQueTrans(m_ptrAMQP2->createQueue(strQueName_Trans));
		m_ptrQueTrans = std::move(ptrQueTrans);
		m_ptrQueTrans->Declare(strQueName_Trans);
		m_ptrQueTrans->Bind(amqp_trans_exchange, amqp_trans_key);
		m_ptrQueTrans->addEvent(AMQP_MESSAGE, mq_callback_trans);

		std::unique_ptr<AMQPQueue> ptrQueTransEnd(m_ptrAMQP3->createQueue(strQueName_TransEnd));
		m_ptrQueTransEnd = std::move(ptrQueTransEnd);
		m_ptrQueTransEnd->Declare(strQueName_TransEnd);
		m_ptrQueTransEnd->Bind(amqp_trans_close_exchange, amqp_trans_close_key);
		m_ptrQueTransEnd->addEvent(AMQP_MESSAGE, mq_callback_trans_end);

		CreateMqThread_Mirco();
		CreateMqThread_Trans();
		CreateMqThread_TransEnd();
	}
	catch (...)
	{
		
	}

}

DWORD WINAPI CMainFrame::S_MqThread_Micro(void* lpVoid)
{
	CMainFrame* pThis = (CMainFrame*)lpVoid;
	try
	{
		pThis->m_ptrQueMicro->Consume(AMQP_NOACK);
	}
	catch (...)
	{
		
	}
	
	return 0;
}

DWORD WINAPI CMainFrame::S_MqThread_Trans(void* lpVoid)
{
	CMainFrame* pThis = (CMainFrame*)lpVoid;
	try
	{
		pThis->m_ptrQueTrans->Consume(AMQP_NOACK);
	}
	catch (...)
	{

	}
	return 0;
}

DWORD WINAPI CMainFrame::S_MqThread_TransEnd(void* lpVoid)
{
	CMainFrame* pThis = (CMainFrame*)lpVoid;
	try
	{ 
		pThis->m_ptrQueTransEnd->Consume(AMQP_NOACK);
	}
	catch (...)
	{

	}

	return 0;
}

LRESULT CMainFrame::OnShowMainWnd(WPARAM wParam, LPARAM lParam)
{
	ShowWindow( IsIconic() ? SW_RESTORE : SW_SHOW);
	SetForegroundWindow();
	return 0;
}

LRESULT CMainFrame::OnCloseTrans(WPARAM wParam, LPARAM lParam)
{
	
	return 0;
}

LRESULT CMainFrame::OnClean(WPARAM wParam, LPARAM lParam)
{
	FileMgr::ClearAppPath();
	CPromptDialog dialog(this, _T("提示"), _T("清理完成。"), CPromptDialog::PROMPT_OK);
	dialog.DoModal();
	return 0;
}
LRESULT CMainFrame::OnCheckUpdate(WPARAM wParam, LPARAM lParam)
{
	CPromptDialog dialog(GetDesktopWindow(), _T("更新"), _T("当前ME为最新版本，无需进行更新！"), CPromptDialog::PROMPT_OK);
	dialog.DoModal();
	return 0;
}

LRESULT CMainFrame::OnLogout(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_HIDE);
	//PostQuitMessage(0);
	OnAppExit();
	//g_LoginData.Clear();
	//CLoginWnd loginWnd;
	//ReleaseMutex(theApp.m_hMutex);
	//Sleep(3000);

	log_off();

	base::LaunchOptions lo;
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, sizeof(szPath) / sizeof(TCHAR));
	base::FilePath fp(szPath);
	base::CommandLine new_process(fp);
	base::LaunchProcess(new_process, lo, 0);
	return 0;
}

LRESULT CMainFrame::OnExitApp(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_HIDE);
	DestroyWindow();
	return 0;
}

LRESULT CMainFrame::OnDownloadNotify(WPARAM wParam, LPARAM lParam)
{
	LPTSTR pPath = (LPTSTR)lParam;
	if (PathFileExists(pPath))
	{
		duPlugin* pAvatarBtn = GetPluginByName(GetSafeHwnd(), _T("avatar_button"));
		DulibEx::SetPluginPic(theApp.GetWindowManager(), pAvatarBtn, pPath);
	}
	free(pPath);
	return 0;
}

LRESULT CMainFrame::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CMainFrame, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CMainFrame::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CMainFrame::OnMinBtnClick)
		duON_MESSAGE(_T("top_menu_button"), DUM_BTNCLICK, &CMainFrame::OnTopMenuButton)
		duON_MESSAGE(_T("change_language_btn"), DUM_CKCLICK, &CMainFrame::OnChangeLanguageBtnClick)
		duON_MESSAGE(_T("setting_save_button"), DUM_BTNCLICK, &CMainFrame::OnSettingSaveBtnClick)
		duON_MESSAGE(_T("setting_cancel_button"), DUM_BTNCLICK, &CMainFrame::OnSettingCancelBtnClick)
		duON_MESSAGE(_T("language_cbx*"), DUM_CKCLICK, &CMainFrame::OnLanguageCbxClick)
		duON_MESSAGE(_T("instant_trans_cbx"), DUM_CKCLICK, &CMainFrame::OnInstantTransCbxClick)
	duEND_MESSAGE_MAP()
}

LRESULT CMainFrame::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

LRESULT CMainFrame::OnTopMenuButton(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	CTopMenuWnd* pTopMenuWnd = new CTopMenuWnd;
	pTopMenuWnd->CreateEx(WS_OVERLAPPED, _T("TopMenuWnd"), NULL, NULL, 0, 0, 0, 0, GetSafeHwnd(), NULL);
	pTopMenuWnd->ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CMainFrame::OnSystemTray(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_RBUTTONUP)
	{
		CTrayMenuWnd* pTrayWnd = new CTrayMenuWnd;
		pTrayWnd->Create(_T("TrayMenuWnd"), NULL, NULL, CRect(0, 0, 0, 0), GetDesktopWindow(), 8870, NULL);
		pTrayWnd->ShowWindow(SW_SHOW);
	}
	else if (lParam == WM_LBUTTONUP)
	{
		ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
		SetForegroundWindow();
	}
	return 0;
}

void CMainFrame::OnAppExit()
{
	DestroyWindow();
	PostQuitMessage(0);
}

LRESULT CMainFrame::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	//CPromptDialog promptDlg()
	OnAppExit();
	return 0;
}

void CMainFrame::ShowSettingPanel(BOOL bShow)
{
	duPlugin* pWeb = GetPluginByName(GetSafeHwnd(), _T("web_layout"));
	duPlugin* pSetting = GetPluginByName(GetSafeHwnd(), _T("setting_layout"));
	duHwndObj* pHwndObj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("browser_hwndobj"));
	if (pWeb && pSetting && pHwndObj)
	{
		::ShowWindow(m_pWebBrowser->GetSafeHwnd(), bShow ? SW_HIDE : SW_SHOW);
		pWeb->SetVisible(!bShow);
		pSetting->SetVisible(bShow);
		Plugin_Redraw(pWeb, FALSE);
		Plugin_Redraw(pSetting, FALSE);
	}

}

#include "biz/EasemobBizApi.h"
LRESULT CMainFrame::OnChangeLanguageBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowSettingPanel((BOOL)wParam);
	return 0;
}

const static PTCHAR LANG_ABBR_LIST[] =
{
	_T(""), _T("中"), _T("英"), _T("日"), _T("法"),
	_T("德"), _T("俄"), _T("韩"), _T("繁"), _T("荷"),
	_T("意"), _T("西"), _T("葡"), _T("阿"), _T("土"),
};

LRESULT CMainFrame::OnSettingSaveBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowSettingPanel(FALSE);
	std::vector<UINT> vtSelected;
	for (UINT i = 0; i < m_vtLangCbx.size(); i++)
	{
		if (m_vtLangCbx[i]->IsChecked())
		{
			vtSelected.push_back(m_vtLangCbx[i]->GetParam());
			
		}
	}
	return 0;
}

LRESULT CMainFrame::OnSettingCancelBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowSettingPanel(FALSE);
	return 0;
}

LRESULT CMainFrame::OnInstantTransCbxClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	m_bSendSwitch = ((duCheckBox*)pPlugin)->IsChecked();
	if (m_bSendSwitch)
	{
		SetEvent(m_tpMqMicro.hEventHandle_);
		SetEvent(m_tpMqTrans.hEventHandle_);
		SetEvent(m_tpMqTransEnd.hThreadHandle_);
	}
	else
	{
		ResetEvent(m_tpMqMicro.hEventHandle_);
		ResetEvent(m_tpMqTrans.hEventHandle_);
		ResetEvent(m_tpMqTransEnd.hEventHandle_);
	}
	return 0;
}

LRESULT CMainFrame::OnLanguageCbxClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
// 	const int MAX_SELECT = 2;
// 	int nSelected = 0;
// 	for (UINT i = 0; i < m_vtLangCbx.size(); i++)
// 	{
// 		nSelected += m_vtLangCbx[i]->IsChecked() ? 1 : 0;
// 	}
// 	if (nSelected >= MAX_SELECT)
// 	{
// 		for (UINT i = 0; i < m_vtLangCbx.size(); i++)
// 		{
// 			m_vtLangCbx[i]->SetDisable(!m_vtLangCbx[i]->IsChecked());
// 			Plugin_Redraw(m_vtLangCbx[i], FALSE);
// 		}
// 	}
// 	else
// 	{
// 		for (UINT i = 0; i < m_vtLangCbx.size(); i++)
// 		{
// 			m_vtLangCbx[i]->SetDisable(FALSE);
// 			Plugin_Redraw(m_vtLangCbx[i], FALSE);
// 		}
// 	}

	INT_PTR nLangID = pPlugin->GetParam();
	if (!m_bsLangRights[nLangID] && (BOOL)wParam == TRUE)
	{
		CPromptDialog dialog(this, _T("提示"), _T("您暂时没有该语种权限，是否进行测试？"), CPromptDialog::PROMPT_OK);
		if (dialog.DoModal() == CPromptDialog::PROMPT_IDOK)
		{
			CNewerTaskWnd* pNewerTaskWnd = new CNewerTaskWnd;
			pNewerTaskWnd->CreateEx(NULL, _T("NewerTaskWnd"), _T(""),
				WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 500, 600, GetSafeHwnd(), NULL, NULL);

		}
		((duCheckBox*)pPlugin)->SetChecked(FALSE);
	}
	else
	{
		((duCheckBox*)pPlugin)->SetChecked(TRUE);
	}

	return 0;
}

void CMainFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CFrameWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}


void CMainFrame::OnDestroy()
{
	//m_shadow_win.follow();
	CFrameWnd::OnDestroy();
	// TODO:  在此处添加消息处理程序代码
}


void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID)
	{
	case SC_CLOSE:
		OnAppExit();
		return;
	default:
		break;
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	return CFrameWnd::PreTranslateMessage(pMsg);
}


void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	
	duPlugin* pInstantCbx = GetPluginByName(GetSafeHwnd(), _T("instant_trans_cbx"));
	if (pInstantCbx && pInstantCbx->IsDisable())
	{
		RECT rc = { 0 };
		pInstantCbx->GetRect(&rc);
		if (PtInRect(&rc, point))
		{
			duPlugin* pPrompt = GetPluginByName(GetSafeHwnd(), _T("prompt_static"));
			if (pPrompt)
			{
				pPrompt->SetVisible(TRUE);
				Plugin_Redraw(pPrompt, FALSE);
				SetTimer(TIMER_SHOW_PROMPT, 3 * 1000, NULL);
			}
		}
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case TIMER_SHOW_PROMPT:
	{
		KillTimer(nIDEvent);
		duPlugin* pPrompt = GetPluginByName(GetSafeHwnd(), _T("prompt_static"));
		if (pPrompt)
		{
			pPrompt->SetVisible(FALSE);
			Plugin_Redraw(pPrompt, FALSE);
		}
		break;
	}
	default:
		break;
	}
	CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::OnNcDestroy()
{
	
	CFrameWnd::OnNcDestroy();
	delete this;
	// TODO:  在此处添加消息处理程序代码
}


void CMainFrame::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类

	CFrameWnd::PostNcDestroy();
}
