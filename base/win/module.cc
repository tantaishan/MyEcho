//	module.cc
//
//

#include "stdafx.h"
#include "module.h"

#include <intrin.h>
#pragma intrinsic(_ReturnAddress)

namespace base
{
namespace win
{
	HMODULE GetModuleFromAddress(void* address)
	{
		HMODULE instance = NULL;
		if (!::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			static_cast<wchar_t*>(address),
			&instance)) {
				DBG(ERROR) << "GetModuleHandleExW error!";
		}
		return instance;
	}

	//	得到EXE所在路径
	HMODULE GetSelfModuleHandle(void)
	{
		void *pCallerAddress = _ReturnAddress();
		return GetModuleFromAddress(pCallerAddress);
	}

} // namespace win.
} // namespace base.

