#pragma once
#include<time.h>
#include<WinSock2.h>
#include<Windows.h>

class Timer
{
public:
	Timer()
	{
		GetLocalTime(&m_StartTime);
		m_OriginTicks = GetTickCount();
	}
	SYSTEMTIME get_time();
private:
	void updatetime();

	DWORD m_OriginTicks;
	SYSTEMTIME m_StartTime;
};