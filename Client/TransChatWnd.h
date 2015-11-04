#pragma once

#include "media/WaveRecorder.h"
#include "../base/win/shadow_window.h"
// CTransChatWnd

class CTransChatWnd : public CWnd
{
	DECLARE_DYNAMIC(CTransChatWnd)

public:
	CTransChatWnd();
	virtual ~CTransChatWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL InitChatAvatar();
	BOOL StartRecord();
	BOOL StopRecord();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

private:
	void InitCharFormat();
	void FormatInputEdit();

private:
	base::win::ShadowWindow m_shadow_win;
	duListCtrl* m_pChatList;
	WaveRecorder m_Recorder;
	CRichEditCtrl m_editView;
	CRichEditCtrl m_editInput;
	CHARFORMAT m_cfNow;
	CHARFORMAT m_cfViewTime;
};


