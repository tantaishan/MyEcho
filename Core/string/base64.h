#pragma once
#include <iostream>

class base64
{
public:
	static std::string base64_encode(unsigned char const* , unsigned int len);
	static std::string base64_decode(std::string const& s);
};

