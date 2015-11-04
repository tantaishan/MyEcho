
#pragma once

#include <map>
#include <atlutil.h>
#include "stdafx.h"
#include "downloader.h"

//��������������
class CDownloadWork
{
public:
	class DownInfo
	{
	public:
		LONG nID;		//��������ID
		CString sUrl;	//����url
		CString sPath;	//����ı���·��
		HWND hWnd;		//���֪ͨ����
		UINT nNotify;	//֪ͨ��ϢID

		DownInfo(LONG id, LPCTSTR szUrl, LPCTSTR szPath, HWND hW, UINT nMsg)
		{
			nID = id;
			sUrl = szUrl;
			sPath = szPath;
			hWnd = IsWindow(hW) ? hW : NULL;
			nNotify = nMsg;
		}
	};

	typedef struct _RequestType
	{
		DownInfo *Obj;
	} *RequestType;

	virtual BOOL Initialize(void* pvWorkerParam);
	virtual void Terminate(void* pvWorkerParam);
	virtual BOOL GetWorkerData(DWORD dwParam, void ** ppvData);
	void Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped);	//���غ���
};

//���ع�����
class CDownloader
{
private:
	LONG m_ID;	//��ˮ����ID
	UINT m_ProcessMsg;	//���������ϢID
	CThreadPool<CDownloadWork> m_Pool;	//�̳߳ض���
	map<LONG, CDownloadWork::RequestType> m_DwndLst;	//��������� <����id�������������>

	BOOL TaskExists(LPCTSTR szUrl, LPCTSTR szSavePath);
public:
	CDownloader(UINT nMsgID, int nThreadNum);
	~CDownloader(void);

	LONG Add(LPCTSTR szUrl, LPCTSTR szSavePath, HWND hWnd);	//������������ ����������url������·��������¼���Ӧ���ڣ�
	void ClearFinished();	//�ֶ�����ѽ�������������
};

