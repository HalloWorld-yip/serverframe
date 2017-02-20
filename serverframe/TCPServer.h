#pragma once
#include<functional>

#include"Loop.h"
#include"Channel.h"
#include"Socket.h"
#include"Connection.h"
#include"Acceptor.h"
#include"LockMap.h"
class Asocket
{
public:
	Asocket() { s_.bind(); s_.listen(); }
	SOCKET socket() { return s_.socket(); }
private:
	Socket s_;
};


/*
	���ݺͲ������룬���߳��£����������������亯���ϣ���ӵ�е���Ҫ�����ʵ�����Ӧ��������
	����ȫ��������Ͷ�ݵ��˿��ϵ�Channel�У�ȡ��ʱ������в�����
*/
class TCPServer
{
public:
	typedef std::function<void()> EventCallback;
	//typedef std::function<void(std::shared_ptr<Connection>)> IOCallback;
	typedef std::function<void(Connection*)> IOCallback;
	//typedef std::vector<std::shared_ptr<Connection> > ConnectionsList;
	typedef std::map<SOCKET, std::shared_ptr<Connection> > ConnectionsList;
	////debug:
	//typedef std::map<SOCKET, Connection* > ConnectionsList;

	TCPServer(Loop &loop);
	~TCPServer() { WSACleanup(); }
	void start();

	void setReadCallback(const IOCallback& cb) { m_acceptor.setReadCallback(cb); }
	void setWriteCallback(const IOCallback& cb) { m_acceptor.setWriteCallback(cb); }
	void setCloseCallback(const IOCallback& cb) { m_acceptor.setCloseCallback(cb); }
	void setErrorCallback(const IOCallback& cb) { m_acceptor.setErrorCallback(cb); }
	
private:

	void removeConnection();



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
	Loop&	m_loop;
	Asocket	m_accetpSocket;
	ConnectionsList m_connections;


	Acceptor m_acceptor;
};