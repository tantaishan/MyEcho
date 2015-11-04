#pragma once
#include "include/cef_v8.h"
#include "typedef.h"
#include "../base/str/stdcxx.h"
#include "../Core/file/FileManager.h"
#include "TransChatWebWnd.h"
#include "FolderFileSelectDialog.h"
#include "AudioPlayer.h"

class CTransChatWebWnd;
class ClientV8ExtHandler : public CefV8Handler
{
public:
	ClientV8ExtHandler() :  audioplayer_(NULL),
		test_param_("An initial string value.") {}

	virtual ~ClientV8ExtHandler() {}

	// Execute with the specified argument list and return value.  Return true if
	// the method was handled.
	//�÷����̳�CefV8Handler����ʵ��
	//����:�ж�js���õı��ط������ƣ����ղ������ݺͷ���ֵ��ʵ�ַ������幦��
	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);

public:
	CefString			test_param_;
	AudioPlayer*		audioplayer_;
	IMPLEMENT_REFCOUNTING(ClientV8ExtHandler);
};