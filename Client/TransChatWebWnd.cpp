// TransChatWebWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "TransChatWebWnd.h"
#include "../base/win/shadow_window.h"
#include "include/Cef_browser.h"
#include "config.h"
#include "ClientV8ExtHandler.h"
#include "FileMgr.h"
#include "../../Core/conv/conv.h"
#include "biz/TransnBizApi.h"
#include "PromptDialog.h"

extern USER_DATA2::DATA g_LoginData;
extern SESSION_DATA g_SessionData;

// CTransChatWebWnd

IMPLEMENT_DYNAMIC(CTransChatWebWnd, CDialogEx)

CTransChatWebWnd::CTransChatWebWnd(CWnd* pParent) :
	CDialogEx(CTransChatWebWnd::IDD, pParent),
	m_nSecCount(0), m_bEndChat(FALSE)
{
	ZeroMemory(g_SessionData.voice_path, MAX_PATH);
}

CTransChatWebWnd::~CTransChatWebWnd()
{

}

BEGIN_MESSAGE_MAP(CTransChatWebWnd, CDialogEx)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_MESSAGE(WM_RECORD_START, OnRecordStart)
	ON_MESSAGE(WM_RECORD_STOP, OnRecordStop)
	ON_MESSAGE(WM_RECORD_SAVE, OnRecordSave)
	ON_MESSAGE(WM_TRANSEND, OnTransEnd)
	ON_WM_NCDESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTransChatWebWnd 消息处理程序

BOOL CTransChatWebWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此添加专用代码和/或调用基类
	WNDCLASS wndclass;
	cs.hMenu = NULL;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndclass);
	wndclass.lpszClassName = _T("TransChatWebWnd");
	VERIFY(AfxRegisterClass(&wndclass));
	cs.lpszClass = wndclass.lpszClassName;
	return CWnd::PreCreateWindow(cs);
}

void CTransChatWebWnd::InitExtension()
{
	std::string Ext = "var Cef;"
		"if (!Cef)"
		"  Cef = {};"
		"if (!Cef.Ext)"
		"  Cef.Ext = {};"
		"if (!Cef.User)"
		"  Cef.User = {};"
		"(function() {"
		"  Cef.Ext.__defineGetter__('test_param', function() {"
		"    native function GetTestParam();"
		"    return GetTestParam();"
		"  });"
		"  Cef.Ext.__defineSetter__('test_param', function(b) {"
		"    native function SetTestParam();"
		"    if (b) SetTestParam(b);"
		"  });"
		"  Cef.Ext.test_object = function() {"
		"    native function GetTestObject();"
		"    return GetTestObject();"
		"  };"
		"  Cef.Ext.SelectFileDlg = function() {"
		"    native function SelectFileDlg();"
		"    return SelectFileDlg();"
		"  };"
		"  Cef.Ext.StartRecord = function() {"
		"    native function StartRecord();"
		"    return StartRecord();"
		"  };"
		"  Cef.Ext.CancelRecord = function() {"
		"    native function CancelRecord();"
		"    return CancelRecord();"
		"  };"
		"  Cef.Ext.SaveRecord = function() {"
		"    native function SaveRecord();"
		"    return SaveRecord();"
		"  };"
		"  Cef.Ext.SendRecord = function() {"
		"    native function SendRecord();"
		"    return SendRecord();"
		"  };"
		"  Cef.Ext.GetVoicePath = function() {"
		"    native function GetVoicePath();"
		"    return GetVoicePath();"
		"  };"
		"  Cef.Ext.GetVoiceData = function() {"
		"    native function GetVoiceData();"
		"    return GetVoiceData();"
		"  };"
		" Cef.Ext.GetFileSize = function(file) {"
		"    native function GetFileSize();"
		"    return GetFileSize(file);"
		"  };"
		" Cef.Ext.GetVoiceSize = function() {"
		"    native function GetVoiceSize();"
		"    return GetVoiceSize();"
		"  };"
		" Cef.Ext.GetVoiceLength = function(file) {"
		"    native function GetVoiceLength();"
		"    return GetVoiceLength(file);"
		"  };"
		"Cef.Ext.PlayAudio = function(url, file){"
		"    native function PlayAudio();"
		"    return PlayAudio(url, file)"
		"  };"
		"Cef.Ext.StopAudio = function(){"
		"    native function StopAudio();"
		"    return StopAudio()"
		"  };"
		"Cef.Ext.ViewImage = function(url){"
		"    native function ViewImage();"
		"    return ViewImage(url)"
		"  };"
		"Cef.User.GetUserId = function(){"
		"    native function GetUserId();"
		"    return GetUserId()"
		"  };"
		"Cef.User.GetUserPwd = function(){"
		"    native function GetUserPwd();"
		"    return GetUserPwd()"
		"  };"
		"Cef.User.GetCustomerId = function(){"
		"    native function GetCustomerId();"
		"    return GetCustomerId()"
		"  };"
		"Cef.User.GetFirstSentence = function(){"
		"    native function GetFirstSentence();"
		"    return GetFirstSentence()"
		"  };"
		"Cef.User.GetCustomerPwd = function(){"
		"    native function GetCustomerPwd();"
		"    return GetCustomerPwd()"
		"  };"
		"})();";

	CefRegisterExtension("v8/Ext", Ext, new ClientV8ExtHandler());//Ext表示的字符串是任意合法的JS代码
}

