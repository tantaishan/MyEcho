//	base_switches.cc
//
//

#include "stdafx.h"
#include "base_switches.h"

namespace switches
{
	// Will add kDebugOnStart to every child processes. If a value is passed, it
	// will be used as a filter to determine if the child process should have the
	// kDebugOnStart flag passed on or not.
	const char kDebugChildren[]			= "debug-children";
	const char kDebugOnStart[]			= "debug-on-start";

	// Will wait for 60 seconds for a debugger to come to attach to the process.
	const char kWaitForDebugger[]		= "wait-for-debugger";

	const char kAutorun[]				= "background";

	const char kUacLevel[]				= "uaclevel";
	const char kRequireAdministrator[]	= "requireadministrator";

} // switches.

