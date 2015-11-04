// MyWebBrowser.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "WebBrowser.h"
#include "BrowserUiHandler.h"

// CMyWebBrowser

IMPLEMENT_DYNCREATE(CWebBrowser, CHtmlView)

CWebBrowser::CWebBrowser() : m_pTag(NULL)
{
//	MyUiHandler* pUi = new MyUiHandler;
//	pUi->QueryInterface(IID_IDocHostUIHandler, (void**)&m_Ui);
}

CWebBrowser::~CWebBrowser()
{
	//m_Ui->Release();
}

void CWebBrowser::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWebBrowser, CHtmlView)
END_MESSAGE_MAP()


// CMyWebBrowser 诊断

#ifdef _DEBUG
void CWebBrowser::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CWebBrowser::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

HRESULT CWebBrowser::OnGetHostInfo(DOCHOSTUIINFO *pInfo)
{
	// 去边框
	if (pInfo != NULL)
	{
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO;
	}
	return S_OK;
}

void CWebBrowser::NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL)
{
	// 去边框
	if (!GetTag())
	{
		Refresh2(0);
		SetTag(TRUE);
	}

// 	IDispatch* pDoc = GetHtmlDocument();
// 	if (pDoc == NULL)
// 	{
// 		pDoc->Release();
// 		return;
// 	}
// 
// 	ICustomDoc* pDoc2 = NULL;
// 	HRESULT hresult = pDoc->QueryInterface(IID_ICustomDoc, (void**)&pDoc2);
// 	if (FAILED(hresult))
// 	{
// 		pDoc->Release();
// 		return;
// 	}
// 
// 	pDoc->Release();
// 
// 	pDoc2->SetUIHandler(&m_Ui);
// 	pDoc2->Release();
}


// CMyWebBrowser 消息处理程序
