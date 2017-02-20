
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<mswsock.h>
#include"Logger.h"
#include "Acceptor.h"

Acceptor::Acceptor(HANDLE port, EventCallback& cb)
	:m_IOCPPort(port),
	m_pAcceptChannel(new Channel(m_acceptSocket.socket()))
{
	m_acceptSocket.bind();
	m_acceptSocket.listen();

	getAcceptFunc();

	CreateIoCompletionPort((HANDLE)m_acceptSocket.socket(), m_IOCPPort, (ULONG_PTR)m_pAcceptChannel.get(), 0);

	m_pAcceptChannel->setDevent(Channel::Event::ACCEPT | Channel::Event::CLOSE | Channel::Event::ERR);
	setAcceptCallback(std::bind(cb, std::placeholders::_1));
}



bool Acceptor::AcceptEx(SOCKET s, LPOVERLAPPED lpOverlapped)
{

	DWORD dwBytes = 0;
	return m_lpfnAcceptEx(m_acceptSocket.socket(),
		s,
		acceptbuf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		lpOverlapped);
}

void Acceptor::getAcceptFunc()
{
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes = 0;
	if (::WSAIoctl(m_acceptSocket.socket(), SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(guidAcceptEx), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx),
		&dwBytes, NULL, NULL) != 0)
	{
		int a = WSAGetLastError();
		WRONG << "wsaioctl failed:" << "\n";
	}
}


