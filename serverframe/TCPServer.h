#pragma once
#include<functional>

#include"Loop.h"
#include"Channel.h"
#include"Socket.h"
#include"Connection.h"
#include"Acceptor.h"
#include"LockMap.h"



/*
	
	数据全部储存在投递到端口上的Channel中，取出时对其进行操作。
	Server在建立连接时留给用户一个启动服务的接口，对于READ,WRITE，CLOSE，ERR等回调需要用户自己设置
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
	void Start();

	void Serve(IOCallback &cb) { m_serveCallback = cb; }
private:

	void postAccept(overlappedplus* pOverlapped);
	void newConnection(overlappedplus* pOverlapped);
	void removeConnection(Connection * conn);


	EventCallback m_acceptCallback;
	IOCallback m_serveCallback;

	/*
	用于管理连接相关数据的生命期
	*/
	ConnectionsList m_connections;
	Lock		lock;
	Loop&		m_loop;
	Acceptor	m_acceptor;


};