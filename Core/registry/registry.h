#pragma once
#include "../string/tstring.h"

class Registry
{
public:
	Registry(UINT flag = 0);
	Registry(HKEY root, const tstring& path, UINT flag = 0);
	bool Create();
	bool Create(HKEY root, const tstring& path);
	bool CreateKey(const tstring& subKey);
	tstring QueryValue(const tstring& key);
	DWORD QuerydwordValue(const tstring& key);
	bool QueryValue(const tstring& key, LPVOID pData, DWORD* cbSize);
	bool SetValue(const tstring& key, int value);
	bool SetValue(const tstring& key, const tstring& value);
	bool SetValue(const tstring& key, LPVOID pData, int cbSize);
	bool SetdwordValue(const tstring& key, DWORD value);
	bool DeleteKey(const tstring& key);
	bool DeleteValue(const tstring& key);
	bool Delete();

private:
	HKEY			root_;
	tstring			path_;
	HKEY			hkey_;
	UINT			flag_;
};
