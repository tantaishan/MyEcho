#pragma once

#include "WmiQueryHelper.h"
#include "wbemidl.h"
#include <comdef.h>
#include <string>
#include <atlbase.h>
#include <atlconv.h>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "wbemuuid.lib")

WmiQueryHelper::WmiQueryHelper()
{
	DataInit();
	SetComValid(SUCCEEDED(CoInitialize(NULL)));
	SetComValid(SetComSecurity());
	SetComValid(CreateComInstance());
	SetComValid(ConnectComServer());
}

WmiQueryHelper::~WmiQueryHelper()
{
	DataUnInit();
	CoUninitialize();
}

bool WmiQueryHelper::SetComSecurity()
{
	HRESULT	hr	=	S_OK;

	if(!GetComValid())
		return false;

	/// Security needs to be initialized in XP first and this was the major problem 
	/// why it was not working in XP.

	hr = CoInitializeSecurity( NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		0);
	return (SUCCEEDED(hr) || RPC_E_TOO_LATE == hr) ? true : false;
}

bool WmiQueryHelper::CreateComInstance()
{
	HRESULT  hRc   = S_OK;

	if(!GetComValid())
		return false;

	hRc = CoCreateInstance( CLSID_WbemAdministrativeLocator,
		NULL ,
		CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER , 
		IID_IUnknown ,
		(void **) &m_pIWbemLocator);

	return (SUCCEEDED(hRc));
}

bool WmiQueryHelper::ConnectComServer()
{
	HRESULT hRc = S_OK;
	BSTR bstrNamespace = (L"root\\cimv2");

	if(!GetComValid())
		return false;
	hRc = m_pIWbemLocator->ConnectServer(   bstrNamespace,  // Namespace
		NULL,           // Userid
		NULL,           // PW
		NULL,           // Locale
		0,              // flags
		NULL,           // Authority
		NULL,           // Context
		&m_pWbemServices);
	return (SUCCEEDED(hRc));
}

bool WmiQueryHelper::ExecQuery(  IN const wchar_t * pcSqlW, 
	IN const wchar_t * pcPropertyValue, 
	OUT std::vector<std::wstring> & vecResult)
{
	bool					bRc			=	true;
	HRESULT					hRc			=	S_OK;
	BSTR					strQuery	=	_bstr_t(pcSqlW);
	BSTR					strQL		=	(L"WQL");
	IEnumWbemClassObject *	pEnumObject	=	NULL;

	const size_t			nClassObject	=	1;
	const ULONG				ulCount			=	1;  ///< pClassObject 的数量是1
	IWbemClassObject *		pClassObject[nClassObject];

	ULONG					ulRc	= 0;
	ULONG					ulIndex = 0;

	vecResult.clear();
	for (ulIndex = 0; ulIndex < nClassObject; ulIndex++)
		pClassObject[ulIndex] = NULL;

	if(!GetComValid())
		return false;

	hRc = m_pWbemServices->ExecQuery(strQL, strQuery,WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumObject);
	if (!SUCCEEDED(hRc))
		return false;

	hRc = pEnumObject->Reset();
	if (!SUCCEEDED(hRc))
	{
		bRc = false;
		goto ExecComQuery_END;
	}

	do
	{
		hRc = pEnumObject->Next(WBEM_INFINITE, ulCount, &pClassObject[0], &ulRc);
		if (!SUCCEEDED(hRc))
			goto ExecComQuery_END;

		ProcessWbemClassObject(ulRc, &pClassObject[0], pcPropertyValue, vecResult);
		for (ulIndex = 0; ulIndex < nClassObject; ulIndex++)
		{
			if(NULL != pClassObject[ulIndex])
			{
				pClassObject[ulIndex]->Release();
				pClassObject[ulIndex] = NULL;
			}
		}
	} while (WBEM_S_NO_ERROR == hRc);

ExecComQuery_END:
	for (ulIndex = 0; ulIndex < nClassObject; ulIndex++)
	{
		if (NULL != pClassObject[ulIndex])
		{
			pClassObject[ulIndex]->Release();
			pClassObject[ulIndex] = NULL;
		}
	}

	if (NULL != pEnumObject)
		pEnumObject->Release();

	return bRc;
}

