#pragma once


/*·â×°socket*/
#include<WinSock2.h>

#pragma comment(lib,"ws2_32.lib")
class Socket
{
public:
	Socket();
	Socket(const Socket& s):m_s(s.m_s){}
	void bind();
	void listen();

	SOCKET socket() { return m_s; }
private:
	SOCKET m_s;
};
