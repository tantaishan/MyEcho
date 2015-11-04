#include "Locker.h"

Locker::Locker()
{
#ifdef WIN32
	InitializeCriticalSection(&_CriticalSection);
#else
	sem_init(&mutex,0,1);
#endif
}

Locker::~Locker()
{
#ifdef WIN32
	DeleteCriticalSection(&_CriticalSection);
#else
	sem_destroy(&mutex);
#endif
}

void Locker::Lock()
{
#ifdef WIN32
	EnterCriticalSection(&_CriticalSection);
#else
	sem_wait(&mutex);
#endif
}

void Locker::Unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&_CriticalSection);
#else
	sem_post(&mutex);
#endif
}