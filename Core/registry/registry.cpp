#include "registry.h"

Registry::Registry(UINT flag/* = 0*/) : flag_(flag) {}

Registry::Registry(HKEY root, const tstring& path, UINT flag/* = 0*/)
	: hkey_(NULL), root_(root), path_(path), flag_(flag) {}

bool Registry::Create()
{
	DWORD dwState = 0;
	LONG lRet = ::RegCreateKeyEx(root_, path_.c_str(), 0, NULL, 0, KEY_ALL_ACCESS|flag_, NULL, &hkey_, &dwState);
	if(lRet == ERROR_SUCCESS) {
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

bool Registry::Create(HKEY root, const tstring& path)
{
	root_ = root;
	path_ = path;
	return Create();
}

bool Registry::CreateKey(const tstring& subKey)
{
	DWORD dwState = 0;
	LONG lRet = ::RegCreateKeyEx(root_, path_.c_str(), 0, NULL, 0, KEY_ALL_ACCESS|flag_, NULL, &hkey_, &dwState);
	if(lRet == ERROR_SUCCESS) {
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

tstring Registry::QueryValue(const tstring& key)
{
	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_READ|flag_, &hkey_);
	TCHAR tszVal[MAX_PATH] = { 0 };
	if (lRet == ERROR_SUCCESS) {
		DWORD dwType, dwSize;
		dwSize = sizeof (tszVal);
		::RegQueryValueEx(hkey_, key.c_str(), 0, &dwType, (LPBYTE)tszVal, &dwSize);
		::RegCloseKey(hkey_);
	}
	return tszVal;
}

DWORD Registry::QuerydwordValue(const tstring& key)
{
	DWORD dwValue = 0, dwType = 0, dwSize = sizeof(DWORD);
	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_READ|flag_, &hkey_);
	if (lRet == ERROR_SUCCESS) {
		::RegQueryValueEx(hkey_, key.c_str(), NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &dwSize);
		::RegCloseKey(hkey_);
	}
	return dwValue;
}

bool Registry::QueryValue(const tstring& key, LPVOID pData, DWORD* cbSize)
{
	if (!pData || !cbSize) return false;

	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_READ|flag_, &hkey_);
	if (lRet == ERROR_SUCCESS) {
		DWORD dwType;
		lRet = ::RegQueryValueEx(hkey_, key.c_str(), 0, &dwType, (LPBYTE)pData, cbSize);
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

bool Registry::SetValue(const tstring& key, int value)
{
	TCHAR szValue[MAX_PATH] = { 0 };
	_itot_s(value, szValue, 10);
	return SetValue(key, szValue);
}

bool Registry::SetValue(const tstring& key, const tstring& value)
{
	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_WRITE|flag_, &hkey_);
	if (lRet == ERROR_SUCCESS) {
		lRet = ::RegSetValueEx(hkey_, key.c_str(), 0, REG_SZ, (LPBYTE)value.c_str(), sizeof(TCHAR) * value.size());
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

bool Registry::SetdwordValue(const tstring& key, DWORD value)
{
	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_WRITE|flag_, &hkey_);
	if (lRet == ERROR_SUCCESS) {
		lRet = ::RegSetValueEx(hkey_, key.c_str(), 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

bool Registry::SetValue(const tstring& key, LPVOID pData, int cbSize)
{
	if (!pData || !cbSize) return false;

	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_WRITE|flag_, &hkey_);
	if (lRet == ERROR_SUCCESS) {
		lRet = ::RegSetValueEx(hkey_, key.c_str(), 0, REG_BINARY, (BYTE*)pData, cbSize);
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

bool Registry::DeleteKey(const tstring& key)
{
	return (::RegDeleteKey(root_, key.c_str()) == ERROR_SUCCESS);
}

bool Registry::DeleteValue(const tstring& key)
{
	LONG lRet = ::RegOpenKeyEx(root_, path_.c_str(), 0, KEY_WRITE|flag_, &hkey_);
	if (lRet == ERROR_SUCCESS) {
		lRet = ::RegDeleteValue(hkey_, key.c_str());
		::RegCloseKey(hkey_);
	}
	return (lRet == ERROR_SUCCESS);
}

bool Registry::Delete()
{
	return (::RegDeleteKey(root_, path_.c_str()) == ERROR_SUCCESS);
}

