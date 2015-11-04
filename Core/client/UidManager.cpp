#include "UidManager.h"
#include <atlconv.h>
#include "ClientVersion.h"
#include "../base/str/stdcxx.h"
#include "../hash/MyMD5.h"
#include "../wmi/WmiQueryHelper.h"
#include "../system/SystemInfo.h"
#include "../crypt/XorCrypt.h"
#include "../config.h"

//////////////////////////////////////////////////////////////////////////
// BKDR Hash Function
long BKDRHash(std::wstring str)
{
	long seed = 131; // 31 131 1313 13131 131313 etc..
	long hash = 0;
	for(unsigned int i = 0; i < str.length(); i++)
	{
		hash = (hash * seed) + str.at(i);
	}

	return hash;
}

// Peter J. Weinberger hash function
long PJWHash(std::wstring str)
{
	long BitsInUnsignedInt = (long)(4 * 8);
	long ThreeQuarters     = (long)((BitsInUnsignedInt  * 3) / 4);
	long OneEighth         = (long)(BitsInUnsignedInt / 8);
	long HighBits          = (long)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	long hash              = 0;
	long test              = 0;
	for(unsigned int i = 0; i < str.length(); i++)
	{
		hash = (hash << OneEighth) + str.at(i);
		if((test = hash & HighBits)  != 0)
		{
			hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return hash;
}

// Create guid wstring
std::wstring CreateGuid()
{
	GUID guid = { 0 };
	TCHAR szGuid[MAX_PATH] = { 0 };
	if (S_OK == ::CoCreateGuid(&guid)) {
		_stprintf_s(szGuid, _T("{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	}
	return szGuid;
}

//////////////////////////////////////////////////////////////////////////

UidManager* UidManager::instance = 0;
Locker UidManager::locker;

UidManager::UidManager() : m_reg(KEY_WOW64_32KEY)
{
	m_reg.Create(REG_APPROOT, REG_APPPATH);
}

UidManager* UidManager::Instance()
{
	if (!instance)
	{
		locker.Lock();
		if (!instance)
		{
			instance = new UidManager;
		}

		locker.Unlock();
	}

	return instance;
}

// 获取机器码
std::wstring UidManager::GetMid(void)
{
	std::wstring newMid = BuildMid();
	std::wstring oldMid = m_reg.QueryValue(REG_MID);
	if (newMid == oldMid) {
		return oldMid;
	} else {
		std::wstring strMcn = m_reg.QueryValue(REG_MCN);
		if (!strMcn.empty()) {
			int nMcn = _wtoi(strMcn.c_str());
			if (nMcn >= REG_MAXMCN) return oldMid;
			m_reg.SetValue(REG_MCN, ++nMcn);
		} else {
			m_reg.SetValue(REG_MCN, L"1");
		}
		m_reg.SetValue(REG_PREMID, oldMid);
		m_reg.SetValue(REG_MID, newMid);
		return newMid;
	}
}

// 获取上一个Mid
std::wstring UidManager::GetPreMid(void)
{
	return m_reg.QueryValue(REG_PREMID);
}

// 获取验证码
std::wstring UidManager::GetAut(void)
{
	return  m_reg.QueryValue(REG_AUT);
}

// 获取机器码变更计数
int UidManager::GetMcn(void)
{
	std::wstring strMcn = m_reg.QueryValue(REG_MCN);
	if (!strMcn.empty()) {
		return _wtoi(strMcn.c_str());
	} else {
		m_reg.SetValue(REG_MCN, L"0");
		return 0;
	}
}

// 生成机器码
std::wstring UidManager::BuildMid(void)
{
	long uid = 0;
	std::wstring mac_addr = SystemInfo::GetMACAddress();
	if (!mac_addr.empty())
		uid = PJWHash(mac_addr);

	if (0 == uid) {
		WmiQueryHelper querier;
		std::vector<std::wstring> vtMac;
		std::wstring pcSqlW = L"SELECT * FROM Win32_NetworkAdapter WHERE ((MACAddress Is Not NULL) AND (Manufacturer <> 'Microsoft'))";
		if (querier.ExecQuery(pcSqlW.c_str(), L"MACAddress", vtMac) && vtMac.size()) {
			uid = PJWHash(vtMac[0]);
		} else {
			uid = PJWHash(CreateGuid());
		}
	}

	TCHAR buff[MAX_PATH] = {0};
	if (0 != uid)
		_ltot_s(uid, buff, 10);
	BuildAut(buff);

	return buff;
}

// 生成验证码
std::wstring UidManager::BuildAut(const std::wstring& mid)
{
	TCHAR szAut[MAX_PATH] = {0};
	_stprintf_s(szAut, MAX_PATH, _T("%s%s"), mid.c_str(), REG_AUTPWD);
	std::wstring aut = MyMD5::MD5(szAut);
	m_reg.SetValue(REG_AUT, aut);
	return aut;
}

std::wstring UidManager::GetDiskFirmware()
{
	std::wstring ret;

	CHAR sz[MAX_PATH] = {0};
	DWORD dw = sizeof(sz);
	if (m_reg.QueryValue(REG_DISKFIRMWARE, sz, &dw))
	{
		std::string str(sz, dw);
		str = XorCrypt::Xor(str, REG_PASSWORD);
		ret = base::stdcxx_s2ws(str);
	}

	return ret;
}

std::wstring UidManager::GetDiskMode()
{
	std::wstring ret;

	CHAR sz[MAX_PATH] = {0};
	DWORD dw = sizeof(sz);
	if (m_reg.QueryValue(REG_DISKMODE, sz, &dw))
	{
		std::string str(sz, dw);
		str = XorCrypt::Xor(str, REG_PASSWORD);
		ret = base::stdcxx_s2ws(str);
	}

	return ret;
}

std::wstring UidManager::GetDiskSerial()
{
	std::wstring ret;

	CHAR sz[MAX_PATH] = {0};
	DWORD dw = sizeof(sz);
	if (m_reg.QueryValue(REG_DISKSERIAL, sz, &dw))
	{
		std::string str(sz, dw);
		str = XorCrypt::Xor(str, REG_PASSWORD);
		ret = base::stdcxx_s2ws(str);
	}

	return ret;
}

int UidManager::GetDiskRotation()
{
	int ret = m_reg.QuerydwordValue(REG_DISKROTATION);
	return ret;
}
