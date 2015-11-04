//	wrapped_window_proc.h
//	2012/08/11
//

#ifndef __WRAPPED_WINDOW_PROC__
#define __WRAPPED_WINDOW_PROC__
#pragma once

#include <windows.h>



namespace base
{
namespace win
{

	typedef int (__cdecl *WinProcExceptionFilter)(EXCEPTION_POINTERS* info);

	WinProcExceptionFilter SetWinProcExceptionFilter(WinProcExceptionFilter filter);

	int CallExceptionFilter(EXCEPTION_POINTERS* info);



	template <WNDPROC proc>
	LRESULT CALLBACK WrappedWindowProc(HWND hwnd, UINT message,
		WPARAM wparam, LPARAM lparam)
	{
			LRESULT rv = 0;
			__try
			{
				rv = proc(hwnd, message, wparam, lparam);
			}
			__except(CallExceptionFilter(GetExceptionInformation()))
			{
			}
			return rv;
	}


	void* _SetWindowUserData(HWND hwnd, void* user_data);
	void* _GetWindowUserData(HWND hwnd);

	template<class WinCls>
	LRESULT CALLBACK WindowProcT(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		WinCls* self = reinterpret_cast<WinCls*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		switch(message)
		{
		case WM_NCCREATE:
			{
				CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lparam);
				self = reinterpret_cast<WinCls*>(cs->lpCreateParams);
				self->m_hwnd = hwnd;

				SetLastError(ERROR_SUCCESS);
				_SetWindowUserData(hwnd, self);
				DBG_IF(ERROR, GetLastError() != ERROR_SUCCESS) << "_SetWindowUserData error!";
			}
			break;
		case WM_NCDESTROY:
			{
				self->m_hwnd = NULL;
				SetLastError(ERROR_SUCCESS);
				_SetWindowUserData(hwnd, NULL);
				DBG_IF(ERROR, GetLastError() != ERROR_SUCCESS) << "_SetWindowUserData error!";
			}
			break;
		}

		if(self)
		{
			__if_exists(WinCls::message_handler_)
			{
				if(self->message_handler_) {
					LRESULT message_result = 0;
					if(self->message_handler_->ProcessMessage(hwnd, message, wparam, lparam, &message_result)){
						return(message_result);
					}
				}
			}
			__if_not_exists(WinCls::message_handler_)
			{
				return self->OnWndProc(message, wparam, lparam);
			}
		}
		return ::DefWindowProc(hwnd, message, wparam, lparam);
	}


} // namespace win
} // namespace base


#endif // __WRAPPED_WINDOW_PROC__

