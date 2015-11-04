#include "stdafx.h"
#include "ClientV8ExtHandler.h"
#include "biz/TransnBizApi.h"
#include "../Core/client/ClientAPI.h"
#include "FileMgr.h"
#include "ViewImageWnd.h"
#include "biz/EasemobBizApi.h"

extern USER_DATA2::DATA g_LoginData;
extern SESSION_DATA g_SessionData;

bool ClientV8ExtHandler::Execute(const CefString& name,
	CefRefPtr<CefV8Value> object,
	const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval,
	CefString& exception)
{
	if (name == "SetTestParam")
	{
		// Handle the SetTestParam native function by saving the string argument
		// into the local member.
		if (arguments.size() != 1 || !arguments[0]->IsString())
			return false;

		test_param_ = arguments[0]->GetStringValue();
		return true;
	}
	else if (name == "GetTestParam")
	{
		// Handle the GetTestParam native function by returning the local member
		// value.
		retval = CefV8Value::CreateString(test_param_);
		return true;
	}
	else if (name == "GetTestObject")
	{
		// Handle the GetTestObject native function by creating and returning a
		// new V8 object.
		retval = CefV8Value::CreateObject(NULL);
		// Add a string parameter to the new V8 object.
		retval->SetValue("param", CefV8Value::CreateString(
			"Retrieving a parameter on a native object succeeded."),
			V8_PROPERTY_ATTRIBUTE_NONE);
		// Add a function to the new V8 object.
		retval->SetValue("GetMessage",
			CefV8Value::CreateFunction("GetMessage", this),
			V8_PROPERTY_ATTRIBUTE_NONE);
		return true;
	}
	else if (name == "GetMessage")
	{
		// Handle the GetMessage object function by returning a string.
		retval = CefV8Value::CreateString(
			"Calling a function on a native object succeeded.");
		return true;
	}
	else if (name == "StartRecord")
	{
		::PostMessage(g_SessionData.chat_hwnd, WM_RECORD_START, 0, 0);
		return true;
	}
	else if (name == "SendRecord")
	{
		::SendMessage(g_SessionData.chat_hwnd, WM_RECORD_STOP, 0, 0);
		::SendMessage(g_SessionData.chat_hwnd, WM_RECORD_SAVE, 0, 0);

		Easemob::TOKEN token;
		if (Easemob::GetToken(token) == 0)
		{
			Easemob::UPLOAD_FILE upload;
			upload.authorization = token.access_token;
			upload.restrict_access = true;
			upload.file_path = base::stdcxx_ws2s(g_SessionData.voice_path);

			Easemob::UPLOAD_RESULT upload_result;
			if (Easemob::UploadFile(upload, upload_result) == 0)
			{
				Easemob::AUDIO_MESSAGE audio_msg;
				audio_msg.from = g_LoginData.userId;
				audio_msg.to.push_back(g_SessionData.customer_id);
				audio_msg.bodies.url = upload_result.entities[0].uuid;

				TCHAR szFileName[MAX_PATH];
				StrCpy(szFileName, g_SessionData.voice_path);
				LPTSTR lpFileName = PathFindFileName(szFileName);
				audio_msg.bodies.filename = base::stdcxx_ws2s(lpFileName);

				audio_msg.bodies.secret = upload_result.entities[0].share_secret;

				AudioPlayer player;
				player.Open(g_SessionData.voice_path);
				DWORD dwLength = player.Length(g_SessionData.voice_path);
				audio_msg.bodies.length = dwLength / 1000;
				Easemob::SendAudioMsg(token, audio_msg);

			}
		}
		return true;
	}

	else if (name == "CancelRecord")
	{
		::PostMessage(g_SessionData.chat_hwnd, WM_RECORD_STOP, 0, 0);
		return true;
	}
	else if (name == "GetFileSize")
	{
		CefString strFile = arguments[0]->GetStringValue();
		DWORD dwSize = FileManager::GetFileSize(strFile.c_str());
		retval = CefV8Value::CreateInt(dwSize);
		return true;
	}
	else if (name == "SaveRecord")
	{
		::SendMessage(g_SessionData.chat_hwnd, WM_RECORD_SAVE, 0, 0);
		return true;
	}
	else if (name == "SelectFileDlg")
	{
		const int kMaxFileBuffer = MAX_PATH * 256;
		wchar_t wstrErr[] = L"你不能选择这个路径。\n请尝试选择其它路径。";
		CFolderFileSelectDialog dlg(TRUE, _T("*.*"), NULL, NULL, _T("All files and folders(*.*)|*.*||"));
		//重新设置缓冲区大小，防止文件路径被截断
		dlg.m_ofn.nMaxFile = kMaxFileBuffer;
		dlg.m_ofn.lpstrFile = new TCHAR[kMaxFileBuffer]; //重新定义缓冲区大小         
		memset(dlg.m_ofn.lpstrFile, 0, kMaxFileBuffer*sizeof(TCHAR));  //初始化定义的缓冲区
		dlg.SetSpecialStrings(_T("文件(夹):"), _T("选择"));
		dlg.SetErrorInfo(wstrErr);
		if (dlg.DoModal() == IDOK)
		{
			if (dlg.HaveSelectSomething())
			{
				//	if (_tcscmp(szCurr, _T("\\")))
				//		sTarget.Append(szCurr);
				retval = CefV8Value::CreateString(base::stdcxx_ws2s(dlg.m_SelectedItemList[0].GetBuffer()));
			}
			else
			{
				::MessageBox(g_SessionData.chat_hwnd, _T("不能选择虚拟目录"), NULL, MB_OK);
			}
		}
		else if (dlg.IsVirtualItemSelected())
		{
			::MessageBox(g_SessionData.chat_hwnd, _T("不能选择虚拟目录"), NULL, MB_OK);
		}

		delete[] dlg.m_ofn.lpstrFile;
		return true;
	}

	else if (name == "GetVoiceData")
	{

		HANDLE hFile = ::CreateFile(g_SessionData.voice_path, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return false;
		unsigned long fileSize = ::GetFileSize(hFile, NULL);
		char* buff = new char[fileSize + 1];
		memset(buff, 0, fileSize + 1);
		unsigned long readSize = 0;
		::ReadFile(hFile, buff, fileSize, &readSize, NULL);

		std::string __str;

		__str.resize(fileSize);
		memcpy(const_cast<char*>(__str.c_str()), buff, fileSize);

		::CloseHandle(hFile);
		delete buff;

		retval = CefV8Value::CreateString(__str);

		return true;
		return true;
	}
	else if (name == "GetVoicePath")
	{
		retval = CefV8Value::CreateString(base::stdcxx_ws2utf8(g_SessionData.voice_path));
		return true;
	}
	else if (name == "GetVoiceSize")
	{
		//::PostMessage(hWnd_, WM_RECORD_SEND, 0, 0);
		//if (arguments[0]->IsInt() && arguments[1]->IsString())
		//{
		retval = CefV8Value::CreateInt(FileManager::GetFileSize(g_SessionData.voice_path));
		//}
		return true;
	}
	else if (name == "GetVoiceLength")
	{
		CefString strPath = arguments[0]->GetStringValue();
		AudioPlayer player;
		player.Open(strPath.ToWString().c_str());
		DWORD dwLength = player.Length(strPath.ToWString().c_str());
		retval = CefV8Value::CreateInt(dwLength / 1000);
		return true;
	}
	else if (name == "ViewImage")
	{
		CefString strUrl = arguments[0]->GetStringValue();
		LPTSTR wszUrl = new TCHAR[MAX_PATH];
		ZeroMemory(wszUrl, MAX_PATH);
		StrCpy(wszUrl, strUrl.ToWString().c_str());

		PathUnquoteSpaces(wszUrl);

		if (::PathIsURL(wszUrl))
		{
			// 			PARSEDURL pu;
			// 			ZeroMemory(&pu, sizeof(pu));
			// 			ParseURL(wszUrl, &pu);

			std::wstring wsFile = wszUrl;
			wsFile = wsFile.substr(wsFile.find_last_of(L'/') + 1, wsFile.length());
			wsFile += L".jpg";

			TCHAR szPath[MAX_PATH] = { 0 };
			_stprintf_s(szPath, MAX_PATH, _T("%s\\%s"),
				FileMgr::GetAppPathById(conv::s2wcs(g_LoginData.userId), _T("recv")).GetBuffer(),
				wsFile.c_str());

			if (PathFileExists(szPath))
			{
				CViewImageWnd* pWnd = new CViewImageWnd(szPath);
				pWnd->CreateEx(0, _T("ViewImageWnd"), NULL, WS_OVERLAPPED | WS_VISIBLE, CRect(0, 0, 0, 0), NULL, 0, 0);
				return 0;
			}
			else
			{
				HRESULT hr = URLDownloadToFile(NULL, wszUrl, szPath, 0, NULL);
				if (SUCCEEDED(hr))
				{
					CViewImageWnd* pWnd = new CViewImageWnd(szPath);
					pWnd->CreateEx(0, _T("ViewImageWnd"), NULL, WS_OVERLAPPED | WS_VISIBLE, CRect(0, 0, 0, 0), NULL, 0, 0);
					return 0;
				}

			}
		}
		else
		{
			CViewImageWnd* pWnd = new CViewImageWnd(wszUrl);
			pWnd->CreateEx(0, _T("ViewImageWnd"), NULL, WS_OVERLAPPED | WS_VISIBLE, CRect(0, 0, 0, 0), NULL, 0, 0);
			return 0;
		}

		delete wszUrl;
		return true;

	}
	else if (name == "PlayAudio")
	{
		//CefRefPtr<CefV8Value> objUrl = arguments[0];
		CefString strUrl = arguments[0]->GetStringValue();
		CefString strFile = arguments[1]->GetStringValue();

		LPTSTR wszUrl = new TCHAR[MAX_PATH];
		ZeroMemory(wszUrl, MAX_PATH);
		StrCpy(wszUrl, strUrl.ToWString().c_str());
		wszUrl++;

		PathUnquoteSpaces(wszUrl);

		if (::PathIsURL(wszUrl))
		{
// 			PARSEDURL pu;
// 			ZeroMemory(&pu, sizeof(pu));
// 			ParseURL(wszUrl, &pu);

			std::wstring wsFile = wszUrl;
			wsFile = wsFile.substr(wsFile.find_last_of(L'/') + 1, wsFile.length());
			wsFile += L".amr";

			TCHAR szPath[MAX_PATH] = { 0 };
			_stprintf_s(szPath, MAX_PATH, _T("%s\\%s"),
				FileMgr::GetAppPathById(conv::s2wcs(g_LoginData.userId), _T("recv")).GetBuffer(),
				wsFile.c_str());

			if (PathFileExists(szPath))
			{
				if (audioplayer_)
				{
					audioplayer_->Stop();
					delete audioplayer_;
					audioplayer_ = NULL;
				}

				audioplayer_ = new AudioPlayer;
				audioplayer_->Open(szPath);
				audioplayer_->Play();
			}
			else
			{
				HRESULT hr = URLDownloadToFile(NULL, wszUrl, szPath, 0, NULL);
				if (SUCCEEDED(hr))
				{
					if (audioplayer_)
					{
						audioplayer_->Stop();
						delete audioplayer_;
						audioplayer_ = NULL;
					}

					audioplayer_ = new AudioPlayer;
					audioplayer_->Open(szPath);
					audioplayer_->Play();
				}
				
			}
		}
		else
		{
			if (audioplayer_)
			{
				audioplayer_->Stop();
				delete audioplayer_;
				audioplayer_ = NULL;
			}

			audioplayer_ = new AudioPlayer;
			audioplayer_->Open(wszUrl);
			audioplayer_->Play();
		}

		wszUrl--;
		delete wszUrl;
		return true;
	}
	else if (name == "StopAudio")
	{
		if (audioplayer_)
		{
			audioplayer_->Stop();
			delete audioplayer_;
			audioplayer_ = NULL;
		}
		return true;
	}
	else if (name == "GetUserId")
	{
		retval = CefV8Value::CreateString(g_LoginData.userId);
		return true;
	}
	else if (name == "GetUserPwd")
	{
		retval = CefV8Value::CreateString("123456");
		return true;
	}
	else if (name == "GetCustomerId")
	{
		retval = CefV8Value::CreateString(g_SessionData.customer_id);
		return true;
	}
	else if (name == "GetCustomerPwd")
	{
		retval = CefV8Value::CreateString("123456");
		return true;
	}
	else if (name == "GetFirstSentence")
	{
		retval = CefV8Value::CreateString(base::stdcxx_s2utf8(g_SessionData.first_sentence));
		return true;
	}
	return false;
}