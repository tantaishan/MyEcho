#pragma once
#include <iostream>
#include <Windows.h>

class ResourceMgr
{
public:
	static BOOL ReleaseFile(int nId, LPCTSTR lpszType, LPCTSTR lpszPath);
};

