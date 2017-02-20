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
	数据和操作分离，多线程下，管理操作的类绑定在其函数上，其拥有的需要被访问的数据应尽量减少
	数据全部储存在投递到端口上的Channel中，取出时对其进行操作。
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
	addChannel单纯地将管理句柄的Channel加入Map记录
	需要预先投递事件。
	*/
	/*
	将channel的管理分发到connect中，channelMap似乎不太必要
	void  addChannel(std::shared_ptr<Channel> channel);
	void  deleteChannel(std::shared_ptr<Channel> channel);

	std::map<int, std::shared_ptr<Channel> > m_ChannelMap;
*/
	Loop&	m_loop;
	Asocket	m_accetpSocket;
	ConnectionsList m_connections;


	Acceptor m_acceptor;
};