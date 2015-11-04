/***************************************************
* FILE NAME: execDos.c
*
* PURPOSE:
*   NSIS plugin for DOS (console) applications.
*   Creates child process with redirected I/O.
*   Catches application's output and can put it to the
*   log file. Returns process exit code or error code.
*
* CHANGE HISTORY
*
* $LOG$
*
* Author          Date       Modifications
* Takhir Bedertdinov  May 31 2004  Original
*   --//--        Oct 31 2004  Log output
*   --//--        Feb 10 2005  /TIMEOUT option
*   --//--        Jul 23 2005  /ASYNC option
*   --//--        Jul 25 2005  multithreading
*   --//--        Mar 10 2006  perf. improved
*   --//--        Jul 25 2006  to stack option
*   --//--        Dec 12 2006  detailed option
* Afrow UK        Jul 27 2007  func options
*   --//--                 isdone func
* Takhir Bedertdinov  Jul 28 2007  to window option
*      Moscow, Russia, ineum@narod.ru
**************************************************/

#include <windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include "..\ExDll\exdll.h"
#include "COMMCTRL.H"

enum ERROR_CODES {
  ERR_DUPHANDLE = -16,
  ERR_CREATEPIPE,
  ERR_CREATEPROC,
  ERR_CLOSEHANDLE,
  ERR_WRITEPIPE,
  ERR_GETEXITCODE,
  ERR_LOGOPEN,
  ERR_LOGWRITE,
  ERR_TERMINATED,
  ERR_CREATETHREAD
};

enum OUT_OPTIONS {
  OUT_FILE,
  OUT_STACK,
  OUT_WINDOW,
  OUT_FUNCTION
};

#define SLEEP_MS 10
#define T_O      "/TIMEOUT"
#define N_W      "/ASYNC"
#define TOSTACK  "/TOSTACK"
#define DETAILED  "/DETAILED"
#define TOWINDOW  "/TOWINDOW"
#define TOFUNC    "/TOFUNC"
#define ENDFUNC  "/ENDFUNC"

typedef struct _threadParams {
  char *dosExec;
  char *pipeWrite;
  DWORD target;
  char *logFile;
  DWORD timeout;
  HWND hOut;
  LONG lFuncAddress;
  LONG lEndFuncAddress;
} threadParams, *pthreadParams;

extra_parameters* ep;

/*****************************************************
 * FUNCTION NAME: my_strchr() my_atoui
 * PURPOSE: 
 *   libc/msvcrt replacements
 *****************************************************/
char *my_strchr(char *s, char c)
{
  while(*s != 0)
  {
    if(*s == c)
      return s;
    s++;
  }
  return NULL;
}

DWORD my_atoui(char *s)
{
  unsigned int v=0;
  char m=10;
  char t='9';

  if (*s == '0')
  {
   s++; // skip over 0
   if (s[0] >= '0' && s[0] <= '7')
   {
    m=8; // octal
    t='7';
   }
   if ((s[0] & ~0x20) == 'X')
   {
    m=16; // hex
    s++; // advance over 'x'
   }
  }

  for (;;)
  {
   int c=*s++;
   if (c >= '0' && c <= t) c-='0';
   else if (m==16 && (c & ~0x20) >= 'A' && (c & ~0x20) <= 'F') c = (c & 7) + 9;
   else break;
   v*=m;
   v+=c;
  }
  return v;
}

