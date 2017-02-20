#pragma once
#include<process.h>
#include<functional>
#include<WinSock2.h>
#include"Logger.h"
class Thread
{
public:
	typedef std::function<void()>	threadcallback;

	explicit Thread(const threadcallback& cb):func(cb){}
	~Thread() 
	{
		TRACE << "Thread dtor\n";
		WaitForSingleObject(m_Handle, INFINITE);
		CloseHandle(m_Handle);
	}

	unsigned ThreadID() { return m_ThreadID; }

	void run() { m_Handle = (HANDLE)_beginthreadex(0, 0, &Thread::threadcall, &func, 0, &m_ThreadID); }
private:
	static unsigned  __stdcall threadcall(void* pm) 
	{
		threadcallback* pfunc= (threadcallback*)pm;
		(*pfunc)();
		return 0;
	}

	threadcallback func;
	unsigned m_ThreadID;
	HANDLE m_Handle;//为防止对close后的HANDLE操作，不提供接口
};
