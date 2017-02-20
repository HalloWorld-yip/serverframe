#pragma once
#include<WinSock2.h>
#include"Channel.h"
#include"Loop.h"


/*
	ͨ����������Ļص��������Ự��ָ�봫�ݸ��û���ʹĿ����������Ự֮�����ֱ����ϵ��
	���������Ҫ���ص�������
*/
class Connection
{
public:
	typedef std::function<void()> EventCallback;
	//typedef std::function<void(std::shared_ptr<Connection>)> IOCallback;
	typedef std::function<void(Connection*)> IOCallback;

	//Connection(std::shared_ptr<Channel>& channel);
	Connection(Channel* channel, IOCallback& cb);
	~Connection() {
		TRACE << "~Connection";
	}
	void Send(WSABUF* buf);
	void Recv(WSABUF* buf);
	//void Close();


	SOCKET socket() { return m_IOSocket; }

	void setReadCallback(const EventCallback& cb) { m_pIOChannel->setReadCallback(cb); }
	void setWriteCallback(const EventCallback& cb) { m_pIOChannel->setWriteCallback(cb); }
	void setCloseCallback(const EventCallback& cb) { m_defineClose = cb; }
	void setErrorCallback(const EventCallback& cb) { m_pIOChannel->setErrorCallback(cb); }



	

private:
	void closeHandle();

	IOCallback m_closeCallback;
	EventCallback m_defineClose;

	OVERLAPPEDPLUS m_readOverlapped;
	OVERLAPPEDPLUS m_writeOverlapped;
	//std::shared_ptr<Channel> m_pIOChannel;
	Channel* m_pIOChannel;
	SOCKET	m_IOSocket;
	DWORD m_inBytes;
	DWORD m_outBytes;
	bool alive;
};
