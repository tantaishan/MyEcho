//	module.h
//
//

#ifndef __BASE_WIN_MODULE_H__
#define __BASE_WIN_MODULE_H__

#include <windows.h>

namespace base
{
namespace win
{

	HMODULE GetModuleFromAddress(void* address);

	HMODULE GetSelfModuleHandle(void);

} // namespace win.
} // namespace base.

#endif // __BASE_WIN_MODULE_H__

