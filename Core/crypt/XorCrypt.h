#pragma once
#include <iostream>

class XorCrypt
{
public:
	static std::string Xor(const std::string& input, const std::string& key);
	static bool Encrypt(const std::string& input, const std::string& output, const std::string& key);
	static bool Decrypt( const std::string& input, std::string& output, const std::string& key);
	static bool EncryptFile(const std::string& input, const std::string& output, const std::string& key);
	static bool DecryptFile(const std::string& input, const std::string& output, const std::string& key);
};