#pragma once
#include "../base/win/shadow_window.h"
#include "include/cef_app.h"
#include "CefWebClient.h"
#include "media/WaveRecorder.h"
#include "Resource.h"
// CTransChatWebWnd

class CTransChatWebWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CTransChatWebWnd)

public:
	CTransChatWebWnd(CWnd* pParent);
	virtual ~CTransChatWebWnd();

	enum { IDD = IDD_DUIDIALOG };

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	void InitExtension();
	void ExecuteJavascript(const wstring& strCode);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecordStart(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecordStop(WPARAM wParam, LPARAM lParam);
	LRESULT OnRecordSave(WPARAM wParam, LPARAM lParam);
	LRESULT OnTransEnd(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
private:
	base::win::ShadowWindow m_shadow_win;
	CefRefPtr<CefWebClient> m_cWebClient;
	WaveRecorder m_Recorder;
	int			m_nSecCount;
	BOOL		m_bEndChat;

	friend class ClientV8ExtHandler;

public:
	//CMyWebBrowser* pWebBrowser;
	afx_msg void OnNcDestroy();
	//ClientV8ExtHandler* m_pV8Exthandler;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


