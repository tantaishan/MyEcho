//+-------------------------------------------------------------------------
//
//  dulib - DirectUI Library
//  Copyright (C) BodSoft
//
//  File:       duEdit.h
//
//  History:    Nov-23-2009   Steven Li  Created
//
//--------------------------------------------------------------------------
#pragma once
#include "duUtility.h"
#include "duPlugin.h"
#include "duTypes.h"
#include <imm.h>

#define BUFLIMIT_INITIAL    30000
#define BUFLIMIT_SINGLE		32766
#define GROWLENGTH			32
#define ROUND_TO_GROW(size)	(((size) + (GROWLENGTH - 1)) & ~(GROWLENGTH - 1))
#define HSCROLL_FRACTION	3

#define EF_MODIFIED			0x0001
#define EF_FOCUSED			0x0002
#define EF_UPDATE			0x0004
#define EF_VSCROLL_TRACK	0x0008
#define EF_HSCROLL_TRACK	0x0010

#define EF_USE_SOFTBRK		0x0100
#define EF_APP_HAS_HANDLE   0x0200
#define EF_AFTER_WRAP		0x0080	/* the caret is displayed after the last character of a
					   wrapped line, instead of in front of the next character */
#define EF_USE_SOFTBRK		0x0100	/* Enable soft breaks in text. */
#define EF_DIALOGMODE       0x0400

#define BUFLIMIT_MULTI		65534	/* maximum buffer size (not including '\0')
					   FIXME: BTW, new specs say 65535 (do you dare ???) */

typedef enum
{
	END_0 = 0,
	END_WRAP,
	END_HARD,
    END_SOFT,
    END_RICH
} LINE_END;

typedef struct tagLINEDEF {
	INT length;
	INT net_length;
	LINE_END ending;
	INT width;
	INT index;
	struct tagLINEDEF *next;
} LINEDEF;

#define SWAP_UINT32(x,y) do { UINT temp = (UINT)(x); (x) = (UINT)(y); (y) = temp; } while(0)
#define ORDER_UINT(x,y) do { if ((UINT)(y) < (UINT)(x)) SWAP_UINT32((x),(y)); } while(0)

/* used for disabled or read-only edit control */
//#define EDIT_NOTIFY_PARENT(es, wNotifyCode) 

static const WCHAR empty_stringW[] = {0};


// duEdit
// 
// 
class duEdit : public duPlugin
{
public:
	virtual void RegisterControlProperty();
	virtual void OnCreate();
	virtual void DrawObject(HDC hDC);
	virtual LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKillFocus(duPlugin *pNewFocus);
	virtual BOOL OnSetFocus(duPlugin *pOldFocus);
	virtual void OnMouseMove(POINT pt);
	virtual void OnMouseDbClick(POINT pt);
	virtual void OnMouseLDown(POINT pt);
	virtual void OnMouseLUp(POINT pt);
	virtual void OnMouseRDown(POINT pt);
	virtual void OnMouseRUp(POINT pt);
	virtual void OnTimer(UINT nEventId);
	virtual BOOL OnSetCursor();
	virtual void Resize(LPRECT lpRect/*=NULL*/);

	virtual void SetText(LPCTSTR lpszText);
	virtual LPCTSTR GetText() { return m_lpszText; }
	
	virtual int CharFromPos(int x, int y);
	virtual UINT GetLimitText() { return m_uBufferLimit; }
	virtual TCHAR GetPasswordChar() { return m_chPassword; }
	virtual DWORD GetSel();
	virtual void PosFromChar(int nIndex, LPPOINT lppt);
	virtual void SetLimitText(UINT uMax);
	virtual void ReplaceSel(LPCTSTR lpszNewText, BOOL fCanUndo);
	virtual BOOL IsReadOnly() { return m_fReadOnly; }
	virtual BOOL IsPassword() { return m_fPassword; }
	virtual BOOL IsNumber() { return m_fNumber; }
	virtual void SetSel(int nStartChar, int nEndChar);
	virtual void OnPaste();
	virtual void OnCopy();
	virtual void OnClear();
	virtual void OnCut();
	virtual void SetReadOnly(BOOL fReadOnly);
	virtual void SetPassword(BOOL fPassword);
	virtual void SetNumber(BOOL fNumber);
	virtual LPCTSTR GetFont(){return m_szFont;}
	virtual void SetFont(LPCTSTR lpszFont);
	virtual COLORREF GetTextColor(){return m_clrText;}
	virtual void SetTextColor(COLORREF clrText);
	virtual COLORREF GetSelectTextColor(){return m_clrSelectText;}
	virtual void SetSelectTextColor(COLORREF clrSelectText);
	virtual COLORREF GetSelectColor(){return m_clrSelect;}
	virtual void SetSelectColor(COLORREF clrSelect);
	virtual int GetLeftBorder(){return m_nLeftBorder;}
	virtual void SetLeftBorder(int nLeftBorder);
	virtual int GetTopBorder(){return m_nTopBorder;}
	virtual void SetTopBorder(int nTopBorder);
	virtual int GetRightBorder(){return m_nRightBorder;}
	virtual void SetRightBorder(int nRightBorder);
	virtual int GetBottomBorder(){return m_nBottomBorder;}
	virtual void SetBottomBorder(int nBottomBorder);

