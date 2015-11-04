
#pragma once

#include <map>
#include <atlutil.h>
#include "stdafx.h"
#include "downloader.h"

//单个的下载项类
class CDownloadWork
{
public:
	class DownInfo
	{
	public:
		LONG nID;		//下载任务ID
		CString sUrl;	//下载url
		CString sPath;	//保存的本地路径
		HWND hWnd;		//完成通知窗口
		UINT nNotify;	//通知消息ID

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
	void Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped);	//下载函数
};

//下载管理类
class CDownloader
{
private:
	LONG m_ID;	//流水自增ID
	UINT m_ProcessMsg;	//下载完成消息ID
	CThreadPool<CDownloadWork> m_Pool;	//线程池对象
	map<LONG, CDownloadWork::RequestType> m_DwndLst;	//下载任务表 <任务id，下载任务对象>

	BOOL TaskExists(LPCTSTR szUrl, LPCTSTR szSavePath);
public:
	CDownloader(UINT nMsgID, int nThreadNum);
	~CDownloader(void);

	LONG Add(LPCTSTR szUrl, LPCTSTR szSavePath, HWND hWnd);	//新增下载任务 参数（下载url、保存路径、完成事件响应窗口）
	void ClearFinished();	//手动清除已结束的下载任务
};

