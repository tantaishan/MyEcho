//	wrapped_window_proc.cc
//	2012/08/11
//

#include "stdafx.h"
#include "wrapped_window_proc.h"


namespace
{

	base::win::WinProcExceptionFilter s_exception_filter = NULL;

}  // namespace.


namespace base
{
namespace win
{

	WinProcExceptionFilter SetWinProcExceptionFilter(WinProcExceptionFilter filter)
	{
		//subtle::AtomicWord rv = subtle::NoBarrier_AtomicExchange(
		//	reinterpret_cast<subtle::AtomicWord*>(&s_exception_filter),
		//	reinterpret_cast<subtle::AtomicWord>(filter));
		//return reinterpret_cast<WinProcExceptionFilter>(rv);

		return reinterpret_cast<WinProcExceptionFilter>(InterlockedExchangePointer((PVOID*)&s_exception_filter, filter));
	}


	int CallExceptionFilter(EXCEPTION_POINTERS* info)
	{
		return s_exception_filter ? s_exception_filter(info) : EXCEPTION_CONTINUE_SEARCH;
	}



	void* _SetWindowUserData(HWND hwnd, void* user_data) {
		return
			reinterpret_cast<void*>(SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(user_data)));
	}

	void* _GetWindowUserData(HWND hwnd) {
		DWORD process_id = 0;
		/*DWORD thread_id =*/ GetWindowThreadProcessId(hwnd, &process_id);
		// A window outside the current process needs to be ignored.
		if (process_id != ::GetCurrentProcessId())
			return NULL;
		return reinterpret_cast<void*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
} // namespace win
} // namespace base

