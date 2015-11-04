#include "StdAfx.h"
#include "SystemTray.h"

SystemTray::SystemTray() {}

SystemTray::~SystemTray(void)
{
	Close();
}

void SystemTray::Create(HWND hWnd, UINT uID, UINT uMsg, LPTSTR lpTip)
{
	ZeroMemory(&m_nid, sizeof(NOTIFYICONDATA));
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = hWnd;
	m_nid.uID = uID;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = uMsg;
	m_nid.hIcon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(uID));
	_tcscpy_s(m_nid.szTip, lpTip);
}


BOOL SystemTray::Show()
{
	return ::Shell_NotifyIcon(NIM_ADD, &m_nid);
}

BOOL SystemTray::Close()
{
	return ::Shell_NotifyIcon(NIM_DELETE, &m_nid);
}