/*****************************************************
* FUNCTION NAME: redirect()
* PURPOSE: 
*   Creates child process with redirected IO
* SPECIAL CONSIDERATIONS:
*
*****************************************************/
DWORD __stdcall redirect(void *pp)
{
  HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
  HANDLE hInputWriteTmp,hInputRead,hInputWrite;
  HANDLE hErrorWrite;
  PROCESS_INFORMATION pi;
  STARTUPINFO *psi = GlobalAlloc(GPTR, sizeof(STARTUPINFO));
  SECURITY_ATTRIBUTES sa;
  HANDLE f = INVALID_HANDLE_VALUE;
  DWORD rslt,
    waitTime = 0,
    dwRead = 0;
  char b[256];
  char *szStack = NULL;
  unsigned int iStack = 0;
  pthreadParams ptp = (pthreadParams)pp;
  LVITEM lvItem = {LVIF_TEXT, 0};
  
  /* creates child process with redirected IO */
  sa.nLength= sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;
  if(ptp->target != OUT_FILE)
  {
    szStack = GlobalAlloc(GPTR, g_stringsize + 2);
    lvItem.pszText = szStack;
  }
  
  while(1)
  {
    /* creating pipes for child process with redirected IO */
    if(!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0) ||
      !CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0))
    { rslt = ERR_CREATEPIPE; break; }
    if(!DuplicateHandle(GetCurrentProcess(), hOutputWrite, GetCurrentProcess(),
      
      /* duplicates handles and makes them inheritable */
      &hErrorWrite, 0, TRUE, DUPLICATE_SAME_ACCESS) ||
      !DuplicateHandle(GetCurrentProcess(), hOutputReadTmp, GetCurrentProcess(),
      &hOutputRead, 0, FALSE, DUPLICATE_SAME_ACCESS) ||
      !DuplicateHandle(GetCurrentProcess(), hInputWriteTmp, GetCurrentProcess(),
      &hInputWrite, 0, FALSE, DUPLICATE_SAME_ACCESS))
    { rslt = ERR_DUPHANDLE; break; }
    
    
    /* run process and close unnecessary handles */
    psi->cb = sizeof(STARTUPINFO);
    psi->dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    psi->hStdOutput = hOutputWrite;
    psi->hStdInput  = hInputRead;
    psi->hStdError  = hErrorWrite;
    psi->wShowWindow = SW_HIDE; /* SW_NORMAL - for tests */
    if(!CreateProcess(NULL, ptp->dosExec, NULL, NULL, TRUE, 0, NULL, NULL, psi, &pi))
    { rslt = ERR_CREATEPROC; break; }
    
    /* closes temporary and unnesessery handles */
    if(!CloseHandle(hOutputReadTmp) ||
      !CloseHandle(hInputWriteTmp) ||
      !CloseHandle(pi.hThread) ||
      !CloseHandle(hOutputWrite) ||
      !CloseHandle(hInputRead ) ||
      !CloseHandle(hErrorWrite))
    { rslt = ERR_CLOSEHANDLE; break; }
    
    /* write all to pipe - on tests it caches input strings correctly */
    if(lstrlen(ptp->pipeWrite) > 0)
    {
      if(WriteFile(hInputWrite, ptp->pipeWrite, lstrlen(ptp->pipeWrite), &rslt, NULL) == 0 ||
        rslt != (unsigned)lstrlen(ptp->pipeWrite))
      { rslt = ERR_WRITEPIPE; break; }
      FlushFileBuffers(hInputWrite);
    }
    
    /* open log file if name is correct */
    if(ptp->target == OUT_FILE &&
      *(ptp->logFile) != 0 &&
      (f = CreateFile(ptp->logFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
    { rslt = ERR_LOGOPEN; break; }
    
    /* OK, this is main execution loop */
    while(waitTime <= ptp->timeout &&
      rslt != ERR_LOGWRITE &&
      PeekNamedPipe(hOutputRead, 0, 0, 0, &dwRead, NULL) &&
      GetExitCodeProcess(pi.hProcess, &rslt))
    {
      if(dwRead > 0)
      {
        if(ReadFile(hOutputRead, b, dwRead > sizeof(b) ? sizeof(b) : dwRead, &rslt, NULL) &&
          rslt > 0)
        {
          if(ptp->target == OUT_FILE)
          {
            if(f != INVALID_HANDLE_VALUE &&
              (WriteFile(f, b, rslt, &dwRead, NULL) == 0 || dwRead != rslt))
            {
              rslt = ERR_LOGWRITE;
              break;
            }
          }
          else
          {
            dwRead = 0;
            while(dwRead < rslt)
            {
              if(b[dwRead] == '\n')
              {
                /* supress empty lines */
                if(iStack > 0)
                {
                  szStack[iStack] = 0;
                  if(ptp->target == OUT_STACK)
                    pushstring(szStack);
                  else if(ptp->target == OUT_FUNCTION)
                  {
                    pushstring(szStack);
                    ep->ExecuteCodeSegment(ptp->lFuncAddress-1, 0);
                  }
                  else if(lstrcmpi(ptp->logFile, "SysListView32") == 0)
                  {
                    lvItem.iItem = SendMessage(ptp->hOut, LVM_GETITEMCOUNT, 0, 0);
                    lvItem.iItem = SendMessage(ptp->hOut, LVM_INSERTITEM, 0,  (LPARAM)&lvItem);
                    SendMessage(ptp->hOut, LVM_ENSUREVISIBLE, lvItem.iItem,  0);
                  }
                  else if(lstrcmpi(ptp->logFile, "Edit") == 0 || lstrcmpi(ptp->logFile, "RichEdit20A") == 0)
                  {
                    lstrcat(szStack, "\r\n");
                    lvItem.iItem = SendMessage(ptp->hOut, WM_GETTEXTLENGTH, 0, 0);
                    SendMessage(ptp->hOut, EM_SETSEL, lvItem.iItem, lvItem.iItem);
                    SendMessage(ptp->hOut, EM_REPLACESEL, 0,  (LPARAM)szStack);
                  }
                  else if(lstrcmpi(ptp->logFile, "ListBox") == 0)
                  {
                    lvItem.iItem = SendMessage(ptp->hOut, LB_GETCOUNT, 0, 0);
                    lvItem.iItem = SendMessage(ptp->hOut, LB_INSERTSTRING, lvItem.iItem,  (LPARAM)szStack);
                    //SendMessage(ptp->hOut, LVM_ENSUREVISIBLE, lvItem.iItem,  0);
                  }
                  iStack = 0;
                }
              }
              else if(b[dwRead] != '\r' && iStack < (g_stringsize-1))
              {
                szStack[iStack++] = b[dwRead];
              }
              dwRead++;
            }
          }
        }
        if(f != INVALID_HANDLE_VALUE)
          FlushFileBuffers(f);
      }
      else if(rslt == STILL_ACTIVE)
      {
        Sleep(SLEEP_MS);
        waitTime += SLEEP_MS;
      }
      else break;
    }
    if(iStack > 0)
    {
      szStack[iStack] = 0;
      pushstring(szStack);
    }
    if(f != INVALID_HANDLE_VALUE) CloseHandle(f);
    
    if(GetExitCodeProcess(pi.hProcess, &rslt))
    {
      if(rslt == STILL_ACTIVE)
      {
        TerminateProcess(pi.hProcess, -1);
        rslt = ERR_TERMINATED;
      }
    }
    else rslt = ERR_GETEXITCODE;
    CloseHandle(pi.hProcess);
    CloseHandle(hOutputRead);
    CloseHandle(hInputWrite);
    break;
  }
  
  GlobalFree(psi);
  GlobalFree(ptp->dosExec);
  GlobalFree(ptp->pipeWrite);
  GlobalFree(ptp->logFile);
  GlobalFree(ptp);
  if(szStack) GlobalFree(szStack);

  if (ptp->lEndFuncAddress != -1)
    ep->ExecuteCodeSegment(ptp->lEndFuncAddress - 1, 0);

  return (DWORD)rslt;
}


/*****************************************************
 * FUNCTION NAME: wait()
 * PURPOSE: 
 *   waits for thread exit and closes handle
 * SPECIAL CONSIDERATIONS:
 *   tested with my consApp.exe
 *****************************************************/
void __declspec(dllexport) wait(HWND hwndParent,
                      int string_size,
                      char *variables,
                      stack_t **stacktop)
{
  int rslt = ERR_CREATETHREAD;
  char exitCode[16];
  HANDLE hThread;

  popstring(exitCode);
  hThread = (HANDLE)my_atoui(exitCode);
/* push to stack application' exit code or -1 or "still_running" */
  if(hThread != NULL)
  {
    WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, &rslt);
    CloseHandle(hThread);
    hThread = NULL;
  }
  wsprintf(exitCode,"%d", rslt);
  pushstring(exitCode);

}

