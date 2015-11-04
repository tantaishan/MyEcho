#pragma once
#include "../registry/registry.h"
#include "../thread/Locker.h"

class UidManager
{
public:
	static UidManager* Instance();
	UidManager();
	std::wstring GetMid(void);
	std::wstring GetPreMid(void);
	std::wstring GetAut(void);
	int GetMcn(void);

	std::wstring GetDiskFirmware();
	std::wstring GetDiskMode();
	std::wstring GetDiskSerial();
	int GetDiskRotation();

private:
	class Garbo
	{
	public:
		~Garbo()
		{
			if (UidManager::instance)
				delete UidManager::instance;
		}
	};

	static Garbo garbo;
	static UidManager* instance;
	static Locker locker;

private:
	std::wstring BuildMid(void);
	std::wstring BuildAut(const std::wstring& mid);

	Registry m_reg;
};