#pragma once
#include<WinSock2.h>
#include"Logger.h"
#include"Channel.h"
#include"Socket.h"
#include"Connection.h"
#include"LockMap.h"
/*
	Acceptor������TCPServer֮�У�������Ӧ��ͬ��TCPServer
	�����ڷ�����ʼ�մ�
	���accept socket �������ݴ�����Acceptorջ�ϣ�
	��IOsocket�Ĺ���Ӧת�Ƶ���Ӧ��channel�ϡ�
*/
class Acceptor
{
public:

	typedef std::function<void(overlappedplus*)> EventCallback;
	typedef std::function<void(Connection*)> IOCallback;
	//typedef std::vector<std::shared_ptr<Connection> > ConnectionsList;
	typedef std::map<SOCKET, std::shared_ptr<Connection> > ConnectionsList;
	////debug:
	//typedef std::map<SOCKET, Connection* > ConnectionsList;


	Acceptor(SOCKET s, HANDLE port, ConnectionsList connections);

	void postAccept(overlappedplus* pOverlapped);

	void setAcceptCallback(const EventCallback& cb) { m_pAcceptChannel->setAcceptCallback(cb); }

	void setReadCallback(const IOCallback& cb) { m_readCallback = cb; }
	void setWriteCallback(const IOCallback& cb) { m_writeCallback = cb; }
	void setCloseCallback(const IOCallback& cb) { m_closeCallback = cb; }
	void setErrorCallback(const IOCallback& cb) { m_errorCallback = cb; }

private:
	void getAcceptFunc();
	void defaultAcceptHandle(overlappedplus* pOverlapped );
	void defaultCloseHandle(Connection *conn);

	HANDLE m_IOCPPort;
	SOCKET m_acceptSocket;
	std::shared_ptr<Channel> m_pAcceptChannel;
	ConnectionsList m_connections;

	//func ptr
	LPFN_ACCEPTEX m_lpfnAcceptEx;
	IOCallback m_readCallback;
	IOCallback m_writeCallback;
	IOCallback m_closeCallback;
	IOCallback m_errorCallback;
	Lock lock;
	char acceptbuf[512];//����buf��÷��ڽ�β����Խ����ɵ�bug���Է���
};
