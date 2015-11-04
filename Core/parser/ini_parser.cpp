#include "ini_parser.h"
#include "../string/string_helper.h"

ini_parser::ini_parser(hsctstr filepath)
{
	m_filepath = 0;
	if (filepath)
	{
		int len = wcslen(filepath) + 1;
		m_filepath = new hstchar[wcslen(filepath) + 1];
		wcscpy_s(m_filepath, len, filepath);
	}
}

ini_parser::~ini_parser()
{
	if (m_filepath)
	{
		delete []m_filepath;
		m_filepath = 0;
	}
}

hsstring ini_parser::get_string(hsctstr keyname, hsctstr default_value)
{
	//check filepath
	if (!m_filepath)
		return _T("");

	hsctstr p = wcschr(keyname, _T('.'));

	if (!p)
		throw _T("");

	hsstring appName(keyname, p);

	p++;

	hsstring keyName(p);

	if (wcschr(p, _T('.')))
		throw _T("");

	hstchar buffer[4096] = {0};
	GetPrivateProfileString(appName.c_str(), keyName.c_str(), default_value, buffer, sizeof(buffer), m_filepath);
	return buffer;
}

void ini_parser::put_string(hsctstr keyname, hsctstr value)
{
	if (!m_filepath)
		throw _T("");

	hsctstr p = wcschr(keyname, _T('.'));

	if (!p)
		throw _T("");

	hsstring appName(keyname, p);

	p++;

	hsstring keyName(p);

	if (wcschr(p, _T('.')))
		throw _T("");

	if(!WritePrivateProfileString(appName.c_str(), keyName.c_str(), value, m_filepath))
		throw _T("");
}