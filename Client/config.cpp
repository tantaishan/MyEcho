#include "stdafx.h"
#include "config.h"
#include <Windows.h>
#include <tchar.h>
#include <memory>

#include <shlobj.h>
#include <shellapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

#include "../Core/json/json.h"
#include "../Core/crypt/XorCrypt.h"
#include "../Core/string/base64.h"

#define CONFIG_FILE		_T("config.dat")
#define LOGIN_DAT		_T("login.dat")
#define APP_NAME		_T("MyEcho")
#define XOR_PWD			"ME@)!%"

std::string amqp_conn_user;
std::string amqp_conn_pwd;
std::string amqp_conn_host;
int amqp_conn_port;

std::string amqp_micro_exchange;
std::string amqp_micro_key;

std::string amqp_trans_exchange;
std::string amqp_trans_key;

std::string amqp_trans_close_exchange;
std::string amqp_trans_close_key;

std::string client_config_newer_task;
std::string client_config_faq_web;

std::string login_name;
std::string login_pwd;

std::string biz_url_login;
std::string biz_url_helptask;

std::string biz_url_unfinish_task;
std::string biz_url_query_task;
std::string biz_url_feedback;
std::string biz_url_get_feedback_item;

bool config_init()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(GetModuleHandle(NULL), szPath, _countof(szPath));
	::PathRemoveFileSpec(szPath);
	::PathAppend(szPath, CONFIG_FILE);

	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwSize = ::GetFileSize(hFile, NULL);
	std::unique_ptr<CHAR> lpBuffer(new CHAR[++dwSize]);
	ZeroMemory(lpBuffer.get(), dwSize);
	DWORD dwReadSize = 0;
	::ReadFile(hFile, lpBuffer.get(), dwSize, &dwReadSize, NULL);
	::CloseHandle(hFile);
	Json::Reader reader;

	Json::Value config;
	if (!reader.parse(lpBuffer.get(), config))
		return false;

	{
		Json::Value& client_config = config["client_config"];
		client_config_newer_task = client_config["newer_task"].asString();
		client_config_faq_web = client_config["faq_web"].asString();
	}
	

	{
		Json::Value& amqp_config_conn = config["amqp_config"]["conn"];
		amqp_conn_host = amqp_config_conn["host"].asString();
		amqp_conn_port = amqp_config_conn["port"].asInt();
		amqp_conn_user = amqp_config_conn["user"].asString();
		amqp_conn_pwd = XorCrypt::Xor(base64::base64_decode(amqp_config_conn["pwd"].asString()), XOR_PWD);
	}

	{
		Json::Value& amqp_config_micro = config["amqp_config"]["micro"];
		amqp_micro_exchange = amqp_config_micro["exchange_name"].asString();
		amqp_micro_key = amqp_config_micro["key"].asString();
	}

	{
		Json::Value& amqp_config_trans = config["amqp_config"]["trans"];
		amqp_trans_exchange = amqp_config_trans["exchange_name"].asString();
		amqp_trans_key = amqp_config_trans["key"].asString();
	}

	{
		Json::Value& amqp_config_trans_close = config["amqp_config"]["trans_close"];
		amqp_trans_close_exchange = amqp_config_trans_close["exchange_name"].asString();
		amqp_trans_close_key = amqp_config_trans_close["key"].asString();
	}

	{
		Json::Value& biz_url = config["biz_url"];
		biz_url_login = biz_url["login"].asString();
		biz_url_helptask = biz_url["helptask"].asString();

		biz_url_unfinish_task = biz_url_helptask + "getUnfinishHelpTask/";
		biz_url_query_task = biz_url_helptask + "getMicroTaskInfo/";
		biz_url_feedback = biz_url_helptask + "feedbackMicroTask/";
		biz_url_get_feedback_item = biz_url_helptask + "getFeedbackItem/";

	}
	return true;
}

bool check_login()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	::PathAppend(szPath, APP_NAME);
	::PathAppend(szPath, LOGIN_DAT);

	if (!PathFileExists(szPath))
		return false;

	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwSize = ::GetFileSize(hFile, NULL);
	std::unique_ptr<CHAR> lpBuffer(new CHAR[++dwSize]);
	ZeroMemory(lpBuffer.get(), dwSize);
	DWORD dwReadSize = 0;
	::ReadFile(hFile, lpBuffer.get(), dwSize, &dwReadSize, NULL);
	::CloseHandle(hFile);
	Json::Reader reader;

	Json::Value login;
	if (!reader.parse(lpBuffer.get(), login))
		return false;

	login_name = login["user"].asString();
	login_pwd = XorCrypt::Xor(base64::base64_decode(login["pwd"].asString()), XOR_PWD);

	return true;
}

bool save_login()
{
	Json::Value login;

	login["pwd"] = base64::base64_encode((UCHAR*)XorCrypt::Xor(login_pwd, XOR_PWD).c_str(), login_pwd.length());
	login["user"] = login_name;

	TCHAR szPath[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	::PathAppend(szPath, APP_NAME);

	if (!PathFileExists(szPath))
	{
		SHCreateDirectoryEx(NULL, szPath, NULL);
	}

	::PathAppend(szPath, LOGIN_DAT);

	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE | GENERIC_READ,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwWritten = 0;
	::WriteFile(hFile, login.toStyledString().c_str(), login.toStyledString().size(), &dwWritten, NULL);
	::FlushFileBuffers(hFile);
	::CloseHandle(hFile);

	return true;
}

bool log_off()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);
	::PathAppend(szPath, APP_NAME);
	::PathAppend(szPath, LOGIN_DAT);

	if (!PathFileExists(szPath))
		return false;

	::DeleteFile(szPath);
	return true;
}