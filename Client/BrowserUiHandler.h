#pragma once
#include <mshtmhst.h>

class BrowserUiHandler :public IDocHostUIHandler
{
public:
	BOOL m_vbEnableCtxMenus;
	ULONG __m_RefCount;
	BrowserUiHandler() : m_vbEnableCtxMenus(FALSE), __m_RefCount(0) {}
	virtual ~BrowserUiHandler(){}

	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT FAR* ppt, IUnknown FAR* pcmdtReserved, IDispatch FAR* pdispReserved)
	{
		if (m_vbEnableCtxMenus == TRUE)
			return S_FALSE;
		else
			return S_OK;
	}

	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO FAR *pInfo)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject FAR* pActiveObject,
		IOleCommandTarget FAR* pCommandTarget,
		IOleInPlaceFrame  FAR* pFrame,
		IOleInPlaceUIWindow FAR* pDoc)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(HideUI)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(UpdateUI)(void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(EnableModeless)(BOOL fEnable)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnDocWindowActivate)(BOOL fActivate)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow FAR* pUIWindow, BOOL fRameWindow)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID FAR* pguidCmdGroup, DWORD nCmdID)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetOptionKeyPath)(LPOLESTR FAR* pchKey, DWORD dw)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetExternal)(IDispatch** ppDispatch)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)   {
		return E_NOTIMPL;
	}

	STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppvObj)
	{
		if (iid == IID_IUnknown || iid == IID_IDocHostUIHandler)
		{
			*ppvObj = this;
			return S_OK;
		}
		return S_FALSE;
	}

	STDMETHOD_(ULONG, AddRef)() throw()
	{
		return ++__m_RefCount;
	}

	STDMETHOD_(ULONG, Release)()
	{
		if (--__m_RefCount != 0)
			return __m_RefCount;
		delete this;
		return 0;
	}
};