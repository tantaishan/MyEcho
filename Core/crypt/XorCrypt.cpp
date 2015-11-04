
#include <Windows.h>
#include "XorCrypt.h"

std::string XorCrypt::Xor(__in const std::string& input,
	__in const std::string& key)
{
	std::string output;
	output.resize(input.size(), '\0');
	for (unsigned int i = 0, j = 0; i < input.size(); i++)
		output[i] = input[i] ^ key[j == key.size() ? 0 : j++];

	return output;
}

bool XorCrypt::Encrypt(__in const std::string& input,
	__in const std::string& output, __in const std::string& key)
{
	std::string encrypt = Xor(input, key);
	HANDLE hFile = ::CreateFileA(output.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	unsigned long written = 0;
	::WriteFile(hFile, encrypt.c_str(), encrypt.size(), &written, NULL);
	::FlushFileBuffers(hFile);
	::CloseHandle(hFile);
	return true;
}

bool XorCrypt::Decrypt(__in const std::string& input,
	__out std::string& output, __in const std::string& key)
{

	HANDLE hFile = ::CreateFileA(input.c_str(), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	unsigned long fileSize = ::GetFileSize(hFile, NULL);
	char* buff = new char[fileSize + 1];
	memset(buff, 0, fileSize + 1);
	unsigned long readSize = 0;
	::ReadFile(hFile, buff, fileSize, &readSize, NULL);

	output.resize(fileSize);
	memcpy(const_cast<char*>(output.c_str()), buff, fileSize);

	output = Xor(output, key);
	::CloseHandle(hFile);
	delete buff;
	return true;
}

bool XorCrypt::EncryptFile(__in const std::string& input,
	__in const std::string& output, __in const std::string& key)
{
	HANDLE hFile = ::CreateFileA(input.c_str(), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	unsigned long fileSize = ::GetFileSize(hFile, NULL);
	char* buff = new char[fileSize + 1];
	memset(buff, 0, fileSize + 1);
	unsigned long readSize = 0;
	::ReadFile(hFile, buff, fileSize, &readSize, NULL);
	::CloseHandle(hFile);
	std::string data;
	data.resize(fileSize);
	memcpy(const_cast<char*>(data.c_str()), buff, fileSize);
	delete buff;
	return Encrypt(data, output, key);
}

bool XorCrypt::DecryptFile(__in const std::string& input,
	__in const std::string& output, __in const std::string& key)
{
	std::string data;
	if (!Decrypt(input, data, key))
		return false;
	HANDLE hFile = ::CreateFileA(output.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	unsigned long written = 0;
	::WriteFile(hFile, data.c_str(), data.size(), &written, NULL);
	::FlushFileBuffers(hFile);
	::CloseHandle(hFile);
	return true;
}