#pragma once

#include "../types.h"
#include "../string/string_helper.h"

class base_parser
{
protected:
	base_parser(){}
	virtual ~base_parser(){}

public:
	template <class T>
	T get(hsctstr keyname, T default_value)
	{
		throw hsexception();
	}

	template <>
	hsstring get(hsctstr keyname, hsstring default_value)
	{
		return this->get_string(keyname, default_value.c_str());
	}

	template <>
	int get(hsctstr keyname, int default_value)
	{
		hsstring value = string_helper::str_from_int(default_value);
		hsstring str = this->get_string(keyname, value.c_str());
		return string_helper::str_to_int(str);
	}

	template <>
	bool get(hsctstr keyname, bool default_value)
	{
		hsstring value = string_helper::str_from_int(default_value);
		hsstring str = this->get_string(keyname, value.c_str());
		return string_helper::str_to_int(str) != 0;
	}

	template <>
	double get(hsctstr keyname, double default_value)
	{
		hsstring value = string_helper::str_from_double(default_value);
		hsstring str = this->get_string(keyname, value.c_str());
		return string_helper::str_to_double(str);
	}

	template <>
	__int64 get(hsctstr keyname, __int64 default_value)
	{
		hsstring value = string_helper::str_from_int64(default_value);
		hsstring str = this->get_string(keyname, value.c_str());
		return string_helper::str_to_int64(str);
	}

	template <class T>
	void put(hsctstr keyname, T value)
	{
		throw hsexception();
	}

	template <>
	void put(hsctstr keyname, hsstring value)
	{
		this->put_string(keyname, value.c_str());
	}

	template <>
	void put(hsctstr keyname, int value)
	{
		hsstring str = string_helper::str_from_int(value);
		this->put_string(keyname, str.c_str());
	}

	template <>
	void put(hsctstr keyname, bool value)
	{
		hsstring str = string_helper::str_from_int(value);
		this->put_string(keyname, str.c_str());
	}

	template <>
	void put(hsctstr keyname, double value)
	{
		hsstring str = string_helper::str_from_double(value);
		this->put_string(keyname, str.c_str());
	}

	template <>
	void put(hsctstr keyname, __int64 value)
	{
		hsstring str = string_helper::str_from_int64(value);
		this->put_string(keyname, str.c_str());
	}

protected:
	virtual hsstring get_string(hsctstr keyname, hsctstr default_value) = 0;
	virtual void put_string(hsctstr keyname, hsctstr value) = 0;
};
