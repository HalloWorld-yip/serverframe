#pragma once
#include<functional>

#include"Loop.h"
#include"Channel.h"
#include"Socket.h"
#include"Connection.h"
#include"Acceptor.h"
#include"LockMap.h"



/*
	���ݺͲ������룬���߳��£����������������亯���ϣ���ӵ�е���Ҫ�����ʵ�����Ӧ��������
	����ȫ��������Ͷ�ݵ��˿��ϵ�Channel�У�ȡ��ʱ������в�����
*/
class TCPServer
{
public:
	typedef std::function<void(overlappedplus*)> EventCallback;
	//typedef std::function<void(std::shared_ptr<Connection>)> IOCallback;
	typedef std::function<void(Connection*)> IOCallback;

	typedef std::map<SOCKET, std::shared_ptr<Connection> > ConnectionsList;


	TCPServer(Loop &loop);
	~TCPServer() { WSACleanup(); }
	void start();


	void setReadCallback(const IOCallback& cb) { m_readCallback = cb; }
	void setWriteCallback(const IOCallback& cb) { m_writeCallback = cb; }
	void setCloseCallback(const IOCallback& cb) { m_closeCallback = cb; }
	void setErrorCallback(const IOCallback& cb) { m_errorCallback = cb; }
	
private:

	void postAccept(overlappedplus* pOverlapped);

	void newConnection(overlappedplus* pOverlapped);
	void removeConnection(Connection * conn);



	/*
	addChannel�����ؽ���������Channel����Map��¼
	��ҪԤ��Ͷ���¼���
	*/
	/*
	��channel�Ĺ���ַ���connect�У�channelMap�ƺ���̫��Ҫ
	void  addChannel(std::shared_ptr<Channel> channel);
	void  deleteChannel(std::shared_ptr<Channel> channel);

	std::map<int, std::shared_ptr<Channel> > m_ChannelMap;
*/

	EventCallback m_acceptCallback;
	IOCallback m_readCallback;
	IOCallback m_writeCallback;
	IOCallback m_closeCallback;
	IOCallback m_errorCallback;

	Lock		lock;
	Loop&		m_loop;
	Acceptor	m_acceptor;

	ConnectionsList m_connections;

};