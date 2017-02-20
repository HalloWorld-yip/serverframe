#pragma once
#include<WinSock2.h>
#include<windows.h>
class Lock
{
public:
	Lock() {
		InitializeCriticalSection(&_cs);
	}
	void lock()//仅允许LockGuard调用，禁止用户调用
	{
		EnterCriticalSection(&_cs);
	}
	void unlock()//仅允许LockGuard调用，禁止用户调用
	{
		LeaveCriticalSection(&_cs);
	}
	
private:
	friend class Condition;
	CRITICAL_SECTION _cs;
};

class LockGuard
{
public:
	LockGuard(Lock& csl) :_csl(csl)
	{
		_csl.lock();
	}
	~LockGuard() { _csl.unlock(); }
private:
	Lock& _csl;
};

class Condition
{
public:
	Condition(Lock& csl):_csl(csl)
	{
		InitializeConditionVariable(&cv);
	}

	void notify() { WakeConditionVariable(&cv); }
	void notifyall() { WakeAllConditionVariable(&cv); }
	void waitForSeconds(size_t dwTimeout)
	{
		SleepConditionVariableCS(&cv, &_csl._cs, dwTimeout);
	}
private:
	CONDITION_VARIABLE cv;
	Lock &_csl;
};