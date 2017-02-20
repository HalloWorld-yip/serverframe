#pragma once
#include<functional>

#include"Loop.h"
#include"Channel.h"
#include"Socket.h"
#include"Connection.h"
#include"Acceptor.h"
#include"LockMap.h"



/*
	
	����ȫ��������Ͷ�ݵ��˿��ϵ�Channel�У�ȡ��ʱ������в�����
	Server�ڽ�������ʱ�����û�һ����������Ľӿڣ�����READ,WRITE��CLOSE��ERR�Ȼص���Ҫ�û��Լ�����
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
	���ڹ�������������ݵ�������
	*/
	ConnectionsList m_connections;
	Lock		lock;
	Loop&		m_loop;
	Acceptor	m_acceptor;


};