	virtual void FinalRelease() { delete this; }
	virtual LPCTSTR GetTypeInfoName() { return _T("duEdit"); }	
	virtual void EMSetSel(UINT start, UINT end);

public:
	BOOL OnEMCanUndo();
	void OnEMEmptyUndoBuffer();
	INT CallWordBreakProc(INT start, INT index, INT count, INT action);
	UINT GetEditTextLength();
	LPWSTR GetPasswordPointerSL();
	void CalcLineWidthSL();
	void ConfinePoint(LPINT x, LPINT y);
	LRESULT EMPosFromChar(INT index);
	void GetLineRect(INT line, INT scol, INT ecol, LPRECT rc);
	void TextBufferChanged();
	void LockBuffer();
	void UnlockBuffer(BOOL force);
	BOOL MakeFit(UINT size);
	BOOL MakeUndoFit(UINT size);

	void SLInvalidateText(INT start, INT end);
	void InvalidateText(INT start, INT end);
	//void EMSetSel(UINT start, UINT end);
	
	void SetEditCaretPos(INT pos);
	void EMScrollCaret();
	void MoveBackward(BOOL extend);
	void MoveEnd(BOOL extend, BOOL ctrl);
	void MoveForward(BOOL extend);
	void MoveHome(BOOL extend, BOOL ctrl);
	void MoveWordBackward(BOOL extend);
	void MoveWordForward(BOOL extend);
	INT PaintText(HDC dc, INT x, INT y, INT line, INT col, INT count, BOOL rev);
	void AdjustFormatRect();
	void SetRectNP(const RECT *rc);
	LRESULT OnEmCharFromPos(INT x, INT y);
	
	void OnEmReplaceSel(BOOL can_undo, LPCWSTR lpsz_replace, BOOL send_update, BOOL honor_limit);
	int CalcMinSetMarginSize(HDC dc, INT left, INT right);
	void EMSetMargins(INT action, WORD left, WORD right, BOOL repaint);
	BOOL OnEmUndo();
	LRESULT OnChar(WCHAR c);
	void OnContextMenu(INT x, INT y);
	INT OnGetText(INT count, LPWSTR dst, BOOL unicode);
	LRESULT OnKeyDown(INT key);

	LRESULT OnLButtonDown(INT x, INT y);
	LRESULT OnMouseMove(INT x, INT y);

	void OnSetFont(HFONT font, BOOL redraw);
	LRESULT OnSysKeyDown(INT key, DWORD key_data);
	void GetCompositionStr(HIMC hIMC, LPARAM CompFlag);

	//��������m_hFont
	void ResetHFont();
	
	LRESULT EditWndProc_common( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL unicode);

protected:
	void Init(BOOL fUnicode);
	void Destroy();
	void _GetClientRect(HWND hWnd, LPRECT lpRect);
	void _TextOutW(HDC hDC, HFONT hFont, int nXStart, int nYStart, LPCWSTR lpString, int cbString, BOOL fSelect);
	void _RedrawEdit();

public:
	duEdit();
	~duEdit();

public:
	TCHAR m_szFont[MAX_NAME];
	COLORREF m_clrText;
	COLORREF m_clrSelectText;
	COLORREF m_clrSelect;
	int m_nLeftBorder;
	int m_nTopBorder;
	int m_nRightBorder;
	int m_nBottomBorder;

protected:
	int m_lTimerId;
	BOOL m_fMouseIn;
	BOOL m_fRedraw;
	
protected:
	BOOL m_fUnicode;
	LPWSTR m_lpszText;
    UINT m_uTextLength;
	UINT m_uBufferSize;
	UINT m_uBufferLimit;
	HFONT m_hFont;
	INT m_nXoffset;
	INT m_nLineHeight;
	INT m_nCharWidth;
	DWORD m_dwStyle;
	WORD m_wflags;
	INT m_nUndoInsertCount;
	UINT m_uUndoPosition;
	LPWSTR m_lpszUndoText;
	UINT m_uUndoBufferSize;
	INT m_nSelectionStart;
	INT m_nSelectionEnd;
	WCHAR m_chPassword;
	INT m_nLeftMargin;
	INT m_nRightMargin;
	RECT m_rcFormat;
	INT m_nTextWidth;
	INT m_nRegionPosX;
	INT m_nRegionPosY;
	void *m_pWordBreakProc;
	INT m_nLineCount;
	INT m_nYOffset;
	BOOL m_fCaptureState;
	BOOL m_fEnableState;
	/*
	 *	only for multi line controls
	 */
	INT m_nLockCount;
	INT m_nTabsCount;
	LPINT m_pTabs;
	LINEDEF *m_pFirstLineDef;
	HLOCAL m_hLoc32W;
	HLOCAL m_hLoc32A;

	/*
	 * IME Data
	 */
	UINT m_nCompositionLen;
	int m_nCompositionStart;

	BOOL m_fShowCaret;
	BOOL m_fReadOnly;
	BOOL m_fPassword;
	BOOL m_fNumber;
};
