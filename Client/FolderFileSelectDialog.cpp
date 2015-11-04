
#include "stdafx.h"
#include "FolderFileSelectDialog.h"
#include <shlobj.h>
#include <exception>

#pragma comment( lib, "shell32.lib")
using std::exception;
#pragma warning( push )
#pragma warning( disable : 4311 4312 )

#define MAX_PATH_EditControl   4*1024*1024

WNDPROC CFolderFileSelectDialog::m_wndProc = NULL;


IMPLEMENT_DYNAMIC(CFolderFileSelectDialog, CFileDialog)


CFolderFileSelectDialog::CFolderFileSelectDialog(BOOL bOpenFileDialog,
							 LPCTSTR lpszDefExt,
							 LPCTSTR lpszFileName,
							 DWORD dwFlags,
							 LPCTSTR lpszFilter,
							 CWnd* pParentWnd)
							 :CFileDialog(
							 bOpenFileDialog,
							 lpszDefExt,
							 lpszFileName,
							 dwFlags | OFN_EXPLORER | OFN_HIDEREADONLY & (~OFN_SHOWHELP),
							 lpszFilter,
							 pParentWnd,
							 0,FALSE)
							 , m_bSelectVirtualItem(false)
							 , m_bCmb13Changed(false)
{
	dwFlags |= (OFN_EXPLORER | OFN_HIDEREADONLY & (~OFN_SHOWHELP));

}

CFolderFileSelectDialog::~CFolderFileSelectDialog()
{
}

BEGIN_MESSAGE_MAP(CFolderFileSelectDialog, CFileDialog)	
END_MESSAGE_MAP()


static BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

/*
* 检查一个  路径 是否存在（绝对路径、相对路径，文件或文件夹均可）
* 存在则返回 1 (TRUE)
*/
static BOOL FilePathExists(LPCTSTR lpPath)
{
	return FindFirstFileExists(lpPath, FALSE);
}

//判断是否是文件绝对路径，不管是否存在
static BOOL IsFileAbsolutePath(LPCTSTR lpPath)
{
	CString str(lpPath);
	int iPos = str.Find(_T(":\\"));
	if (iPos != 1) return FALSE;
	return TRUE;
}

static BOOL FileAbsolutePathExists(LPCTSTR lpPath)
{
	CString str(lpPath);
	int iPos = str.Find(_T(":\\"));
	if (iPos != 1) return FALSE;
	return FindFirstFileExists(lpPath, FALSE);
}

/*
[cpp] view plaincopyprint?
* 检查一个 文件夹 是否存在（绝对路径、相对路径均可）
[cpp] view plaincopyprint?
* 存在则返回 1 (TRUE)
[cpp] view plaincopyprint?
*/
static BOOL FolderExists(LPCTSTR lpPath)
{
	return FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY);
}

