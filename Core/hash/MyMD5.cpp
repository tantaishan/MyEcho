#include "MyMD5.h"
#include "md5.h"
#include <Windows.h>

std::string MyMD5::MD5(const std::string& str)
{
	MD5_CTX m_md5;
	MD5Init(&m_md5, 0);
	MD5Update(&m_md5, (unsigned char*)str.c_str(), str.length());
	MD5Final(&m_md5);
	char sz[34]={};

	for(size_t i = 0; i < 8; ++i)
	{
		sprintf_s(sz, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			m_md5.digest[0],m_md5.digest[1],m_md5.digest[2],m_md5.digest[3],
			m_md5.digest[4],m_md5.digest[5],m_md5.digest[6],m_md5.digest[7],
			m_md5.digest[8],m_md5.digest[9],m_md5.digest[10],m_md5.digest[11],
			m_md5.digest[12],m_md5.digest[13],m_md5.digest[14],m_md5.digest[15]);
	}

	return sz;
}

std::wstring MyMD5::MD5(const std::wstring& wstr)
{
	std::string str;
	int iLen = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (iLen > 0)
	{
		char* pszDst = new char[iLen];
		if (pszDst)
		{
			::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pszDst, iLen, NULL, NULL);
			pszDst[iLen -1] = 0;
			str= pszDst;
			delete [] pszDst;
		}
	}
	std::string md5 = MD5(str);

	std::wstring wmd5;
	iLen = ::MultiByteToWideChar(CP_ACP, 0, md5.c_str(), -1, NULL, 0);
	if (iLen > 0)
	{
		wchar_t* pwszDst = new wchar_t[iLen];
		if(pwszDst)
		{
			::MultiByteToWideChar(CP_ACP, 0, md5.c_str(), -1, pwszDst, iLen);
			pwszDst[iLen -1] = 0;
			wmd5= pwszDst;
			delete []pwszDst;
		}
	}

	return wmd5;
}