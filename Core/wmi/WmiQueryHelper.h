#pragma once

#include <vector>
#include <wbemcli.h>

class WmiQueryHelper
{
public:
	WmiQueryHelper();
	virtual ~WmiQueryHelper();
	bool ExecQuery(IN const wchar_t * pcSqlW, IN const wchar_t * pcPropertyValue, OUT std::vector<std::wstring> & vecResult);

private:
	void SetComValid(bool bValid) {m_bIsComValid = bValid;}
	bool GetComValid() {return m_bIsComValid;}

	bool SetComSecurity();
	bool CreateComInstance();
	bool ConnectComServer();

	bool ProcessWbemClassObject(IN ULONG ulRc, 
		IN IWbemClassObject ** ppClassObject, 
		IN const wchar_t * pcPropertyValue,
		OUT std::vector<std::wstring> & vecResult);

	void VariantToString(const LPVARIANT pVar, std::wstring & strRc);

	void DataInit();
	void DataUnInit();

private:
	bool			m_bIsComValid;
	IWbemLocator *	m_pIWbemLocator;
	IWbemServices *	m_pWbemServices;
};
