#include "Connection.h"

Connection::Connection(Channel* channel) :m_pIOChannel(channel),m_IOSocket(m_pIOChannel->getSocket())
{
	m_pIOChannel->setDevent(Channel::Event::READ | Channel::Event::WRITE | Channel::Event::CLOSE | Channel::Event::ERR);
	m_pIOChannel->setReadCallback(std::bind(&Connection::readHandler, this));
	m_pIOChannel->setWriteCallback(std::bind(&Connection::writeHandler, this));
	m_pIOChannel->setCloseCallback(std::bind(&Connection::closeHandler, this));
	m_pIOChannel->setErrorCallback(std::bind(&Connection::errHandler, this));
	m_readOverlapped.event_ = Channel::Event::READ;
	m_writeOverlapped.event_ = Channel::Event::WRITE;
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



void Connection::postRecv()
{
	readHandler();
}
