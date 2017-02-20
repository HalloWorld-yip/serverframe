#pragma once
#include<WinSock2.h>
#include"Channel.h"
#include"Loop.h"
class Connection
{
public:
	typedef std::function<void()> EventCallback;
	//typedef std::function<void(std::shared_ptr<Connection>)> IOCallback;
	typedef std::function<void(Connection*)> IOCallback;

	//Connection(std::shared_ptr<Channel>& channel);
	Connection(Channel* channel);
	~Connection() {
		TRACE << "~Connection";
	}
	void Send(WSABUF* buf);
	void Recv(WSABUF* buf);
	//void Close();

	void postSend();
	void postRecv();

	SOCKET socket() { return m_IOSocket; }

	void setReadCallback(const EventCallback& cb) { m_pIOChannel->setReadCallback(cb); }
	void setWriteCallback(const EventCallback& cb) { m_pIOChannel->setWriteCallback(cb); }
	void setCloseCallback(const EventCallback& cb) { m_pIOChannel->setCloseCallback(cb); }
	void setErrorCallback(const EventCallback& cb) { m_pIOChannel->setErrorCallback(cb); }


	void setPostRecv(const EventCallback& cb) { m_postRecv = cb; }
	void setPostSend(const EventCallback& cb) { m_postSend = cb; }

	

private:

	EventCallback m_postSend;
	EventCallback m_postRecv;

	OVERLAPPEDPLUS m_readOverlapped;
	OVERLAPPEDPLUS m_writeOverlapped;
	//std::shared_ptr<Channel> m_pIOChannel;
	Channel* m_pIOChannel;
	SOCKET	m_IOSocket;
	DWORD m_inBytes;
	DWORD m_outBytes;
};
