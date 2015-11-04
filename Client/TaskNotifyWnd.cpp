// MicroTaskNotifyWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "TaskNotifyWnd.h"
#include "afxdialogex.h"

// CMicroTaskNotifyWnd 对话框
IMPLEMENT_DYNAMIC(CTaskNotifyWnd, CDialogEx)

CTaskNotifyWnd::CTaskNotifyWnd(CWnd* pParent /*=NULL*/, UINT uType/* = TASKNOTIFY_MICROTASK*/)
: CDialogEx(CTaskNotifyWnd::IDD, pParent), m_uType(uType)
{

}

CTaskNotifyWnd::~CTaskNotifyWnd()
{
}

void CTaskNotifyWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTaskNotifyWnd, CDialogEx)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DIRECTUI_NOTIFY, OnDuiNotify)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CMicroTaskNotifyWnd 消息处理程序
int CTaskNotifyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

LRESULT CTaskNotifyWnd::OnDuiNotify(WPARAM wParam, LPARAM lParam)
{
	duBEGIN_MESSAGE_MAP(CTaskNotifyWnd, wParam, lParam)
		duON_MESSAGE(_T("ignore_button"), DUM_BTNCLICK, &CTaskNotifyWnd::OnIgnoreBtnClick)
		duON_MESSAGE(_T("view_button"), DUM_BTNCLICK, &CTaskNotifyWnd::OnViewBtnClick)
	duEND_MESSAGE_MAP()
}

LRESULT CTaskNotifyWnd::OnIgnoreBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(TASKNOTIFY_IDIGNORE);
	return 0;
}

LRESULT CTaskNotifyWnd::OnViewBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam)
{
	EndDialog(TASKNOTIFY_IDVIEW);
	return 0;
}

BOOL CTaskNotifyWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			EndDialog(0);
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTaskNotifyWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);
	m_shadow_win.follow();
}


void CTaskNotifyWnd::OnDestroy()
{
	m_shadow_win.follow();
	CDialogEx::OnDestroy();
}


BOOL CTaskNotifyWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strWndName;
	if (m_uType == TASKNOTIFY_MICROTASK)
	{
		strWndName = _T("micro_task_notify_wnd");
	}
	else if (m_uType == TASKNOTIFY_TRANS)
	{
		strWndName = _T("trans_notify_window");
	}

	if (!theApp.GetWindowManager()->Attach(GetSafeHwnd(), strWndName))
		return FALSE;

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	m_shadow_win.create(GetSafeHwnd());

	RECT rcDesktop, rcWnd;
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcDesktop, NULL);
	this->GetWindowRect(&rcWnd);
	auto sub = [](int x1, int y1, int x2, int y2) { return (x1 - y1) - (x2 - y2); };
	// 计算右下角坐标
	int x = sub(rcDesktop.right, 0, rcWnd.right, rcWnd.left);
	int y = sub(rcDesktop.bottom, 0, rcWnd.bottom, rcWnd.top);

	SetForegroundWindow();
	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, x - 25, y - 25, NULL, NULL, SWP_NOSIZE);
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CTaskNotifyWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	BringWindowToTop();
	// TODO:  在此处添加消息处理程序代码
}
