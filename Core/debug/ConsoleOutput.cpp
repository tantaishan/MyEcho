#include "ConsoleOutput.h"

OutputFuncScope::OutputFuncScope(LPCTSTR lpszFunc) : lpszFunc_((LPTSTR)lpszFunc)
{
	ConsoleOutput::Instance()->OutputLn(_T(">>%s"), lpszFunc_);
}

OutputFuncScope::~OutputFuncScope()
{
	ConsoleOutput::Instance()->OutputLn(_T("<<%s"), lpszFunc_);
}

//////////////////////////////////////////////////////////////////////////
ConsoleOutput* ConsoleOutput::instance = 0;
ConsoleOutput* ConsoleOutput::Instance()
{
	if (!instance)
	{
		if (!instance)
			instance = new ConsoleOutput;
	}

	return instance;
}

ConsoleOutput::ConsoleOutput(LPCTSTR lpszWndTile)
{
	if (::AllocConsole())
	{
		if (lpszWndTile)
			::SetConsoleTitle(lpszWndTile);
	}
}

BOOL ConsoleOutput::Output(LPCTSTR lpszFmt, ...)
{
	const int kMaxLen = 1024;
	TCHAR szText[kMaxLen] = { 0 };

	va_list arglist;
	va_start(arglist, lpszFmt);
	_vstprintf_s(szText, _countof(szText), lpszFmt, arglist);
	va_end(arglist);

	return ::WriteConsole(::GetStdHandle(STD_OUTPUT_HANDLE), szText, _tcslen(szText), NULL, NULL);
}

BOOL ConsoleOutput::OutputLn(LPCTSTR lpszFmt, ...)
{
	const int kMaxLen = 1024;
	TCHAR szText[kMaxLen] = { 0 };

	va_list arglist;
	va_start(arglist, lpszFmt);
	_vstprintf_s(szText, _countof(szText), lpszFmt, arglist);
	va_end(arglist);
	_tcscat_s(szText, kMaxLen, _T("\n"));

	return ::WriteConsole(::GetStdHandle(STD_OUTPUT_HANDLE), szText, _tcslen(szText), NULL, NULL);
}

BOOL ConsoleOutput::ShowWindow(BOOL bShow)
{
	return ::ShowWindow(::GetConsoleWindow(), bShow ? SW_SHOW : SW_HIDE);
}

BOOL ConsoleOutput::SetWindowTile(LPCTSTR lpszWndTitle)
{
	if (lpszWndTitle)
		return ::SetConsoleTitle(lpszWndTitle);
	else
		return FALSE;
}

ConsoleOutput::~ConsoleOutput(void)
{
	::FreeConsole();
}
