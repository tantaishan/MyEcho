
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "TopMenuWnd.h"
#include "../base/win/shadow_window.h"
#include "SystemTray.h"
#include "../amqcpp/AMQPcpp.h"
#include "../base/lock/lock.h"
#include "biz/TransnBizApi.h"
#include "CefWebClient.h"
#include <bitset>
#include "../Core/thread/Locker.h"
#include "typedef.h"
#include "WebBrowser.h"

extern SESSION_DATA g_SessionData;

class CMainFrame : public CFrameWnd
{

public:
	CMainFrame();
	~CMainFrame();

protected:
	DECLARE_DYNAMIC(CMainFrame)
	afx_msg void OnFileClose();
	DECLARE_MESSAGE_MAP()

public:
	BOOL FilterTask(const AMQP_TASK& task);
	LRESULT OnChangeLanguageBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnCheckUpdate(WPARAM wParam, LPARAM lParam);
	LRESULT OnClean(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnDownloadNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnDuiNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnExitApp(WPARAM wParam, LPARAM lParam);
	LRESULT OnInstantTransCbxClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnLanguageCbxClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnLogout(WPARAM wParam, LPARAM lParam);
	LRESULT OnMinBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnSettingCancelBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnSettingSaveBtnClick(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowMainWnd(WPARAM wParam, LPARAM lParam);
	LRESULT OnSystemTray(WPARAM wParam, LPARAM lParam);
	LRESULT OnCloseTrans(WPARAM wParam, LPARAM lParam);
	LRESULT OnTopMenuButton(duPlugin* pPlugin, WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	int OnMqMessage_Micro(AMQPMessage* message);
	int OnMqMessage_Trans(AMQPMessage* message);
	int OnMqMessage_TransEnd(AMQPMessage* message);

	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void DisplayTask_Micro(const TASK_INFO2& task);
	void DisplayTask_Trans(const AMQP_TASK_TRANS& task);
	void InitInstantCbx();
	void InitLangRights();
	void InitLanguageCbx();
	void OnAppExit();
	void UpdateLangSkill();

private:
	void InitMqThreads();

	static DWORD WINAPI S_ChkUnfinishTask(void* lpVoid);
	static DWORD WINAPI S_MqThread_Micro(void* lpVoid);
	static DWORD WINAPI S_MqThread_Trans(void* lpVoid);
	static DWORD WINAPI S_MqThread_TransEnd(void* lpVoid);

	void CreateChkUnfinishThread();
	void CreateMqThread_Mirco();
	void CreateMqThread_Trans();
	void CreateMqThread_TransEnd();

	void ShowSettingPanel(BOOL bShow);

	void StopChkUnfinishThread();
	void StopMqThread_Micro();
	void StopMqThread_Trans();
	void StopMqThread_TransEnd();

private:
	CDWordArray					m_vtLangSkills;
	//CefRefPtr<CefWebClient>		m_faqWebBrowser;
	SystemTray					m_Tray;
	base::ThreadParam			m_tpChkUnfinish;
	base::ThreadParam			m_tpMqMicro;
	base::ThreadParam			m_tpMqTrans;
	base::ThreadParam			m_tpMqTransEnd;
	base::win::ShadowWindow		m_shadow_win;
	bitset<32>					m_bsLangRights;
	static Locker				m_locker;
	std::unique_ptr<AMQP>		m_ptrAMQP;
	std::unique_ptr<AMQP>		m_ptrAMQP2;
	std::unique_ptr<AMQP>		m_ptrAMQP3;
	std::unique_ptr<AMQPQueue>	m_ptrQueMicro;
	std::unique_ptr<AMQPQueue>	m_ptrQueTrans;
	std::unique_ptr<AMQPQueue>	m_ptrQueTransEnd;
	std::vector<duCheckBox*>	m_vtLangCbx;
	volatile int				m_bSendSwitch;
	CWebBrowser*					m_pWebBrowser;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();
};