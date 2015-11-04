#pragma once

#ifdef WIN32
#include <windows.h>
#else
#include <semaphore.h>
#endif

class Locker
{
#ifdef WIN32
	CRITICAL_SECTION _CriticalSection;
#else
	sem_t mutex;
#endif

public:
	Locker();

	virtual ~Locker();
	void Lock();

	void Unlock();

private:
	Locker(const Locker&);
	Locker& operator=(const Locker&);
};
