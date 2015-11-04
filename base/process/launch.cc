//	launch.cc
//
//

#include "stdafx.h"
#include "launch.h"

namespace base
{

	LaunchOptions::LaunchOptions()
		: wait(false),
#if defined(OS_WIN)
		start_hidden(false),
		inherit_handles(false),
		as_user(NULL),
		empty_desktop_name(false),
		job_handle(NULL),
		stdin_handle(NULL),
		stdout_handle(NULL),
		stderr_handle(NULL),
		force_breakaway_from_job_(false)
#else
#endif  // !defined(OS_WIN)
	{
	}

	LaunchOptions::~LaunchOptions()
	{
	}

}  // namespace base

