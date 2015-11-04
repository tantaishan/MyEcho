//	launch.h
//
//

#ifndef __BASE_PROCESS_LAUNCH_H__
#define __BASE_PROCESS_LAUNCH_H__


#include "..\base_types.h"
#include "process_handle.h"
#include <string>

namespace base
{
	class CommandLine;

	struct LaunchOptions
	{
		LaunchOptions();
		~LaunchOptions();

		// If true, wait for the process to complete.
		bool wait;

#if defined(OS_WIN)
		bool start_hidden;

		// If true, the new process inherits handles from the parent. In production
		// code this flag should be used only when running short-lived, trusted
		// binaries, because open handles from other libraries and subsystems will
		// leak to the child process, causing errors such as open socket hangs.
		bool inherit_handles;

		// If non-NULL, runs as if the user represented by the token had launched it.
		// Whether the application is visible on the interactive desktop depends on
		// the token belonging to an interactive logon session.
		//
		// To avoid hard to diagnose problems, when specified this loads the
		// environment variables associated with the user and if this operation fails
		// the entire call fails as well.
		UserTokenHandle as_user;

		// If true, use an empty string for the desktop name.
		bool empty_desktop_name;

		// If non-NULL, launches the application in that job object. The process will
		// be terminated immediately and LaunchProcess() will fail if assignment to
		// the job object fails.
		HANDLE job_handle;

		// Handles for the redirection of stdin, stdout and stderr. The handles must
		// be inheritable. Caller should either set all three of them or none (i.e.
		// there is no way to redirect stderr without redirecting stdin). The
		// |inherit_handles| flag must be set to true when redirecting stdio stream.
		HANDLE stdin_handle;
		HANDLE stdout_handle;
		HANDLE stderr_handle;

		// If set to true, ensures that the child process is launched with the
		// CREATE_BREAKAWAY_FROM_JOB flag which allows it to breakout of the parent
		// job if any.
		bool force_breakaway_from_job_;
#else
#error todo
#endif
	};

	bool LaunchProcess(const CommandLine& cmdline,
		const LaunchOptions& options,
		ProcessHandle* process_handle);

#if defined(OS_WIN)
	bool LaunchProcess(const std::wstring& cmdline,
		const LaunchOptions& options,
		ProcessHandle* process_handle);

	BOOL GetTokenByName(HANDLE &hToken,LPWSTR lpName);

	bool SetJobObjectLimitFlags(HANDLE job_object, DWORD limit_flags);

	// Output multi-process printf, cout, cerr, etc to the cmd.exe console that ran
	// chrome. This is not thread-safe: only call from main thread.
	void RouteStdioToConsole();
#elif defined(OS_POSIX)

#endif

	// Executes the application specified by |cl| and wait for it to exit. Stores
	// the output (stdout) in |output|. Redirects stderr to /dev/null. Returns true
	// on success (application launched and exited cleanly, with exit code
	// indicating success).
	bool GetAppOutput(const CommandLine& cl, std::string* output);

	// If supported on the platform, and the user has sufficent rights, increase
	// the current process's scheduling priority to a high priority.
	void RaiseProcessToHighPriority();



} // namespace base.

#endif // __BASE_PROCESS_LAUNCH_H__

