// RsrcToCpp.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1) return 0;

	HANDLE hRsrc = CreateFile(argv[1], GENERIC_WRITE | GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hRsrc == INVALID_HANDLE_VALUE) return 0;

	TCHAR drive[_MAX_DRIVE] = {0};
	TCHAR dir[_MAX_DIR] = {0};
	TCHAR fname[_MAX_FNAME] = {0};
	TCHAR ext[_MAX_EXT] = {0};

	_tsplitpath_s(argv[1], drive, dir, fname, ext);
	TCHAR output[_MAX_DIR] = {0};
	if (argc == 2)
		_stprintf_s(output, _T("%s%s%s%s"), drive, dir, fname, _T(".h"));
	else
		_stprintf_s(output, _T("%s%s%s"), argv[2], fname, _T(".h"));

	HANDLE hCpp = CreateFile(output, GENERIC_WRITE|GENERIC_READ, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCpp == INVALID_HANDLE_VALUE)
	{
		printf("INVALID_HANDLE_VALUE");
		return -1;
	}

	printf("正在转换...\n");

	unsigned char ch = 0;
	DWORD dwReadSize = 0;
	DWORD dwWrittenSize = 0;
	DWORD dwFileSize = GetFileSize(hRsrc, NULL);

	char header[1024] = {0};
	sprintf_s(header, "%s%s%d%s%s%d%s","#pragma once\n#include \"StdAfx.h\"\n", "int g_dataLen = ", dwFileSize + 25, 
		";\n", "unsigned char g_rsrcData[", dwFileSize + 25, "] = {\n");

	WriteFile(hCpp, header, strlen(header), &dwWrittenSize, NULL);

	char radihex[2] = {0};
	char buff[10] = {0};
	int cnt = 0;
	do
	{
		if(!ReadFile(hRsrc, &ch, sizeof(char), &dwReadSize, NULL))
			break;
		memset(radihex, 0, sizeof(radihex));
		memset(buff, 0, sizeof(buff));
		cnt++;
		if (cnt % 16 == 0 && cnt != 0)
		{
			sprintf_s(buff, "0x%02x,\n", ch);
			WriteFile(hCpp, buff, strlen(buff), &dwWrittenSize, NULL);
		}
		else if (cnt == dwFileSize + 1)
		{
			sprintf_s(buff, "0x%02x", ch);
			WriteFile(hCpp, buff, strlen(buff), &dwWrittenSize, NULL);
		}
		else
		{
			sprintf_s(buff, "0x%02x,", ch);
			WriteFile(hCpp, buff, strlen(buff), &dwWrittenSize, NULL);
		}

	} while (dwReadSize);

	char tail[1024] = {0};
	sprintf_s(tail, "%s", "\n};");
	WriteFile(hCpp, tail, strlen(tail), &dwWrittenSize, NULL);
	
	FlushFileBuffers(hCpp);
	CloseHandle(hRsrc);
	CloseHandle(hCpp);

	printf("转换完成!\n");
	//Sleep(1000);

	return 0;
}