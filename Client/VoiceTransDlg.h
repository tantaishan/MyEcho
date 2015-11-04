#pragma once
#include "../base/win/shadow_window.h"
#include "TransEdit.h"
// CVoiceTransDlg 对话框
#include "PlayGifWnd.h"
#include "AudioPlayer.h"
#include "biz/TransnBizApi.h"

class CVoiceTransDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVoiceTransDlg)

public:
	CVoiceTransDlg(CWnd* pParent, const TASK_INFO2& task);   // 标准构造函数
	virtual ~CVoiceTransDlg();

// 对话框数据
	enum { IDD = IDD_DUIDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnClaimBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnSubmitBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnPlayBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnStopBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDownloadNotify(WPARAM wParam, LPARAM lParam);
private:
	base::win::ShadowWindow m_shadow_win;
	CTransEdit	m_editInput;
	TASK_INFO2	m_Task;
	AudioPlayer m_player;
	BOOL		m_bClaim;
	BOOL		m_bDownload;
	DWORD		m_dwPlayTick;
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
 