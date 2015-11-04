//	process_handle_win.cc
//
//

#include "stdafx.h"
#include "process_handle.h"

namespace base
{
	void CloseProcessHandle(ProcessHandle process)
	{
		CloseHandle(process);
	}

} // namespace base.