/*****************************************************
 * FUNCTION NAME: isdone()
 * PURPOSE: 
 *   checks if the thread has completed
 * SPECIAL CONSIDERATIONS:
 *   
 *****************************************************/
void __declspec(dllexport) isdone(HWND hwndParent,
                      int string_size,
                      char *variables,
                      stack_t **stacktop)
{
  char s[16] = "";
  HANDLE hThread;

// get thread handle from stack
  popstring(s);
  hThread = (HANDLE)my_atoui(s);
// is it running? 1 == yes, 0 == exited, -1 == error 
  if(hThread != NULL)
    pushstring(WaitForSingleObject(hThread, 0) == WAIT_TIMEOUT ? "0" : "1");
  else
    pushstring("-1");
}

/*****************************************************
 * FUNCTION NAME: exec()
 * PURPOSE: 
 *   C dll entry point for hidden DOS process execution
 * SPECIAL CONSIDERATIONS:
 *
 *****************************************************/
void __declspec(dllexport) exec(HWND hwndParent,
                      int string_size,
                      char *variables,
                      stack_t **stacktop,
                      extra_parameters *extra)
{
  DWORD dwThreadId;
  HANDLE hThread;
  BOOL fWait = TRUE;
  pthreadParams ptp = GlobalAlloc(GPTR, sizeof(threadParams));
  char s[16];
  char *p;
  HWND childwnd;

  ep = extra;
  EXDLL_INIT();

  ptp->dosExec = GlobalAlloc(GPTR, string_size);
  ptp->pipeWrite = GlobalAlloc(GPTR, string_size);
  ptp->logFile = GlobalAlloc(GPTR, string_size);
  ptp->timeout = 0xffffff;
  ptp->lEndFuncAddress = -1;

  while(!popstring(ptp->dosExec) && *(ptp->dosExec) == '/')
  {
    if(lstrcmpi(ptp->dosExec, N_W) == 0)
    {
      fWait = FALSE;
    }
    else if(lstrcmpi(ptp->dosExec, TOSTACK) == 0)
    {
      ptp->target = OUT_STACK;
    }
    else if(lstrcmpi(ptp->dosExec, TOWINDOW) == 0)
    {
      ptp->target = OUT_WINDOW;
    }
    else if(lstrcmpi(ptp->dosExec, TOFUNC) == 0)
    {
      ptp->target = OUT_FUNCTION;
    }
    else if(lstrcmpi(ptp->dosExec, DETAILED) == 0)
    {
// convert 'detailed' to 'window' option
      ptp->target = OUT_WINDOW;
      if(hwndParent &&
        (childwnd = FindWindowEx(hwndParent, NULL, "#32770", NULL)) != NULL)
        wsprintf(ptp->logFile, "%d", (int)GetDlgItem(childwnd, 0x3f8));
    }
    else if((p = my_strchr(ptp->dosExec, '=')) != NULL)
    {
      *p++ = 0;
      if(lstrcmpi(ptp->dosExec, T_O) == 0)
        ptp->timeout = my_atoui(p);
      else if(lstrcmpi(ptp->dosExec, ENDFUNC) == 0)
        ptp->lEndFuncAddress = my_atoui(p);
    }
    *(ptp->dosExec) = 0;
  }

// if stack is not empty and no /tostack or /detailed output option
  if(popstring(ptp->pipeWrite) == 0 &&
    *ptp->logFile == 0 && // may be in use by DETAILED window
    ptp->target != OUT_STACK) // 2 param suposed only
    popstring(ptp->logFile);
// output window was kept in string. Let's handle it once
  if(ptp->target == OUT_WINDOW)
  {
    ptp->hOut = (HWND)my_atoui(ptp->logFile);
    GetClassName(ptp->hOut, ptp->logFile, string_size);
  }
  if(ptp->target == OUT_FUNCTION)
  {
    ptp->lFuncAddress = (LONG)my_atoui(ptp->logFile);
  }

  hThread = CreateThread(NULL, 0, redirect, (void*)ptp, 0, &dwThreadId);
  wsprintf(s, "%u", hThread);
  pushstring(s);
  if(fWait) wait(hwndParent, string_size, variables, stacktop);
}

/*****************************************************
 * FUNCTION NAME: DllMain()
 * PURPOSE: 
 *   Dll main entry point
 * SPECIAL CONSIDERATIONS:
 *   
 *****************************************************/
BOOL WINAPI DllMain(HANDLE hInst,
              ULONG ul_reason_for_call,
              LPVOID lpReserved)
{
  return TRUE;
}