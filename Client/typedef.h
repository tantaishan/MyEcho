#pragma once

#define MAIN_MUTEX_NAME			_T("_MyEcho_Main_Process_Mutex_")
#define AUTO_CLOSE_WND_TIME		10

enum
{
	WM_SYSTRAY = WM_USER + 0x0001,
	WM_RESTORE_STYLE,
	WM_DOWNLOAD_NOTIFY,
	WM_SHOWMAINWND,
	WM_CLEAN,
	WM_CHECK_UPDATE,
	WM_LOGOUT,
	WM_EXIT_APP,
	WM_IMG_ROTATE,
	WM_RECORD_START,
	WM_RECORD_STOP,
	WM_RECORD_SAVE,
	WM_SHOWCHATWND,
	WM_CLOSETRANS,
	WM_TRANSEND,
	WM_ZOOMIN,
	WM_ZOOMOUT,
};

enum
{
	TIMER_SHOW_PROMPT = 0x100,
	TIMER_VOICE_PLAYEND,
	TIMER_AUTO_CLOSE,
};

typedef struct _SESSION_DATA
{
	std::string		conversation_id;
	std::string		customer_id;
	std::string		first_sentence;
	HWND			chat_hwnd;
	TCHAR			voice_path[MAX_PATH];
	bool			has_close_wnd;
} SESSION_DATA;
