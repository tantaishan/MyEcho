#pragma once

#include "../thread/Locker.h"
#include <iostream>

class ClientVersion
{
public:
	static ClientVersion* Instance(HMODULE hMod = NULL);
public:
	std::wstring GetVersion();
	std::wstring GetPrevVer();
	int GetCode();

private:
	ClientVersion(HMODULE hMod = NULL);
	~ClientVersion();
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (ClientVersion::instance)
				delete ClientVersion::instance;
		}
	};

	static CGarbo Garbo;
	static ClientVersion* instance;
	static Locker singletonLocker;

private:
	std::wstring	m_version;
	std::wstring	m_prever;
	int				m_code;
	std::string		m_strfile;
};