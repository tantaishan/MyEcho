//	debugger.cc
//
//

#include "stdafx.h"
#include "debugger.h"
#include "log/logging.h"
//#include "base/threading/platform_thread.h"

namespace base {
namespace debug {

	static bool is_debug_ui_suppressed = false;

	bool WaitForDebugger(int wait_seconds, bool silent) {
#if defined(OS_ANDROID)
		// The pid from which we know which process to attach to are not output by
		// android ddms, so we have to print it out explicitly.
		DLOG(INFO) << "DebugUtil::WaitForDebugger(pid=" << static_cast<int>(getpid())
			<< ")";
#endif
		for (int i = 0; i < wait_seconds * 10; ++i) {
			if (BeingDebugged()) {
				if (!silent)
					BreakDebugger();
				return true;
			}
			//TODO
#if 0
			PlatformThread::Sleep(TimeDelta::FromMilliseconds(100));
#else
			::Sleep(100);
#endif
		}
		return false;
	}

	void SetSuppressDebugUI(bool suppress) {
		is_debug_ui_suppressed = suppress;
	}

	bool IsDebugUISuppressed() {
		return is_debug_ui_suppressed;
	}

}  // namespace debug
}  // namespace base

