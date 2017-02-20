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

	void setReadCallback(const IOCallback& cb) { m_readCallback = cb; }
	void setWriteCallback(const IOCallback& cb) { m_writeCallback = cb; }
	void setCloseCallback(const IOCallback& cb) { m_closeCallback = cb; }
	void setErrorCallback(const IOCallback& cb) { m_errorCallback = cb; }
private:
	void readHandler() { m_readCallback(this); }//fix 
	void writeHandler() { m_writeCallback(this); }
	void closeHandler() { m_closeCallback(this); }
	void errHandler() { m_errorCallback(this); }


	IOCallback m_readCallback;
	IOCallback m_writeCallback;
	IOCallback m_closeCallback;
	IOCallback m_errorCallback;

	OVERLAPPEDPLUS m_readOverlapped;
	OVERLAPPEDPLUS m_writeOverlapped;
	//std::shared_ptr<Channel> m_pIOChannel;
	Channel* m_pIOChannel;
	SOCKET	m_IOSocket;
	DWORD m_inBytes;
	DWORD m_outBytes;
};
