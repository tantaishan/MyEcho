#pragma once
#include <Windows.h>
#include <tchar.h>

class OutputFuncScope
{
public:
	explicit OutputFuncScope(LPCTSTR lpszFunc);
	~OutputFuncScope();
private:
	LPCTSTR lpszFunc_;
};

//////////////////////////////////////////////////////////////////////////
class ConsoleOutput
{
public:
	static ConsoleOutput* Instance();
	explicit ConsoleOutput(LPCTSTR lpszWndTile = NULL);
	BOOL Output(LPCTSTR lpszFmt, ...);
	BOOL OutputLn(LPCTSTR lpszFmt, ...);
	BOOL ShowWindow(BOOL bShow);
	BOOL SetWindowTile(LPCTSTR lpszWndTitle);
	~ConsoleOutput(void);

private:
	static ConsoleOutput* instance;
	class Garbo {
	public:
		~Garbo() {
			if (ConsoleOutput::instance)
				delete ConsoleOutput::instance;
		}
	};
	static Garbo garbo;
};
