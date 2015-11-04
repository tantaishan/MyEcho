#pragma once
#include <map>
#include <string>

class INIReader
{
public:
	INIReader(std::string filename);
	INIReader(const char* buf);
	int	ParseError();
	std::string	Get(std::string section, std::string name, std::string default_value);
	long	GetInteger(std::string section, std::string name, long default_value);
	double	GetReal(std::string section, std::string name, double default_value);
	bool	GetBoolean(std::string section, std::string name, bool default_value);

private:
	static std::string MakeKey(std::string section, std::string name);
	static int ValueHandler(void* user, const char* section, const char* name, const char* value);

private:
	int	_error;
	std::map<std::string, std::string> _values;
};

