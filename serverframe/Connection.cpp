#include "Connection.h"

Connection::Connection(Channel* channel, IOCallback &cb) :m_closeCallback(cb),m_pIOChannel(channel),m_IOSocket(m_pIOChannel->getSocket())
{
	m_pIOChannel->setDevent(Channel::Event::READ | Channel::Event::WRITE | Channel::Event::CLOSE | Channel::Event::ERR);
	m_readOverlapped.event_ = Channel::Event::READ;
	m_writeOverlapped.event_ = Channel::Event::WRITE;
	m_pIOChannel->setCloseCallback(std::bind(&Connection::closeHandle, this));
}

void Connection::Send(WSABUF* buf)
{
	DWORD flags = 0;
	WSASend(m_IOSocket, buf, 1, &m_outBytes, flags, &m_writeOverlapped.overlapped_, NULL);
}

void Connection::Recv(WSABUF* buf)
{
	DWORD flags = 0;
	WSARecv(m_IOSocket, buf, 1, &m_inBytes, &flags, &m_readOverlapped.overlapped_, NULL);
}

void Connection::closeHandle()
{
	m_defineClose();
	m_closeCallback(this);
}