static BOOL IsLikeDesktopFolder(CString& sSourcFolder)
{
	TCHAR szPath[MAX_PATH] = {0};
	::GetWindowsDirectory(szPath, MAX_PATH);
	::PathStripToRoot(szPath);
	::PathAppend(szPath, _T("Users"));

	CString sFeature1(szPath);
	sFeature1 = sFeature1.MakeLower();
	CString sFeature2(_T("Desktop"));
	sFeature2 = sFeature2.MakeLower();

	CString sLowSourceFolder = sSourcFolder.MakeLower();
	int idx = sLowSourceFolder.Find(sFeature1);
	if (0 == idx)
	{
		idx = sLowSourceFolder.Find(sFeature2);
		if (idx > 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*
部分文件在c:\user\pubilc\desktop下面，也会显示在桌面上，当遇到这样文件时，还原出实际路径
*/
static BOOL CheckPublicDesktopFile(CString& sSourcFolder, CString& sFileName, CString& sNewFullPath)
{
	if (IsLikeDesktopFolder(sSourcFolder))
	{
		TCHAR szPath[MAX_PATH] = {0};
		::GetWindowsDirectory(szPath, MAX_PATH);
		::PathStripToRoot(szPath);
		::PathAppend(szPath, _T("Users\\Public\\Desktop"));
		::PathAppend(szPath, sFileName);
		if (FilePathExists(szPath))
		{
			sNewFullPath = szPath;
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL CheckPublicDesktopFolder(CString& sSourcFolder, CString& sFolderName, CString& sNewFullPath)
{
	if (IsLikeDesktopFolder(sSourcFolder))
	{
		TCHAR szPath[MAX_PATH] = {0};
		::GetWindowsDirectory(szPath, MAX_PATH);
		::PathStripToRoot(szPath);
		::PathAppend(szPath, _T("Users\\Public\\Desktop"));
		::PathAppend(szPath, sFolderName);
		if (FolderExists(szPath))
		{
			sNewFullPath = szPath;
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL GetKnowLibraryFilesAndFolders(CStringArray& vList)
{
	const GUID guidlist[4] = { FOLDERID_DocumentsLibrary, FOLDERID_MusicLibrary,
		FOLDERID_PicturesLibrary, FOLDERID_VideosLibrary };
	IEnumShellItems* pEnum;
	IShellItem* pItem;
	LPWSTR* pDisplayName;
	HRESULT hr;

	for (int i = 0; i < 4; i++)
	{
		hr = SHGetKnownFolderItem(guidlist[i], KF_FLAG_CREATE, 0, IID_PPV_ARGS(&pItem));
		if (FAILED(hr)) continue;

		hr = pItem->BindToHandler(NULL, BHID_EnumItems, IID_PPV_ARGS(&pEnum));
		if (FAILED(hr)) continue;

		pItem = NULL;
		while (SUCCEEDED(pEnum->Next(1, &pItem, NULL)))
		{
			if (NULL == pItem) break;
			try
			{
				pDisplayName = (LPWSTR*) CoTaskMemAlloc(MAX_PATH);
				memset(pDisplayName, 0, MAX_PATH); //SIGDN_NORMALDISPLAY
				if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, pDisplayName)))
				{
					vList.Add(CString((TCHAR*) *pDisplayName));
				}
			}
			catch (exception ex)
			{
				CoTaskMemFree(pDisplayName);
				pDisplayName = NULL;
			}


			CoTaskMemFree(pDisplayName);
			pItem = NULL;
		}
	}

	return TRUE;

}

//static void  GetShellLibraryNameforLibrary(CStringArray& list)
//{
//	IShellLibrary* pSL;
//	IEnumShellItems* pEnum;
//	IShellItem* pItem;
//	LPWSTR* pDisplayName;
//	HRESULT hr;
//	//hr = SHGetKnownFolderItem(FOLDERID_DocumentsLibrary, KF_FLAG_CREATE, 0, IID_PPV_ARGS(&pItem));
//
//	hr = SHGetKnownFolderItem(FOLDERID_Libraries, KF_FLAG_CREATE, 0, IID_PPV_ARGS(&pItem));
//	if (FAILED(hr)) return ;
//
//	hr = pItem->BindToHandler(NULL, BHID_EnumItems, IID_PPV_ARGS(&pEnum));
//	if (FAILED(hr)) return ;
//
//	pItem = NULL;
//	while (SUCCEEDED(pEnum->Next(1, &pItem, NULL)))
//	{
//		if (NULL == pItem) break;
//		try
//		{
//			pDisplayName = (LPWSTR*) CoTaskMemAlloc(MAX_PATH);
//			memset(pDisplayName, 0, MAX_PATH); //SIGDN_NORMALDISPLAY
//			if (SUCCEEDED(pItem->GetDisplayName(SIGDN_NORMALDISPLAY, pDisplayName)))
//			{
//				list.Add(CString((TCHAR*) *pDisplayName));
//			}
//		}
//		catch (exception ex)
//		{
//			CoTaskMemFree(pDisplayName);
//			pDisplayName = NULL;
//		}
//
//
//		CoTaskMemFree(pDisplayName);
//		pItem = NULL;
//	}
//}

////1:成功找到文件， 0：没有找到文件夹， -1：是库子文件夹，但是不是系统默认的库文件夹，无法匹配选择的文件
//static int FindCurFloderFiles(CFileDialog* pDlg, CStringArray& sCurFolderFileList)
//{
//	sCurFolderFileList.RemoveAll();
//	CString sCurVirtualFolder;
//
//	CWnd* pcmbWnd = pDlg->GetDlgItem(cmb2);
//	if (pcmbWnd)
//		pcmbWnd->GetWindowTextW(sCurVirtualFolder);
//
//	CStringArray subLibraryNameList;
//	GetShellLibraryNameforLibrary(subLibraryNameList);
//	int iCount = subLibraryNameList.GetCount();
//	bool bFindflag = false;
//	for (int i = 0; i < iCount; i++)
//	{
//		if (sCurVirtualFolder.CompareNoCase(subLibraryNameList.GetAt(i)) == 0)
//		{
//			bFindflag = true;
//			continue;
//		}
//	}
//	if (!bFindflag) return 0;
//
//	CStringArray libraryFilesAndFoldersList;
//	if (!GetKnowLibraryFilesAndFolders(libraryFilesAndFoldersList)) return false;
//
//	CWnd* pWnd = pDlg->GetDlgItem(lst2);
//	if (pWnd == NULL)
//		return 0;
//	CListCtrl* wndLst1 = (CListCtrl*) (pWnd->GetDlgItem(1));
//	int nSelected = wndLst1->GetSelectedCount();
//	if (!nSelected)
//		return 0;
//
//	int nItem = wndLst1->GetNextItem(-1, LVNI_SELECTED);
//
//	CString strItemText;
//	while ((nSelected--) > 0)
//	{
//		strItemText = wndLst1->GetItemText(nItem, 0);
//
//		iCount = libraryFilesAndFoldersList.GetCount();
//		for (int i = 0; i < iCount; i++)
//		{
//			CString str = libraryFilesAndFoldersList.GetAt(i);
//			int ipos = str.ReverseFind(_T('\\'));
//			if (ipos >0)
//			{
//				CString strtmp = str.Mid(ipos + 1);
//				//if (strItemText.CompareNoCase(strtmp) == 0)
//				if (strtmp.Find(strItemText) == 0)
//				{
//					sCurFolderFileList.Add(str);
//					break;
//				}
//			}
//		}
//
//		nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED);
//	}
//
//	if (sCurFolderFileList.GetCount() <= 0) return -1;
//	
//	return 1;
//}

BOOL CFolderFileSelectDialog::OnFileNameOK()
{
	m_bSelectVirtualItem = false;
	CWnd* pParent = GetParent();
	CFileDialog* pDlg = (CFileDialog*) CWnd::FromHandle(GetParent()->m_hWnd);
	if (pDlg)
	{
		CWnd* pWnd = pDlg->GetDlgItem(lst2);	
		if (pWnd == NULL)
			return FALSE;

		m_SelectedItemList.RemoveAll();			
		
		CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));

		int nSelected = wndLst1->GetSelectedCount();
		if (!nSelected)		
			return FALSE;
		CString strItemText, strDirectory = m_strCurrendDirectory;
		if (strDirectory.Right(1) != _T("\\"))
			strDirectory += _T("\\");

		CString fileslist = _T("");
		pDlg->SendMessage(CDM_GETSPEC, (WPARAM) MAX_PATH_EditControl,
			(LPARAM) fileslist.GetBuffer(MAX_PATH_EditControl));
		fileslist.ReleaseBuffer();

		//if (m_strCurrendDirectory.GetLength() <= 0)
		//{
		//	//只有在库目录里才会有出现。
		//	//现在只检索系统默认的几个子库文件夹，第三方软件建立的库目录会提示错误。
		//	//目前算是部分解决
		//	CStringArray sFolderAndFileList;
		//	int iRtn = FindCurFloderFiles(pDlg, sFolderAndFileList);
		//	switch (iRtn)
		//	{
		//	case 0:
		//		return 1;
		//		break;
		//	case 1:
		//		m_SelectedItemList.Copy(sFolderAndFileList);
		//		pDlg->EndDialog(IDOK);
		//		return 1;
		//		break;
		//	case -1:
		//		m_bSelectVirtualItem = true;
		//		pDlg->EndDialog(IDCANCEL);
		//		return 1;
		//		break;
		//	}
		//}

		strItemText = strDirectory + fileslist;
		if(nSelected == 1 && fileslist != _T(""))
		{
			CString sTmpNewFileFullPath;
			if (FilePathExists(strItemText))
			{
				m_SelectedItemList.Add(strItemText);
				return CFileDialog::OnFileNameOK();
			}
			else if (CheckPublicDesktopFile(strDirectory, fileslist, sTmpNewFileFullPath))
			{
				m_SelectedItemList.Add(sTmpNewFileFullPath);
				return CFileDialog::OnFileNameOK();
			}
			m_bSelectVirtualItem = true;
			//::MessageBoxW(NULL, m_strError.GetBuffer(), NULL, NULL);
			//return NULL;
		}
	}
	::MessageBeep( MB_ICONQUESTION );
	return 1; 
}

void CFolderFileSelectDialog::OnFolderChange()
{
	//TRACE(_T("CMyOpenDlg::OnFolderChange: \n"));

	CWnd* pcmbWnd = GetParent()->GetDlgItem(cmb13);
	if (pcmbWnd)
		pcmbWnd->SetWindowTextW(_T(""));

	m_strCurrendDirectory = GetFolderPath();

	//SetListView(ODM_VIEW_LARGE_ICONS);

	CFileDialog::OnFolderChange();
}

BOOL CFolderFileSelectDialog::OnInitDialog()
{
	//TRACE(_T("CMyOpenDlg::OnInitDialog, hwnd=%p\n"), m_hWnd);
	return CFileDialog::OnInitDialog();
}

void CFolderFileSelectDialog::OnInitDone()
{
	//TRACE(_T("CMyOpenDlg::OnInitDone\n"));
#ifdef _WIN64

	m_strCurrendDirectory = GetFolderPath();
	CWnd* pFD = GetParent();

	HideControl(edt1);
	HideControl(cmb1);
	HideControl(stc2);

	//HideControl(cmb13);
	//HideControl(stc3);

	/*CRect rectCancel; pFD->GetDlgItem(IDCANCEL)->GetWindowRect(&rectCancel);
	pFD->ScreenToClient(&rectCancel);

	CRect rectOK; pFD->GetDlgItem(IDOK)->GetWindowRect(&rectOK);
	pFD->ScreenToClient(&rectOK);
	pFD->GetDlgItem(IDOK)->SetWindowPos(0,rectCancel.left - rectOK.Width() - 5, rectCancel.top, 0,0, SWP_NOZORDER | SWP_NOSIZE);

	CRect rectList2; pFD->GetDlgItem(lst1)->GetWindowRect(&rectList2);
	pFD->ScreenToClient(&rectList2);
	pFD->GetDlgItem(lst1)->SetWindowPos(0,0,0,rectList2.Width(), abs(rectList2.top - (rectCancel.top - 5)), SWP_NOMOVE | SWP_NOZORDER);

	CRect rectStatic; pFD->GetDlgItem(stc3)->GetWindowRect(&rectStatic);
	pFD->ScreenToClient(&rectStatic);
	pFD->GetDlgItem(stc3)->SetWindowPos(0, rectCancel.left - 375, rectCancel.top + 5, rectStatic.Width(), rectStatic.Height(), SWP_NOZORDER);

	CRect rectEdit1; pFD->GetDlgItem(cmb13)->GetWindowRect(&rectEdit1);
	pFD->ScreenToClient(&rectEdit1);
	pFD->GetDlgItem(cmb13)->SetWindowPos(0, rectCancel.left - 320, rectCancel.top, rectEdit1.Width() - 15, rectEdit1.Height(), SWP_NOZORDER);*/

	SetControlText(stc3, m_strItemName.GetBuffer());
	SetControlText(IDOK, m_strSelect.GetBuffer());

	SetLastError(0);
	m_wndProc = (WNDPROC)::SetWindowLongPtr(pFD->m_hWnd, GWLP_WNDPROC, (LONG_PTR) WindowProcNew);
	DWORD errorNo =  GetLastError();
	pFD->CenterWindow();
#elif WIN32
	m_strCurrendDirectory = GetFolderPath();
	CWnd* pFD = GetParent();

	HideControl(edt1);
	HideControl(cmb1);
	HideControl(stc2);

	//HideControl(0x4a0);

	SetControlText(stc3, m_strItemName.GetBuffer());
	SetControlText(IDOK, m_strSelect.GetBuffer());

	SetControlText(cmb13, _T(""));
	
	::SetWindowLong(pFD->m_hWnd, GWL_USERDATA, (long)this);
	m_wndProc = (WNDPROC)::SetWindowLong(pFD->m_hWnd, GWL_WNDPROC, (long)WindowProcNew);
	pFD->CenterWindow();
#endif
}


//////////////////
// Change the list view to desired mode if the view exists.
// Display TRACE diagnostics either way.
//
BOOL CFolderFileSelectDialog::SetListView(LISTVIEWCMD cmd)
{
	// note that real dialog is my parent, not me
	CWnd* pshell = GetParent()->GetDlgItem(lst2);
	if (pshell) {
		//TRACE(_T("hwnd=%p.\n"), m_wndList.GetSafeHwnd());
		//TRACE(_T("SendMessage\n"));
		pshell->SendMessage(WM_COMMAND, cmd);
		return TRUE;
	}
	//TRACE(_T("failed.\n"), m_wndList.GetSafeHwnd());
	return FALSE;
}


void CFolderFileSelectDialog::OnFileNameChange()
{
	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return ;
	CListCtrl* wndLst1 = (CListCtrl*) (pWnd->GetDlgItem(1));
	int nSelected = wndLst1->GetSelectedCount();
	if (!nSelected)
	{
		CWnd* pcmbWnd = GetParent()->GetDlgItem(cmb13);
		if (pcmbWnd)
			pcmbWnd->SetWindowTextW(_T(""));
		m_strLastSelectList = _T("");
		return ;
	}

	CString strItemText, strDirectory = m_strCurrendDirectory;
	if (strDirectory.Right(1) != _T("\\"))
		strDirectory += _T("\\");

	CString fileslist = _T("");

	CWnd* pcmbWnd = GetParent()->GetDlgItem(cmb13);
	if (pcmbWnd)
		pcmbWnd->GetWindowTextW(fileslist);

	bool bFileFolderSameName = false;
	bool bOnlyFolder = true;
	int nItem = wndLst1->GetNextItem(-1, LVNI_SELECTED);
	while ((nSelected--) > 0)
	{
		strItemText = wndLst1->GetItemText(nItem, 0);
		if (fileslist.Find(strItemText) >= 0) bFileFolderSameName = true;

		strItemText = strDirectory + strItemText;
		DWORD attr = GetFileAttributes(strItemText);
		if (!((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY)))
		{
			bOnlyFolder = false;
			break;
		}
		nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED);
	}

	while (bOnlyFolder)
	{
		//清空之前，判断是否cmb13中新写入值，如果是，就有可能是当扩展名隐藏，有同名的文件和文件夹的情况
		if (fileslist.GetLength() > 0)
		{
			if (fileslist.CompareNoCase(m_strLastSelectList))
			{
				break;
			}
			else if (m_bCmb13Changed && bFileFolderSameName && m_strLastSelectList.GetLength() > 0)
			{
				break;
			}
		}

		CWnd* pcmbWnd = GetParent()->GetDlgItem(cmb13);
		if (pcmbWnd)
			pcmbWnd->SetWindowTextW(_T(""));
		break;
	}

	m_strLastSelectList = fileslist;

	m_bCmb13Changed = false;
}

LRESULT CALLBACK CFolderFileSelectDialog::WindowProcNew(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message ==  WM_COMMAND)
	{
		CFolderFileSelectDialog* pThis = reinterpret_cast<CFolderFileSelectDialog*>(GetWindowLong(hwnd, GWL_USERDATA));
		if (HIWORD(wParam) == BN_CLICKED)
		{
			if (LOWORD(wParam) == IDCANCEL)
			{
				if (pThis)
					pThis->m_bSelectVirtualItem = false;
			}
			else if (LOWORD(wParam) == IDOK)
			{
				CFileDialog* pDlg = (CFileDialog*)CWnd::FromHandle(hwnd);
				if (pThis && pDlg)
				{
					pThis->m_bSelectVirtualItem = false;

					CString sFoldertmp = _T("");
					pDlg->SendMessage(CDM_GETFOLDERPATH, (WPARAM) MAX_PATH_EditControl,
						(LPARAM) sFoldertmp.GetBuffer(MAX_PATH_EditControl));
					sFoldertmp.ReleaseBuffer();

					CString fileslist = _T("");
					pDlg->SendMessage(CDM_GETSPEC, (WPARAM) MAX_PATH_EditControl,
						(LPARAM) fileslist.GetBuffer(MAX_PATH_EditControl));
					fileslist.ReleaseBuffer();
					DWORD tmpAttr = GetFileAttributes(fileslist);

					//如果用户在下拉列表中直接选择最近选择的文件，则直接选择之
					if (IsFileAbsolutePath(fileslist.GetBuffer()))
					{
						if (FindFirstFileExists(fileslist.GetBuffer(), FALSE))
						{
							pThis->m_SelectedItemList.Add(fileslist);
							pDlg->EndDialog(IDOK);
							return NULL;
						}
						else
						{
							pDlg->EndDialog(IDOK);
							return NULL;
						}	
					}

					//if (pThis->m_strCurrendDirectory.GetLength() <= 0)
					//{
					//	//只有在库目录里才会有出现。
					//	//现在只检索系统默认的几个子库文件夹，第三方软件建立的库目录会提示错误。
					//	//目前算是部分解决
					//	CStringArray sFolderAndFileList;
					//	int iRtn = FindCurFloderFiles(pDlg, sFolderAndFileList);
					//	switch (iRtn)
					//	{
					//	case 0:
					//		return CallWindowProc(m_wndProc, hwnd, message, wParam, lParam);
					//		break;
					//	case 1:
					//		pThis->m_SelectedItemList.Copy(sFolderAndFileList);
					//		pDlg->EndDialog(IDOK);
					//		return NULL;
					//		break;
					//	case -1:
					//		pThis->m_bSelectVirtualItem = true;
					//		pDlg->EndDialog(IDCANCEL);					
					//		return NULL;
					//		break;
					//	}	
					//}

					bool bHaveSelect = false;

					if ((tmpAttr != 0xFFFFFFFF) && (tmpAttr & FILE_ATTRIBUTE_DIRECTORY))
					{
						return CallWindowProc(m_wndProc, hwnd, message, wParam, lParam);
					}

					pThis->m_SelectedItemList.RemoveAll();			
					CWnd* pWnd = pDlg->GetDlgItem(lst2);	
					if (pWnd == NULL)
						return FALSE;
					CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
					int nSelected = wndLst1->GetSelectedCount();
					if (!nSelected)		
						return FALSE;
					CString strItemText, strDirectory = pThis->m_strCurrendDirectory;
					if (strDirectory.GetLength() > 1 && strDirectory.Right(1) != _T("\\"))
						strDirectory += _T("\\");
					/////////////////////////////////////////////////////////////////////////////
					//处理文件夹
					BOOL isLikeDesktopFlag = IsLikeDesktopFolder(strDirectory);
					int nItem = wndLst1->GetNextItem(-1,LVNI_SELECTED);

					CString sTmpFileeList = fileslist;
					while((nSelected--) > 0)
					{
						strItemText = wndLst1->GetItemText(nItem,0);
						if (sTmpFileeList.GetLength()>0) //处理当扩展名隐藏，有同名的文件和文件夹的情况
						{
							int idx = sTmpFileeList.Find(strItemText+_T("."));
							if (idx >= 0)
							{
								CString stmp1 = sTmpFileeList.Left(idx);
								CString stmp2 = sTmpFileeList.Mid(idx + strItemText.GetLength()+1);
								sTmpFileeList = stmp1 + stmp2;
								nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED);
								continue;								
							}
						}

						DWORD attr = GetFileAttributes(strDirectory + strItemText);
						if ((attr != 0xFFFFFFFF) && (attr & FILE_ATTRIBUTE_DIRECTORY))
						{
							pThis->m_SelectedItemList.Add(strDirectory + strItemText);
							bHaveSelect = true;
						}
						else if (isLikeDesktopFlag)
						{
							CString sTmpNewFolderFullPath;
							if (CheckPublicDesktopFolder(strDirectory, strItemText, sTmpNewFolderFullPath))
							{
								pThis->m_SelectedItemList.Add(sTmpNewFolderFullPath);
								bHaveSelect = true;
							}
						}
						nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED);
					}
					//////////////////////////////////////////////////////////////////////////////////////
					//处理文件
					strItemText = _T("");
					nSelected = wndLst1->GetSelectedCount();
					if(nSelected > pThis->m_SelectedItemList.GetCount())
					{
						int MoreThanOnFile = fileslist.Find(_T("\""));
						if(MoreThanOnFile != -1)
						{
							for(int i=0; i<fileslist.GetLength(); i++)
							{
								if(fileslist[i] != '\"')
								{
									strItemText.AppendFormat(_T("%c"),fileslist[i]);
									if(fileslist[i-1] == '\"' && fileslist[i] == ' ')
										strItemText.Delete(strItemText.GetLength()-1);
								}
								else if(!strItemText.IsEmpty())
								{
									if (FilePathExists(strDirectory + strItemText))
									{
										pThis->m_SelectedItemList.Add(strDirectory + strItemText);
										strItemText.Empty();
										bHaveSelect = true;
									}
									else if (isLikeDesktopFlag )
									{
										CString sTmpNewFileFullPath;
										if (CheckPublicDesktopFile(strDirectory, strItemText, sTmpNewFileFullPath))
										{
											pThis->m_SelectedItemList.Add(sTmpNewFileFullPath);
											strItemText.Empty();
											bHaveSelect = true;
										}
									}
								}
							}
						}
						else
						{
							if (fileslist.GetLength() > 0)
							{
								if (FilePathExists(strDirectory + fileslist))
								{
									pThis->m_SelectedItemList.Add(strDirectory+fileslist);
									bHaveSelect = true;								
								}
								else if (isLikeDesktopFlag)
								{
									CString sTmpNewFileFullPath;
									if (CheckPublicDesktopFile(strDirectory, fileslist, sTmpNewFileFullPath))
									{
										pThis->m_SelectedItemList.Add(sTmpNewFileFullPath);
										bHaveSelect = true;									
									}
								}
							}
						}
					}
					if (bHaveSelect)
						pDlg->EndDialog(IDOK);
					else
					{
						pThis->m_bSelectVirtualItem = true;
						pDlg->EndDialog(IDCANCEL);
					}
					return NULL;
				} 
			}
		}
		else if (HIWORD(wParam) == CBN_EDITCHANGE)
		{
			if (LOWORD(wParam) == cmb13)
			{
				if (pThis)
					pThis->m_bCmb13Changed = true;
			}
		}
	}
	return CallWindowProc(m_wndProc, hwnd, message, wParam, lParam);
}

#pragma warning( pop )
