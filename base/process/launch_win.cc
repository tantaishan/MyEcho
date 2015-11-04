//	launch_win.cc
//
//

#include "stdafx.h"
#include "launch.h"
#include "kill.h"
#include "scoped_process_information.h"
#include "cmdline/command_line.h"

#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <userenv.h>
#include <psapi.h>
#include <TlHelp32.h>

// userenv.dll is required for CreateEnvironmentBlock().
#pragma comment(lib, "userenv.lib")

#ifndef DCHECK
#	define DCHECK(x)
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

namespace base
{
namespace
{
	// This exit code is used by the Windows task manager when it kills a
	// process.  It's value is obviously not that unique, and it's
	// surprising to me that the task manager uses this value, but it
	// seems to be common practice on Windows to test for it as an
	// indication that the task manager has killed something if the
	// process goes away.
	const DWORD kProcessKilledExitCode = 1;

}  // namespace.

void RouteStdioToConsole()
{
	// Don't change anything if stdout or stderr already point to a
	// valid stream.
	//
	// If we are running under Buildbot or under Cygwin's default
	// terminal (mintty), stderr and stderr will be pipe handles.  In
	// that case, we don't want to open CONOUT$, because its output
	// likely does not go anywhere.
	//
	// We don't use GetStdHandle() to check stdout/stderr here because
	// it can return dangling IDs of handles that were never inherited
	// by this process.  These IDs could have been reused by the time
	// this function is called.  The CRT checks the validity of
	// stdout/stderr on startup (before the handle IDs can be reused).
	// _fileno(stdout) will return -2 (_NO_CONSOLE_FILENO) if stdout was
	// invalid.
	if (_fileno(stdout) >= 0 || _fileno(stderr) >= 0)
		return;

	if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		unsigned int result = GetLastError();
		// Was probably already attached.
		if (result == ERROR_ACCESS_DENIED)
			return;
		// Don't bother creating a new console for each child process if the
		// parent process is invalid (eg: crashed).
		if (result == ERROR_GEN_FAILURE)
			return;
		// Make a new console if attaching to parent fails with any other error.
		// It should be ERROR_INVALID_HANDLE at this point, which means the browser
		// was likely not started from a console.
		AllocConsole();
	}

	// Arbitrary byte count to use when buffering output lines.  More
	// means potential waste, less means more risk of interleaved
	// log-lines in output.
	enum { kOutputBufferSize = 64 * 1024 };

	if (freopen("CONOUT$", "w", stdout)) {
		setvbuf(stdout, NULL, _IOLBF, kOutputBufferSize);
		// Overwrite FD 1 for the benefit of any code that uses this FD
		// directly.  This is safe because the CRT allocates FDs 0, 1 and
		// 2 at startup even if they don't have valid underlying Windows
		// handles.  This means we won't be overwriting an FD created by
		// _open() after startup.
		_dup2(_fileno(stdout), 1);
	}
	if (freopen("CONOUT$", "w", stderr)) {
		setvbuf(stderr, NULL, _IOLBF, kOutputBufferSize);
		_dup2(_fileno(stderr), 2);
	}

	// Fix all cout, wcout, cin, wcin, cerr, wcerr, clog and wclog.
	std::ios::sync_with_stdio();
}