void WmiQueryHelper::VariantToString(const LPVARIANT pVar, std::wstring & strRc)
{  
	USES_CONVERSION;

	CComBSTR HUGEP *	pBstr	= NULL;
	BYTE HUGEP*			pBuf	= NULL;
	LONG				llow	= 0;
	LONG				lhigh	= 0;
	LONG				lIndex	= 0;
	HRESULT				hRc		= S_OK;
	wchar_t				cBufW[_MAX_PATH];

	switch(pVar->vt)
	{
	case VT_BSTR:
		{
			strRc = (wchar_t *)_bstr_t(pVar->bstrVal);
		}
		break;
	case VT_BOOL:
		{
			if(VARIANT_TRUE==pVar->boolVal)
				strRc = L"TRUE";
			else  
				strRc=  L"FALSE";
		}
		break;
	case VT_I4:
		{
			StringCchPrintf( cBufW, sizeof(cBufW) / sizeof(wchar_t), TEXT("%d"), pVar->lVal);
			strRc = cBufW;
		}
		break;
	case VT_UI1:
		{
			StringCchPrintf( cBufW, sizeof(cBufW) / sizeof(wchar_t), TEXT("%d"), pVar->bVal);
			strRc = cBufW;
		}
		break;
	case VT_UI4:
		{
			StringCchPrintf( cBufW, sizeof(cBufW) / sizeof(wchar_t), TEXT("%d"), pVar->ulVal);
			strRc = cBufW;
		}
		break;

	case (VT_BSTR | VT_ARRAY):
		{
			hRc=SafeArrayAccessData(pVar->parray,(void HUGEP**)&pBstr);  
			hRc=SafeArrayUnaccessData(pVar->parray);  
			strRc = (wchar_t *)_bstr_t(pBstr->m_str);  
		}
		break;

	case (VT_I4 | VT_ARRAY):
		{
			SafeArrayGetLBound(pVar->parray, 1, &llow);
			SafeArrayGetUBound(pVar->parray, 1, &lhigh);

			hRc = SafeArrayAccessData(pVar->parray,(void HUGEP**)&pBuf);
			hRc = SafeArrayUnaccessData(pVar->parray);
			strRc = L"";
			for (lIndex=llow; lIndex<=lhigh; ++lIndex)
			{
				StringCchPrintf(cBufW, sizeof(cBufW) / sizeof(wchar_t), TEXT("%d"), pBuf[lIndex]);
				strRc += cBufW;
			}
		}
		break;
	default:
		break;
	}
}

bool WmiQueryHelper::ProcessWbemClassObject(IN ULONG ulRc, 
	IN IWbemClassObject ** ppClassObject, 
	IN const wchar_t * pcPropertyValue,
	OUT std::vector<std::wstring> & vecResult)
{
	VARIANT			v;
	HRESULT			hRc		= S_OK;
	ULONG			ulIndex	=  0;
	std::wstring	strW	= L"";
	/*wchar_t			cBufW[_MAX_PATH];*/

	if ((NULL == ppClassObject) || (NULL == *ppClassObject))
		return false;

	VariantInit(&v);
	for (ulIndex = 0; ulIndex < ulRc; ulIndex++)
	{
		hRc = (*(ppClassObject + ulIndex))->Get(pcPropertyValue, 0, &v, 0, 0);
		if(SUCCEEDED(hRc))
		{
			VariantToString(&v, strW);
			vecResult.push_back(strW);
		}
	}

	VariantClear(&v);
	return true;
}

void WmiQueryHelper::DataInit()
{
	m_bIsComValid = false;
	m_pIWbemLocator = NULL;
	m_pWbemServices = NULL;
}

void WmiQueryHelper::DataUnInit()
{
	if (NULL != m_pWbemServices)
		m_pWbemServices->Release();

	if (NULL != m_pIWbemLocator)
		m_pIWbemLocator->Release();
}