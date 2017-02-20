
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<mswsock.h>
#include"Logger.h"
#include "Acceptor.h"

Acceptor::Acceptor(SOCKET s ,HANDLE port, ConnectionsList connections)
	:m_acceptSocket(s),m_IOCPPort(port),
	m_pAcceptChannel(new Channel(m_acceptSocket)),
	
	m_connections(connections)
{
	getAcceptFunc();

	CreateIoCompletionPort((HANDLE)m_acceptSocket, m_IOCPPort, (ULONG_PTR)m_pAcceptChannel.get(), 0);

	m_pAcceptChannel->setDevent(Channel::Event::ACCEPT | Channel::Event::CLOSE | Channel::Event::ERR);
	setAcceptCallback(std::bind(&Acceptor::defaultAcceptHandle, this, std::placeholders::_1));
}

void Acceptor::postAccept(overlappedplus* pOverlapped)
{

	TRACE << "post accept event";
	SOCKET newSocket;
	{
		LockGuard guard(lock);
		newSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED); 
		/*if (m_connections.find(newSocket) != m_connections.end())
		{
			WRONG << "socket has not free!";
		}*/
	}
	if (newSocket == SOCKET_ERROR)
	{
		WRONG << "socket failed:" << WSAGetLastError();
		return;
	}
	if (m_connections.find(newSocket) != m_connections.end())
	{
		WRONG << "socket has not free!";
	}
	pOverlapped->pChannel = new Channel(newSocket);

	DWORD dwBytes = 0;
	memset(&pOverlapped->overlapped_, 0, sizeof(pOverlapped->overlapped_));
	//设置事件
	pOverlapped->overlapped_.hEvent = WSACreateEvent();
	if (pOverlapped->overlapped_.hEvent == WSA_INVALID_EVENT)
		INFO << "create event failed\n";

	if (0 == m_lpfnAcceptEx(m_acceptSocket,
		pOverlapped->pChannel->getSocket(),
		acceptbuf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		&pOverlapped->overlapped_))
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			TRACE << "repo accept on IOsocket :"<<(int)pOverlapped->pChannel->getSocket();

		}
		else
		{
			INFO << "other error:"<<err;
		}
	}
	else
		WRONG << "Accept not pending\n";
}

void Acceptor::getAcceptFunc()
{
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes = 0;
	if (::WSAIoctl(m_acceptSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(guidAcceptEx), &m_lpfnAcceptEx, sizeof(m_lpfnAcceptEx),
		&dwBytes, NULL, NULL) != 0)
	{
		int a = WSAGetLastError();
		WRONG << "wsaioctl failed:" << "\n";
	}
}

void Acceptor::defaultAcceptHandle(overlappedplus* pOverlapped)
{

	TRACE << "acceptHandle";

	CreateIoCompletionPort((HANDLE)pOverlapped->pChannel->getSocket(), m_IOCPPort, (ULONG_PTR)pOverlapped->pChannel, 0);
	Connection* pConn = new Connection(pOverlapped->pChannel);
	pConn->setWriteCallback(m_writeCallback);
	pConn->setReadCallback(m_readCallback);
	pConn->setCloseCallback(std::bind(&Acceptor::defaultCloseHandle,this,pConn));
	pConn->setErrorCallback(m_errorCallback);


	/*
		这里要先判断insert进去没



	*/
	//auto p = std::shared_ptr<Connection>(pConn);
	int num = pOverlapped->pChannel->getSocket();
	{
		LockGuard guard(lock);
		auto pa = m_connections.insert(std::pair<int, Connection*>(num, pConn));
	}
	//m_connections.push_back(std::shared_ptr<Connection>(pConn));
	//postIO
	pConn->postRecv();


	//repost
	postAccept(pOverlapped);
}

void Acceptor::defaultCloseHandle(Connection * conn)
{
	TRACE << "cancel IO socket:" << (int)conn->socket();
	if (CancelIoEx((HANDLE)conn->socket(), NULL) == 0)
	{
		int err;
		if ((err = GetLastError()) != ERROR_NOT_FOUND)
			WRONG << "CancelIO failed:" << err;
	}

	LockGuard guard(lock);
	closesocket(conn->socket());
	m_connections.erase(conn->socket());
}

