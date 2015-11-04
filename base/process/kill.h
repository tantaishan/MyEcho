//	kill.h
//

#ifndef __BASE_PROCESS_KILL_H__
#define __BASE_PROCESS_KILL_H__ 


#include "base_types.h"
#include "process/process_handle.h"


namespace base
{
	// Return status values from GetTerminationStatus.  Don't use these as
	// exit code arguments to KillProcess*(), use platform/application
	// specific values instead.
	enum TerminationStatus {
		TERMINATION_STATUS_NORMAL_TERMINATION,   // zero exit status
		TERMINATION_STATUS_ABNORMAL_TERMINATION, // non-zero exit status
		TERMINATION_STATUS_PROCESS_WAS_KILLED,   // e.g. SIGKILL or task manager kill
		TERMINATION_STATUS_PROCESS_CRASHED,      // e.g. Segmentation fault
		TERMINATION_STATUS_STILL_RUNNING,        // child hasn't exited yet
		TERMINATION_STATUS_MAX_ENUM
	};

	bool KillProcess(ProcessHandle process, int exit_code, bool wait);

#if defined(OS_WIN)
	bool KillProcessById(ProcessId process_id,
		int exit_code,
		bool wait);
#endif  // defined(OS_WIN)

	TerminationStatus GetTerminationStatus(ProcessHandle handle,
		int* exit_code);

	bool WaitForExitCode(ProcessHandle handle, int* exit_code);

	bool WaitForExitCodeWithTimeout(ProcessHandle handle,
		int* exit_code,
		uint32 timeout);

	//bool WaitForProcessesToExit(
	//	const FilePath::StringType& executable_name,
	//	uint32 wait,
	//	const ProcessFilter* filter);

	bool WaitForSingleProcess(ProcessHandle handle,
		uint32 wait);

	//void EnsureProcessTerminated(ProcessHandle process_handle);
} // namespace base.

#endif // __BASE_PROCESS_KILL_H__