void RunExtensionTest(CefRefPtr<CefBrowser> browser) {
	std::string html =
		"<html><body>ClientV8ExtensionHandler says:<br><pre>"
		"<script language=\"JavaScript\">"
		"Cef.test.test_param ="
		"  'Assign and retrieve a value succeeded the first time.';"
		"document.writeln(Cef.test.test_param);"
		"Cef.test.test_param ="
		"  'Assign and retrieve a value succeeded the second time.';"
		"document.writeln(Cef.test.test_param);"
		"var obj = Cef.test.test_object();"
		"document.writeln(obj.param);"
		"document.writeln(obj.GetMessage());"
		"document.writeln(Cef.test.loadurl(\"hello world.\"));"
		"document.writeln(Cef.test.IndicatorsCloud(123456789, \"leadsec.\"));"
		"</script>"
		"</pre></body></html>";
	browser->GetMainFrame()->LoadString(html, "about:blank");
}

void CTransChatWebWnd::ExecuteJavascript(const std::wstring& strCode)
{
	if (m_cWebClient.get())
	{
		CefRefPtr<CefFrame> frame = m_cWebClient->GetBrowser()->GetMainFrame();
		if (frame.get())
		{
			CefString strCode(strCode.c_str()), strUrl(L"");
			frame->ExecuteJavaScript(strCode, strUrl, 0);
		}
	}
}

int CTransChatWebWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), _T("trans_chat_web_wnd")))
		return -1;
	
	SetWindowText(_T("专人翻译"));

	g_SessionData.chat_hwnd = GetSafeHwnd();

	m_shadow_win.create(GetSafeHwnd());

	m_Recorder.Attach(GetSafeHwnd());

	//m_pV8Exthandler = new ClientV8ExtHandler(this);

	CefRefPtr<CefWebClient> client(new CefWebClient());
	m_cWebClient = client;

	CefSettings cSettings;
	CefSettingsTraits::init(&cSettings);
	cSettings.multi_threaded_message_loop = true;
	CefRefPtr<CefApp> spApp;
	CefInitialize(cSettings, spApp);

	//CefRegisterExtension("v8/Ext", s_JsExt, m_pV8Exthandler);

	duHwndObj* pWebHwndobj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("chat_web_hwndobj"));
	RECT rc = { 0, 0, 0, 0 };
	pWebHwndobj->GetRect(&rc);
	CefWindowInfo info;
	info.SetAsChild(GetSafeHwnd(), rc);

	TCHAR szHtml[MAX_PATH] = { 0 };
	::GetModuleFileName(GetModuleHandle(NULL), szHtml, _countof(szHtml));
	::PathRemoveFileSpec(szHtml);
	::PathAppend(szHtml, _T("html\\trans.html"));

	CefBrowserSettings browserSettings;
	CefBrowser::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(client),
		szHtml, browserSettings);

	InitExtension();
	//RunExtensionTest(m_cWebClient->GetBrowser());

	//pWebHwndobj->Attach(m_cWebClient->GetBrowser()->GetWindowHandle());

	//m_Recorder.StartRecord();
	
	//StrCpy(m_szVoicePath, _T("C:\\Users\\Administrator\\AppData\\Roaming\\MyEcho\\oDHO7Q3LRUtxLg4E9R1adjrsv5irzYa8\\task\\test.amr"));

	return 0;
}


