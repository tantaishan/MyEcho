
#pragma once

#include <dlgs.h> 
#include <vector>
using std::vector;

class CFolderFileSelectDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CFolderFileSelectDialog)

public:
	CFolderFileSelectDialog(BOOL bOpenFileDialog, 
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
						OFN_EXPLORER & (~OFN_SHOWHELP),
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CFolderFileSelectDialog();

	enum { ID_LISTVIEW = lst2 };
	

	//ODM_VIEW_ICONS = 0x7029,

	enum LISTVIEWCMD
	{
		ODM_VIEW_ICONS = 0x7029,
		ODM_VIEW_LIST = 0x702b,
		ODM_VIEW_DETAIL = 0x702c,
		ODM_VIEW_THUMBS = 0x702d,
		ODM_VIEW_TILES = 0x702e,
		ODM_VIEW_LARGE_ICONS = 0x704f,
	};

	void SetSpecialStrings(wchar_t* sItemName, wchar_t* sSelect)
	{
		m_strItemName = CString(sItemName);
		m_strSelect = CString(sSelect);
		
	}
	
protected:
	virtual BOOL OnInitDialog();
	virtual void OnInitDone();
	virtual void OnFolderChange();
	virtual BOOL OnFileNameOK();
	virtual void OnFileNameChange();
	static LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);
	BOOL SetListView(LISTVIEWCMD cmd);
	DECLARE_MESSAGE_MAP()
private:
	CString m_strItemName;
	CString m_strSelect;
	CString m_strError;

	CString m_strLastSelectList;
	bool m_bSelectVirtualItem;
	bool m_bCmb13Changed;

public:
	CString m_strCurrendDirectory;
	CStringArray m_SelectedItemList;	
	static WNDPROC m_wndProc;

	bool IsVirtualItemSelected()
	{
		return m_bSelectVirtualItem;
	}

	void SetErrorInfo(wchar_t* sError)
	{
		m_strError = CString(sError);
	}

	bool HaveSelectSomething()
	{
		return m_SelectedItemList.GetCount() > 0;
	}
};
