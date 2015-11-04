#include "StdAfx.h"
#include "downloader.h"
#include "../Core/client/ClientAPI.h"

CDownloader::CDownloader(UINT nMsgID, int nThreadNum) : m_ID(0), m_ProcessMsg(nMsgID)
{
	if (SUCCEEDED(m_Pool.Initialize((LPVOID)nMsgID, nThreadNum)))
	{
		m_Pool.SetTimeout(1000);
	}
}

CDownloader::~CDownloader(void)
{
	m_Pool.Shutdown();

	map<LONG, CDownloadWork::RequestType>::iterator p = m_DwndLst.begin();
	while (p != m_DwndLst.end())
	{
		if (p->second->Obj) delete p->second->Obj;
		delete p->second;
		p++;
	}
	m_DwndLst.clear();
}

BOOL CDownloader::TaskExists(LPCTSTR szUrl, LPCTSTR szSavePath)
{
	map<LONG, CDownloadWork::RequestType>::iterator p = m_DwndLst.begin();
	while (p != m_DwndLst.begin())
	{
		CDownloadWork::DownInfo *cInf = p->second->Obj;
		if (cInf && cInf->sUrl == szUrl && cInf->sPath == szSavePath)
		{
			return TRUE;
		}
		p++;
	}
	return FALSE;
}

void CDownloader::ClearFinished()
{
	map<LONG, CDownloadWork::RequestType>::iterator p = m_DwndLst.begin();
	while (p != m_DwndLst.end())
	{
		if (!p->second->Obj)
		{
			delete p->second;
			p = m_DwndLst.erase(p);
		}
		else
		{
			p++;
		}
	}
}

LONG CDownloader::Add(LPCTSTR szUrl, LPCTSTR szSavePath, HWND hWnd)
{
	ClearFinished();
	if (TaskExists(szUrl, szSavePath)) return -1;

	LONG id = _InterlockedIncrement(&m_ID);
	CDownloadWork::RequestType pTask = new CDownloadWork::_RequestType;
	if (pTask)
	{
		pTask->Obj = new CDownloadWork::DownInfo(id, szUrl, szSavePath, hWnd, m_ProcessMsg);
		if (pTask->Obj)
		{
			pair<map<LONG, CDownloadWork::RequestType>::iterator, bool> ret =
				m_DwndLst.insert(pair<LONG, CDownloadWork::RequestType>(id, pTask));
			if (ret.second)
			{
				m_Pool.QueueRequest(pTask);
				return id;
			}
			delete pTask->Obj;//ʧ��
		}
		delete pTask;//ʧ��
	}
	return -1;
}

////////////////////////////////////////////////////////
BOOL CDownloadWork::Initialize(void* pvWorkerParam)
{
	return TRUE;
}

//��ʶ�ļ���ʽ,ֻ����gif��jpg��png��bmp
int CheckFileType(LPCTSTR szPath)
{
	UCHAR BmpHeader[] = { 5, 0x42, 0x4d, 0xf6, 0xb8, 0x59 };	//BMPͷ�ļ�
	UCHAR JPGHeader[] = { 3, 0xff, 0xd8, 0xff };		//JPG,JPEGͷ�ļ�
	UCHAR GIFHeader[] = { 4, 0x47, 0x49, 0x46, 0x38 };		//GIFͷ�ļ�
	UCHAR PNGHeader[] = { 8, 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A }; //PNGͷ�ļ�
	UCHAR *ppHeadA[] = { BmpHeader, JPGHeader, GIFHeader, PNGHeader };

	//�Զ����Ʒ�ʽ���ļ�����ȡǰcount���ֽ�
	int i, count = ppHeadA[0][0];
	for (i = 1; i<_countof(ppHeadA); i++)
	{
		if (ppHeadA[i][0]>count) count = ppHeadA[i][0];
	}

	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return 0;

	LPVOID pBuf = _alloca(count);
	if (!pBuf) return 0;
	ZeroMemory(pBuf, count);

	DWORD readed = 0;
	ReadFile(hFile, pBuf, count, &readed, NULL);
	CloseHandle(hFile);
	if (readed != count) return 0;

	//��ʼ����ļ�����
	for (i = 0; i < _countof(ppHeadA); i++)
	{
		if (memcmp(pBuf, &ppHeadA[i][1], ppHeadA[i][0]) == 0)
		{
			return i + 1;
		}
	}

	return 0;
}

void CDownloadWork::Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped)
{
	CDownloadWork::DownInfo *pObj = request->Obj;

	//CString str;
	//str.Format(_T("%s?id=%d"), pObj->sUrl, GetTickCount());

	HRESULT hr = URLDownloadToFile(NULL, pObj->sUrl, pObj->sPath, 0, NULL); //�����ٶȱȽϿ� �����ܴ��������������
	if (!PathFileExists(pObj->sPath))
	{	//���ز��ɹ� ����������������libcurl��������
		ClientAPI_ErrorCode ret = ClientAPI::Instance()->Download((LPCTSTR)pObj->sUrl, (LPCTSTR)pObj->sPath);
	}
	if (IsWindow(pObj->hWnd) && pObj->nNotify &&
		PathFileExists(pObj->sPath))
	{	//�����������֪ͨ
		
		LPTSTR pPath = _tcsdup(pObj->sPath);
		PostMessage(pObj->hWnd, pObj->nNotify, 0, (LPARAM)pPath);
	}
	request->Obj = NULL;
	delete pObj;
}

void CDownloadWork::Terminate(void* pvWorkerParam)
{
}

BOOL CDownloadWork::GetWorkerData(DWORD dwParam, void ** ppvData)
{
	return FALSE;
}
