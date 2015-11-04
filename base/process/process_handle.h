//	process_handle.h
//
//

#ifndef __BASE_PROCESS_PROCESS_HANDLE_H__
#define __BASE_PROCESS_PROCESS_HANDLE_H__


#include "..\base_types.h"

namespace base
{
	// ProcessHandle is a platform specific type which represents the underlying OS
	// handle to a process.
	// ProcessId is a number which identifies the process in the OS.
#if defined(OS_WIN)
	typedef HANDLE ProcessHandle;
	typedef DWORD ProcessId;
	typedef HANDLE UserTokenHandle;
	const ProcessHandle kNullProcessHandle = NULL;
	const ProcessId kNullProcessId = 0;
#elif defined(OS_POSIX)
	// On POSIX, our ProcessHandle will just be the PID.
	typedef pid_t ProcessHandle;
	typedef pid_t ProcessId;
	const ProcessHandle kNullProcessHandle = 0;
	const ProcessId kNullProcessId = 0;
#endif  // defined(OS_WIN)


void CloseProcessHandle(ProcessHandle process);





} // namespace base.

#endif // __BASE_PROCESS_PROCESS_HANDLE_H__

