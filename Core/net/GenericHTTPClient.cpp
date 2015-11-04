// GenericHTTPClient.cpp: implementation of the GenericHTTPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "GenericHTTPClient.h"
#include <IPTypes.h>
#include <winhttp.h>
#pragma comment(lib, "Winhttp.lib")

namespace http
{
	ULONG WINAPI HttpDownload(LPCTSTR RequestUrl, BYTE* pOutBuffer, int* cbRead)
	{
		if (!pOutBuffer || !cbRead)
			return ERROR_OUTOFMEMORY;

		ULONG Result = ERROR_NOT_READY;
		if(RequestUrl!=NULL)
		{
			TCHAR	HostName[MAX_HOSTNAME_LEN]={0};
			TCHAR	RequestPage[MAX_PATH]={0};
			TCHAR  Scheme[MAX_PATH]={0};
			URL_COMPONENTS	UrlComponents={0};

			UrlComponents.lpszHostName = HostName;
			UrlComponents.dwHostNameLength = _countof(HostName);
			UrlComponents.lpszUrlPath = RequestPage;
			UrlComponents.dwUrlPathLength = _countof(RequestPage);
			UrlComponents.dwStructSize = sizeof(URL_COMPONENTS);
			UrlComponents.lpszScheme = Scheme;
			UrlComponents.dwSchemeLength =_countof(Scheme);
			ULONG UrlLength =  _tcslen(RequestUrl);
			if(WinHttpCrackUrl(RequestUrl,UrlLength,0,&UrlComponents))
			{
				HINTERNET hSection = WinHttpOpen(TEXT("Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; Trident/6.0;Jizip_client)"),WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,NULL);
				if(hSection!=NULL)
				{
					HINTERNET hConnect = WinHttpConnect(hSection,UrlComponents.lpszHostName,UrlComponents.nPort,NULL);
					if(hConnect!=NULL)
					{
						HINTERNET hRequest = WinHttpOpenRequest(hConnect,TEXT("GET"),UrlComponents.lpszUrlPath,NULL, WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_REFRESH);
						if(hRequest!=NULL)
						{
							if(WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA, 0,0, 0))
							{
								if(WinHttpReceiveResponse( hRequest, NULL))
								{
									TCHAR StatusText[MAX_PATH];
									ZeroMemory(StatusText,sizeof(StatusText));
									ULONG NumberOfByteAvailable = _countof(StatusText);
									if(WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_STATUS_CODE,WINHTTP_HEADER_NAME_BY_INDEX,StatusText,&NumberOfByteAvailable,WINHTTP_NO_HEADER_INDEX))
									{
										if(_tstol(StatusText)==200)
										{
											ZeroMemory(StatusText,sizeof(StatusText));
											NumberOfByteAvailable = _countof(StatusText);
											ULONG	ContentLength = 0;
											if(WinHttpQueryHeaders(hRequest,WINHTTP_QUERY_CONTENT_LENGTH,WINHTTP_HEADER_NAME_BY_INDEX,StatusText,&NumberOfByteAvailable,WINHTTP_NO_HEADER_INDEX))
											{
												ContentLength = _tstol(StatusText);
												LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR,ContentLength);
												if(lpBuffer!=NULL)
												{
													LPBYTE ptr = lpBuffer;
													ULONG ReadSize = 0;
													ULONG TallRead = 0;
													do
													{
														if(WinHttpQueryDataAvailable(hRequest,&ReadSize))
														{
															if(ReadSize>ContentLength-TallRead) break;

															if(WinHttpReadData(hRequest,ptr,ReadSize,&ReadSize))
															{
																ptr+=ReadSize;
																TallRead+=ReadSize;
															}
														}
													}while(ReadSize>0);
													if(TallRead==ContentLength)
													{
														Result = ERROR_SUCCESS;
														if ((ULONG)*cbRead > TallRead)
														{
															*cbRead = TallRead;
														}
														CopyMemory(pOutBuffer, lpBuffer, *cbRead);
													}
													LocalFree(lpBuffer);
												}
											}else if(ERROR_WINHTTP_HEADER_NOT_FOUND == GetLastError())
											{
												LPBYTE lpBuffer = NULL;
												ULONG TallRead = 0;
												Result = ERROR_WINHTTP_HEADER_NOT_FOUND;
												while(WinHttpQueryDataAvailable(hRequest,&ContentLength))
												{
													if(ContentLength==0)
													{
														Result = ERROR_SUCCESS;
														break;
													}
													LPBYTE Buf = (LPBYTE)LocalAlloc(LPTR,ContentLength);
													ULONG ReadSize = 0;
													if(Buf==NULL)
													{
														if(lpBuffer!=NULL) LocalFree(lpBuffer);
														Result = ERROR_OUTOFMEMORY;
														TallRead = 0;
														lpBuffer = NULL;
														break;
													}
													if(WinHttpReadData(hRequest,Buf,ContentLength,&ReadSize))
													{
														LPBYTE Temp = (LPBYTE)LocalAlloc(LPTR,TallRead+ReadSize);
														if(Temp==NULL)
														{
															if(Buf!=NULL) LocalFree(Buf);
															if(lpBuffer!=NULL) LocalFree(lpBuffer);
															Result = ERROR_OUTOFMEMORY;
															TallRead = 0;
															lpBuffer = NULL;
															break;
														}
														if(lpBuffer!=NULL)
														{
															CopyMemory(Temp,lpBuffer,TallRead);
															LocalFree(lpBuffer);
														}
														CopyMemory(Temp+TallRead,Buf,ReadSize);
														lpBuffer = Temp;
														TallRead+=ReadSize;
													}
													LocalFree(Buf);
												}
												if(Result == ERROR_SUCCESS)
												{
													if (lpBuffer)
													{
														if ((ULONG)*cbRead > TallRead)
														{
															*cbRead = TallRead;
														}
														CopyMemory(pOutBuffer, lpBuffer, *cbRead);
														LocalFree(lpBuffer);
													}
												}
											}
										}
									}
								}
							}
							WinHttpCloseHandle(hRequest);
						}
						WinHttpCloseHandle(hConnect);
					}
					WinHttpCloseHandle(hSection);
				}
			}
		}
		return Result;
	}
}
