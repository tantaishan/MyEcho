/*
 * $ Generic HTTP Client
 * ----------------------------------------------------------------------------------------------------------------
 *
 * name :          GenericHTTPClient
 *
 * version tag :     0.1.0
 *
 * description :    HTTP Client using WININET
 *
 * author :          Heo Yongsun ( gooshin@opentown.net )
 *
 * This code distributed under BSD LICENSE STYLE.
 */

#ifndef __GENERIC_HTTP_CLIENT
#define __GENERIC_HTTP_CLIENT

#include <Windows.h>
#include <tchar.h>

namespace http
{
	ULONG WINAPI HttpDownload(LPCTSTR RequestUrl, BYTE* pOutBuffer, int* cbRead);
}

#endif	// #ifndef __GENERIC_HTTP_CLIENT
