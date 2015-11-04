// Copyright (c) Shenzhen City Ohyo Network Technology Co., Ltd. All rights reserved.
// lock.cpp
//
//

#include "stdafx.h"
#include "lock.h"

namespace base {
namespace platform {

#if defined(OS_WIN) 

	//	线程锁实现	ThreadLockImpl
	ThreadLockImpl::ThreadLockImpl(const wchar_t * /* place_holder */)
	{
		::InitializeCriticalSection(&_cs);
	}
	ThreadLockImpl::~ThreadLockImpl()
	{
		::DeleteCriticalSection(&_cs);
	}
	BOOL ThreadLockImpl::TryLockup()
	{
		return ::TryEnterCriticalSection(&_cs);
	}
	void ThreadLockImpl::Lockup()
	{
		::EnterCriticalSection(&_cs);
	}
	void ThreadLockImpl::Unlock()
	{
		::LeaveCriticalSection(&_cs);
	}

	//	进程锁实现	ProcessLockImpl
	ProcessLockImpl::ProcessLockImpl(const wchar_t *pwszName)
	{
		_hHandle = ::CreateMutexW(NULL, FALSE, pwszName);
		//_hHandle = ::CreateEventW(NULL, FALSE, TRUE, pwszName);
	}
	ProcessLockImpl::~ProcessLockImpl()
	{
		if(_hHandle)
		{
			::CloseHandle(_hHandle);
			_hHandle = NULL;
		}
	}

	BOOL ProcessLockImpl::TryLockup()
	{
		return (WAIT_OBJECT_0 == ::WaitForSingleObject(_hHandle, 0));
	}
	void ProcessLockImpl::Lockup()
	{
		::WaitForSingleObject(_hHandle, INFINITE);
	}
	void ProcessLockImpl::Unlock()
	{
		::ReleaseMutex(_hHandle);
		//::SetEvent(_hHandle);
	}

#elif defined(OS_LINUX)
#elif defined(OS_FREEBSD)
#else
#error Need a platform defined before build this file.
#endif

} // namespace platform
} // namespace base
