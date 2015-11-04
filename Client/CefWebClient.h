#pragma once
#include "include/cef_client.h"

class CefWebClient : public CefClient, public CefLifeSpanHandler
{
protected:
	CefRefPtr<CefBrowser> m_Browser;
	
public:
	CefWebClient() : v8handler_(NULL) {};
	CefWebClient(std::string js_ext, CefV8Handler* v8handler) : 
		js_ext_(js_ext), v8handler_(v8handler) { };
	virtual  ~CefWebClient() {};
	CefRefPtr<CefBrowser> GetBrowser()
	{
		return m_Browser;
	};
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{ return this; };
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) {
		m_Browser = browser;
// 		if (!js_ext_.empty() && v8handler_)
// 			CefRegisterExtension("v8/Ext", js_ext_, v8handler_);
	};
	
	IMPLEMENT_REFCOUNTING(CefWebClient);
	IMPLEMENT_LOCKING(CefWebClient);

private:
	CefV8Handler*	v8handler_;
	std::string		js_ext_;
};

