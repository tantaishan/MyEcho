//	lock.h
//
//

#ifndef __LOCK_H__
#define __LOCK_H__

#include "../base_types.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_LINUX)
#elif defined(OS_FREEBSD)
#endif


namespace base
{
	namespace platform
	{
#if defined(OS_WIN)
		class ThreadLockImpl
		{
		public:
			explicit ThreadLockImpl(const wchar_t *place_holder = NULL);
			virtual ~ThreadLockImpl();

			BOOL TryLockup();	// 尝试进入临界区,如果成功则需要调用Unlock,可重入
			void Lockup();
			void Unlock();

		private:
			CRITICAL_SECTION _cs;

			DISALLOW_COPY_AND_ASSIGN(ThreadLockImpl);
		};

		class ProcessLockImpl
		{
		public:
			explicit ProcessLockImpl(const wchar_t *pwszName);
			virtual ~ProcessLockImpl();

			BOOL TryLockup();	// 尝试锁定,如果成功则需要调用Unlock,可重入
			void Lockup();
			void Unlock();

		private:
			HANDLE _hHandle;

			DISALLOW_COPY_AND_ASSIGN(ProcessLockImpl);
		};
#elif defined(OS_LINUX)
#elif defined(OS_FREEBSD)
#endif
	} // namespace platform



	template<class L>
	class Lock
	{
	public:
		explicit Lock(const wchar_t *pwszName_PLOCK_ONLY = NULL):_lock(pwszName_PLOCK_ONLY){}
		virtual ~Lock(){}

		void Lockup(void)
		{
			_lock.Lockup();
		}
		BOOL TryLockup(void)
		{
			return _lock.TryLockup();
		}
		void Unlock(void)
		{
			_lock.Unlock();
		}

	private:
		DISALLOW_COPY_AND_ASSIGN(Lock);
		L _lock;
	};

	typedef Lock<platform::ThreadLockImpl> TLock;
	typedef Lock<platform::ProcessLockImpl> PLock;



	template<class L>
	class AutoLock
	{
	public:
		explicit AutoLock(L &lock_) : _lock(lock_)
		{
			_lock.Lockup();
		}
		virtual ~AutoLock()
		{
			_lock.Unlock();
		}

	private:
		DISALLOW_COPY_AND_ASSIGN(AutoLock);
		L& _lock;
	};

	typedef AutoLock<TLock> TAutoLock;
	typedef AutoLock<PLock> PAutoLock;


	typedef struct _tagThreadParam
	{
		HANDLE hThreadHandle_;
		HANDLE hEventHandle_;
		base::TLock lock_;

		_tagThreadParam()
		{
			hThreadHandle_ = NULL;
			hEventHandle_ = NULL;
		}

		void Release()
		{
			if(hThreadHandle_)
			{
				::CloseHandle(hThreadHandle_);
				hThreadHandle_ = NULL;
			}
			if(hEventHandle_)
			{
				::CloseHandle(hEventHandle_);
				hEventHandle_ = NULL;
			}
		}
	}ThreadParam;

} // namespace base

#endif // __LOCK_H__
