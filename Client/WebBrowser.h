#pragma once

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CHtmlView��"
#endif 

// CMyWebBrowser Html ��ͼ
#include "BrowserUiHandler.h"

class CWebBrowser : public CHtmlView
{
	DECLARE_DYNCREATE(CWebBrowser)

protected:
	CWebBrowser();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWebBrowser();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);
	UINT_PTR GetTag() { return m_pTag; }
	void SetTag(UINT_PTR pTag) { m_pTag = pTag; }

	virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

private:
	UINT_PTR	m_pTag;
/*	MyUiHandler m_Ui;*/
	DECLARE_MESSAGE_MAP()
};