LRESULT CTransChatWebWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CTransChatWebWnd, wParam, lParam)
		duON_MESSAGE(_T("close_button"), DUM_BTNCLICK, &CTransChatWebWnd::OnCloseBtnClick)
		duON_MESSAGE(_T("min_button"), DUM_BTNCLICK, &CTransChatWebWnd::OnMinBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CTransChatWebWnd::OnRecordStart(WPARAM wParam, LPARAM lParam)
{
	m_Recorder.StartRecord();
	return 0;
}

#include "../Core/system/SystemInfo.h"

LRESULT CTransChatWebWnd::OnRecordStop(WPARAM wParam, LPARAM lParam)
{
	m_Recorder.StopRecord();
	return 0;
}

LRESULT CTransChatWebWnd::OnRecordSave(WPARAM wParam, LPARAM lParam)
{
	m_Recorder.StopRecord();
	TCHAR szPath_Wav[MAX_PATH] = { 0 };
	
	_stprintf_s(szPath_Wav, MAX_PATH, _T("%s\\%s.%s"),
		FileMgr::GetAppPathById(conv::s2wcs(g_LoginData.userId), _T("voice")).GetBuffer(),
		SystemInfo::GetUnixTimeStamp().c_str(), _T("wav"));

	m_Recorder.SaveRecord(szPath_Wav);

	TCHAR tzConverter[MAX_PATH] = { 0 };
	::GetModuleFileName(GetModuleHandle(NULL), tzConverter, _countof(tzConverter));
	::PathRemoveFileSpec(tzConverter);
	::PathAppend(tzConverter, _T("WavToAMR.dll"));

	TCHAR szPath_AMR[MAX_PATH] = { 0 };
	StrCpy(szPath_AMR, szPath_Wav);
	PathRemoveExtension(szPath_AMR);
	PathAddExtension(szPath_AMR, _T(".amr"));

	HINSTANCE  hModule = LoadLibrary(tzConverter);
	typedef int(*__fnWavToAMR)(const char* intfile, const char* outfile, int channels, int bits_per_samp);
	__fnWavToAMR WavToAMR = (__fnWavToAMR)::GetProcAddress(hModule, "WavToAMR");
	if (WavToAMR)
	{
		WavToAMR(base::stdcxx_ws2s(szPath_Wav).c_str(), base::stdcxx_ws2s(szPath_AMR).c_str(), 1, 16);
	}

	DeleteFile(szPath_Wav);

	StrCpy(g_SessionData.voice_path, szPath_AMR);
	return 0;
}

LRESULT CTransChatWebWnd::OnTransEnd(WPARAM wParam, LPARAM lParam)
{
	//DestroyWindow();
	m_bEndChat = TRUE;
	ExecuteJavascript(_T("closeServer()"));
	SetTimer(TIMER_AUTO_CLOSE, 1000, NULL);
	return 0;
}

LRESULT CTransChatWebWnd::OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	if (m_bEndChat)
	{
		EndDialog(0);
		g_SessionData.has_close_wnd = true;
		g_SessionData.chat_hwnd = NULL;
	}
	else
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

			EndDialog(0);
			g_SessionData.has_close_wnd = true;
			//::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_CLOSETRANS, 0, 0);

			g_SessionData.chat_hwnd = NULL;
			TRANS_OPERATE op;
			op.conversation_id = g_SessionData.conversation_id;
			op.nick_name = g_LoginData.nickName;
			op.userId = g_LoginData.userId;
			op.type = TOT_CLOSE;

			TRANS_RESULT res;
			TransnBizApi::OperateTrans(op, res);

		}
	}

	return 0;
}

LRESULT CTransChatWebWnd::OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_MINIMIZE);
	return 0;
}

void CTransChatWebWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CWnd::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
	// TODO:  在此处添加消息处理程序代码
}

void CTransChatWebWnd::OnDestroy()
{
	m_shadow_win.follow();
	CWnd::OnDestroy();
	// TODO:  在此处添加消息处理程序代码
}

void CTransChatWebWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

//	if (m_cWebClient.get())
// 	{
// 		CefRefPtr<CefBrowser> browser = m_cWebClient->GetBrowser();
// 		if (browser)
// 		{
// // 			CefWindowHandle hwnd = browser->GetWindowHandle();
// // 			duHwndObj* pWebHwndobj = (duHwndObj*)GetPluginByName(GetSafeHwnd(), _T("chat_web_hwndobj"));
// // 			RECT rc;
// // 			pWebHwndobj->GetRect(&rc);
// // 			// ::SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, SWP_NOZORDER);  
// // 			::MoveWindow(hwnd, 0, 0, rc.left, rc.top, true);
// 		}
// 	}
	// TODO:  在此处添加消息处理程序代码
}


void CTransChatWebWnd::OnNcDestroy()
{
	CWnd::OnNcDestroy();
	// TODO:  在此处添加消息处理程序代码
}


void CTransChatWebWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case TIMER_AUTO_CLOSE:
	{
		if (m_nSecCount == AUTO_CLOSE_WND_TIME)
		{
			KillTimer(nIDEvent);
			EndDialog(0);
		}
		else
		{
			m_nSecCount++;
			TCHAR szTitle[MAX_PATH] = { 0 };
			_stprintf_s(szTitle, _countof(szTitle), _T("会话已结束，%d 秒自动关闭"), AUTO_CLOSE_WND_TIME - m_nSecCount);
			duWindow* pWnd = (duWindow*)GetPluginByName(GetSafeHwnd(), _T("trans_chat_web_wnd"));
			if (pWnd)
			{
				pWnd->SetText(szTitle);
				Plugin_Redraw(pWnd, FALSE);
			}

		}

		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
