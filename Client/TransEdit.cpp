// TransEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "TransEdit.h"
#include "typedef.h"

// CTransEdit

IMPLEMENT_DYNAMIC(CTransEdit, CEdit)

BEGIN_MESSAGE_MAP(CTransEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_LBUTTONDOWN()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// CTransEdit 消息处理程序

CTransEdit::CTransEdit() : m_bShowPlaceHolder(FALSE), m_bPassword(FALSE), m_bErr(FALSE)
{
	m_textColor = RGB(0, 0, 0);
	m_backColor = TRANS_BACK;
}

CTransEdit::~CTransEdit()
{

}

HBRUSH CTransEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	m_brush.DeleteObject();

	if (m_backColor == TRANS_BACK)
	{
		m_brush.CreateStockObject(HOLLOW_BRUSH);
		pDC->SetBkMode(TRANSPARENT);
	}
	else
	{
		m_brush.CreateSolidBrush(m_backColor);
		pDC->SetBkColor(m_backColor);
	
	}
	if (m_bShowPlaceHolder)
	{
		pDC->SetTextColor(m_placeHolderColor);
	}
	else
	{
		pDC->SetTextColor(m_textColor);
	}

	return (HBRUSH)m_brush;
}

void CTransEdit::OnUpdate()
{
	UpdateCtrl();
}

void CTransEdit::UpdateCtrl()
{
	CWnd* pParent = GetParent();
	CRect rect;
	GetWindowRect(rect);
	pParent->ScreenToClient(rect);
	rect.DeflateRect(2, 2);
	pParent->InvalidateRect(rect, FALSE);
}

void CTransEdit::OnKillfocus()
{
	if (GetTextLength() == 0)
	{
		ShowPlaceHolder(TRUE);
	}
	UpdateCtrl();
}

void CTransEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	UpdateCtrl();
	if (m_bErr)
	{
		//SetWindowText(_T(""));
		::PostMessage(GetParent()->GetSafeHwnd(), WM_RESTORE_STYLE, 0, 0);
		m_bErr = FALSE;
	}
	__super::OnLButtonDown(nFlags, point);
}


int CTransEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_font.CreateFont(23, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, _T("微软雅黑"));
	SetFont(&m_font);

	if (m_placeHolder.GetLength())
	{
		m_bShowPlaceHolder = TRUE;
		SetWindowText((LPCTSTR)m_placeHolder);
	}
	return 0;
}


void CTransEdit::OnSetFocus(CWnd* pOldWnd)
{
	if (m_bShowPlaceHolder)
	{
		ShowPlaceHolder(FALSE);
	}

	CEdit::OnSetFocus(pOldWnd);

	// TODO:  在此处添加消息处理程序代码
}


BOOL CTransEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEWHEEL)
	{
		short zDelta = (short)HIWORD(pMsg->wParam);
		short xPos = (short)LOWORD(pMsg->lParam);
		short yPos = (short)HIWORD(pMsg->lParam);

		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);

		if (GetSafeHwnd())
		{
			CRect rectControl;
			GetWindowRect(&rectControl);
			ScreenToClient(&rectControl);
			if (rectControl.PtInRect(pt))
			{
				int lineIndex = LineFromChar();
				if (zDelta < 0)
				{
					lineIndex += 1;
				}
				else
				{
					lineIndex -= 1;
				}
				int charindex = LineIndex(lineIndex);
				SetSel(charindex, charindex);
			}
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN && IsKeyPressed(VK_CONTROL) && pMsg->wParam == 'A')
	{
		SetSel(0, -1);
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}
