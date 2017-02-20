#include "TCPServer.h"
#include"logger.h"
TCPServer::TCPServer(Loop &loop)
	:m_loop(loop),
	m_acceptCallback(std::bind(&TCPServer::newConnection, this, std::placeholders::_1)),
	m_acceptor(m_loop.IOCPport(),m_acceptCallback)
{
	TRACE << "TCPServer ctor";

	

}

void TCPServer::start() 
{
	POVERLAPPEDPLUS	m_pOverlap(new OVERLAPPEDPLUS);
	m_pOverlap->event_ = Channel::Event::ACCEPT;
	postAccept(m_pOverlap);
}






void TCPServer::postAccept(overlappedplus* pOverlapped)
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
	pOverlapped->pChannel = new Channel(newSocket);

	DWORD dwBytes = 0;
	memset(&pOverlapped->overlapped_, 0, sizeof(pOverlapped->overlapped_));
	//ÉèÖÃÊÂ¼þ
	pOverlapped->overlapped_.hEvent = WSACreateEvent();
	if (pOverlapped->overlapped_.hEvent == WSA_INVALID_EVENT)
		INFO << "create event failed\n";

	if (0 == m_acceptor.AcceptEx(pOverlapped->pChannel->getSocket(),&pOverlapped->overlapped_))
	{
		int err = WSAGetLastError();
		if (err == WSA_IO_PENDING)
		{
			TRACE << "repo accept on IOsocket :" << (int)pOverlapped->pChannel->getSocket();

		}
		else
		{
			INFO << "other error:" << err;
		}
	}
	else
		WRONG << "Accept not pending\n";
}

void TCPServer::newConnection(overlappedplus * pOverlapped)
{
	

		TRACE << "acceptHandle";

		CreateIoCompletionPort((HANDLE)pOverlapped->pChannel->getSocket(), m_loop.IOCPport(), (ULONG_PTR)pOverlapped->pChannel, 0);
		Connection* pConn = new Connection(pOverlapped->pChannel);
		pConn->setWriteCallback(std::bind(m_writeCallback, pConn));
		pConn->setReadCallback(std::bind(m_readCallback, pConn));
		pConn->setCloseCallback(std::bind(&TCPServer::removeConnection, this, pConn));
		pConn->setErrorCallback(std::bind(m_errorCallback, pConn));
		pConn->setPostRecv(std::bind(m_readCallback, pConn));

		{
			LockGuard guard(lock);
			auto pa = m_connections.insert(std::pair<int, Connection*>(pOverlapped->pChannel->getSocket(), pConn));
			if (pa.second != true)
			{
				WRONG << "map insert failed!!!!";
			}
		}
		//postIO
		pConn->postRecv();


		//repost
		postAccept(pOverlapped);
	
}

void TCPServer::removeConnection(Connection * conn)
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





//void TCPServer::addChannel(std::shared_ptr<Channel> channel)
//{
//	TRACE << "add channel";
//	channel->setIdx(m_ChannelMap.size());
//	m_ChannelMap.insert(std::make_pair(channel->getIdx(), channel));
//}
//
//void TCPServer::deleteChannel(std::shared_ptr<Channel> channel)
//{
//	TRACE << "delete channel";
//
//	channel->setDevent(0);
//	m_ChannelMap.erase(channel->getIdx());
//
//}