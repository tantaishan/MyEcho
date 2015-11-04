#pragma once
#include "base_parser.h"

class ini_parser : public base_parser
{
public:
	ini_parser(hsctstr filepath);
	virtual ~ini_parser();

protected:
	virtual hsstring get_string(hsctstr keyname, hsctstr default_value);
	virtual void put_string(hsctstr keyname, hsctstr value);

private:
	hststr m_filepath;
};
