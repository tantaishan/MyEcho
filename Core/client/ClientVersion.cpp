#include "ClientVersion.h"
#include <atlconv.h>
#include <shlwapi.h>
#include "../crypt/XorCrypt.h"
#include "../../base/str/stdcxx.h"
#include "../parser/INIReader.h"
#include "../registry/registry.h"
#include "../version.h"
#include "../config.h"

ClientVersion* ClientVersion::instance = 0;
Locker ClientVersion::singletonLocker;

ClientVersion::ClientVersion(HMODULE hMod/* = NULL*/) : m_code(REG_DEFCODE)
{
	CHAR szPath[MAX_PATH] = {0};
	HMODULE hInst = (NULL == hMod) ? GetModuleHandle(NULL) : hMod;
	::GetModuleFileNameA(hInst, szPath, MAX_PATH);
	::PathRemoveFileSpecA(szPath);
	::PathAppendA(szPath, INI_FILENAME);
	m_strfile = szPath;

	// �����ļ��ж�ȡ��ǰ�汾
	//std::string decrypted;
	//XorCrypt::Decrypt(m_strfile, decrypted, INI_PASSWORD);
	//INIReader reader(decrypted.c_str());
	//m_version = base::stdcxx_s2ws(reader.Get(INI_MAINNODE, INI_VERSION, ""));

	//�汾��Ϣֱ�ӴӶ�������
	m_version = base::stdcxx_s2ws(STR_PRODUCT_VER);

	Registry registry(REG_APPROOT, REG_APPPATH, KEY_WOW64_32KEY);
	if (registry.Create()) {
		// ��ȡ������
		std::wstring code;
		code = registry.QueryValue(REG_CODE);
		if (code.empty())
		{
			// �����ļ��ж�ȡ��ʼ����
			std::string decrypted;
			XorCrypt::Decrypt(m_strfile, decrypted, INI_PASSWORD);
			INIReader reader(decrypted.c_str());
			std::wstring code = base::stdcxx_s2ws(reader.Get(INI_MAINNODE, INI_CODE, ""));
			if (!code.empty())
				m_code = _wtoi(code.c_str());
			registry.CreateKey(REG_CODE);
			registry.SetValue(REG_CODE, code);
		} 
		else
		{
			m_code = _wtoi(code.c_str());
		}
		// ��ȡ��һ�汾
		m_prever = registry.QueryValue(REG_PREVER);
	}
}

ClientVersion::~ClientVersion() {}

ClientVersion* ClientVersion::Instance(HMODULE hMod/* = NULL*/)
{
	if (!instance)
	{
		singletonLocker.Lock();
		if (!instance)
		{
			instance = new ClientVersion(hMod);
		}

		singletonLocker.Unlock();
	}

	return instance;
}

wstring ClientVersion::GetVersion()
{
	return m_version;
}

wstring ClientVersion::GetPrevVer()
{
	return m_prever;
}

int ClientVersion::GetCode()
{
	return m_code;
}
