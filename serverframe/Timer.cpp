#include "Timer.h"

SYSTEMTIME Timer::get_time()
{
	updatetime();
	return SYSTEMTIME(m_StartTime);
}

void Timer::updatetime()
{
	DWORD now = GetTickCount();
	unsigned int during = now - m_OriginTicks + m_StartTime.wMilliseconds;
	m_StartTime.wMilliseconds = during % 1000;
	during /= 1000;
	;

	if (during + m_StartTime.wSecond > 59) {
		GetLocalTime(&m_StartTime);
		m_OriginTicks = GetTickCount();
	}
	else
		m_StartTime.wSecond += during;
}
