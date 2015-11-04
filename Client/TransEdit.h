#pragma once


// CTransEdit
#define TRANS_BACK -1
#define PASSWORD_CHAR _T('¡ñ')

class CTransEdit : public CEdit
{
	DECLARE_DYNAMIC(CTransEdit)

public:
	CTransEdit();
	virtual ~CTransEdit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	
	void SetTextColor(COLORREF col)
	{
		m_textColor = col;
		UpdateCtrl();
	}
	void SetBackColor(COLORREF col)
	{
		m_backColor = col;
		UpdateCtrl();
	}
	void SetPlaceHolderColor(COLORREF col)
	{
		m_placeHolderColor = col;
	}
	void SetPlaceHolder(const CString& str)
	{
		m_placeHolder = str;
	}
	void ShowPlaceHolder(BOOL bShow)
	{
		if (bShow)
		{
			m_bShowPlaceHolder = TRUE;
			if (IsPassword())
			{
				SetPasswordChar(NULL);
			}
			SetWindowText(m_placeHolder);
		}
		else
		{
			m_bShowPlaceHolder = FALSE;
			if (IsPassword())
			{
				SetPasswordChar(PASSWORD_CHAR);
			}
			SetWindowText(_T(""));
		}
	}

	int GetTextLength() const
	{
		if (m_bShowPlaceHolder)
			return 0;
		return __super::GetWindowTextLength();
	}

	void SetPassword(BOOL bPassword)
	{
		m_bPassword = bPassword;
	}

	BOOL IsPassword()
	{
		return m_bPassword;
	}

	void SetError(BOOL bErr)
	{
		m_bErr = bErr;
	}

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillfocus();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void UpdateCtrl();

private:
	COLORREF	m_textColor;
	COLORREF	m_backColor;
	COLORREF	m_placeHolderColor;
	CBrush		m_brush;
	CFont		m_font;
	BOOL		m_bShowPlaceHolder;
	CString		m_placeHolder;
	BOOL		m_bPassword;
	BOOL		m_bErr;
};