bool LaunchProcess(const std::wstring& cmdline,
				   const LaunchOptions& options,
				   ProcessHandle* process_handle)
{
	STARTUPINFO startup_info = {};
	startup_info.cb = sizeof(startup_info);
	if (options.empty_desktop_name)
		startup_info.lpDesktop = L"";
	startup_info.dwFlags = STARTF_USESHOWWINDOW;
	startup_info.wShowWindow = options.start_hidden ? SW_HIDE : SW_SHOW;

	if (options.stdin_handle || options.stdout_handle || options.stderr_handle) {
		DCHECK(options.inherit_handles);
		DCHECK(options.stdin_handle);
		DCHECK(options.stdout_handle);
		DCHECK(options.stderr_handle);
		startup_info.dwFlags |= STARTF_USESTDHANDLES;
		startup_info.hStdInput = options.stdin_handle;
		startup_info.hStdOutput = options.stdout_handle;
		startup_info.hStdError = options.stderr_handle;
	}

	DWORD flags = 0;

	if (options.job_handle) {
		flags |= CREATE_SUSPENDED;

		// If this code is run under a debugger, the launched process is
		// automatically associated with a job object created by the debugger.
		// The CREATE_BREAKAWAY_FROM_JOB flag is used to prevent this.
		flags |= CREATE_BREAKAWAY_FROM_JOB;
	}

	if (options.force_breakaway_from_job_)
		flags |= CREATE_BREAKAWAY_FROM_JOB;

	base::win::ScopedProcessInformation process_info;

	if (options.as_user) {
		flags |= CREATE_UNICODE_ENVIRONMENT;
		void* enviroment_block = NULL;

		if (!CreateEnvironmentBlock(&enviroment_block, options.as_user, FALSE)) {
			DBG(ERROR) << "CreateEnvironmentBlock";
			return false;
		}

		BOOL launched =
			CreateProcessAsUser(options.as_user, NULL,
			const_cast<wchar_t*>(cmdline.c_str()),
			NULL, NULL, options.inherit_handles, flags,
			enviroment_block, NULL, &startup_info,
			process_info.Receive());
		DestroyEnvironmentBlock(enviroment_block);
		if (!launched) {
			DBG(ERROR) << "CreateProcessAsUser";
			return false;
		}
	} else {
		if (!CreateProcess(NULL,
			const_cast<wchar_t*>(cmdline.c_str()), NULL, NULL,
			options.inherit_handles, flags, NULL, NULL,
			&startup_info, process_info.Receive())) {
				DBG(ERROR) << "CreateProcess";
				return false;
		}
	}

	if (options.job_handle) {
		if (0 == AssignProcessToJobObject(options.job_handle,
			process_info.process_handle())) {
				DBG(ERROR) << "Could not AssignProcessToObject.";
				KillProcess(process_info.process_handle(), kProcessKilledExitCode, true);
				return false;
		}

		ResumeThread(process_info.thread_handle());
	}

	if (options.wait)
		WaitForSingleObject(process_info.process_handle(), INFINITE);

	// If the caller wants the process handle, we won't close it.
	if (process_handle)
		*process_handle = process_info.TakeProcessHandle();

	return true;
}

bool LaunchProcess(const CommandLine& cmdline,
				   const LaunchOptions& options,
				   ProcessHandle* process_handle)
{
	return LaunchProcess(cmdline.GetCommandLineString(), options, process_handle);
}

BOOL GetTokenByName(HANDLE &hToken, LPWSTR lpName)
{
	if(!lpName)
	{
		return FALSE;
	}
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0};

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return (FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32)) 
	{
		do 
		{
			if(!wcscmp(_wcsupr(pe32.szExeFile),_wcsupr(lpName)))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE,pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess,TOKEN_ALL_ACCESS,&hToken);
				CloseHandle (hProcessSnap); 
				return (bRet);
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
		bRet = TRUE; 
	} 
	else 
		bRet = FALSE;

	CloseHandle (hProcessSnap); 
	return (bRet);
}

bool SetJobObjectLimitFlags(HANDLE job_object, DWORD limit_flags)
{
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit_info = {0};
	limit_info.BasicLimitInformation.LimitFlags = limit_flags;
	return 0 != SetInformationJobObject(
		job_object,
		JobObjectExtendedLimitInformation,
		&limit_info,
		sizeof(limit_info));
}

// Output multi-process printf, cout, cerr, etc to the cmd.exe console that ran
// chrome. This is not thread-safe: only call from main thread.
void RouteStdioToConsole();

// Executes the application specified by |cl| and wait for it to exit. Stores
// the output (stdout) in |output|. Redirects stderr to /dev/null. Returns true
// on success (application launched and exited cleanly, with exit code
// indicating success).
bool GetAppOutput(const CommandLine& cl, std::string* output)
{
	return false;
}

// If supported on the platform, and the user has sufficent rights, increase
// the current process's scheduling priority to a high priority.
void RaiseProcessToHighPriority()
{
	::SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}

}  // namespace base

