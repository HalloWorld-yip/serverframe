#pragma once

#include<WS2tcpip.h>
#include<mswsock.h>
#include<memory>
#include"Thread.h"
#include"Channel.h"
#include<vector>



class Loop
{
public:
	typedef std::function<void()> exeCallback;
	Loop();
	void Start();
	void stop() { m_running = false; }
	void loopping();


	HANDLE IOCPport() { return m_IOCPport; }
private:
	HANDLE m_IOCPport;
	bool m_running;
};