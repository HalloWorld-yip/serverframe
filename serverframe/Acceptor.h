#pragma once
#include<WinSock2.h>
#include"Logger.h"
#include"Channel.h"
#include"Socket.h"
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


	Acceptor(HANDLE port, EventCallback& cb);


	void setAcceptCallback(const EventCallback& cb) { m_pAcceptChannel->setAcceptCallback(cb); }

	bool AcceptEx(SOCKET s, LPOVERLAPPED lpOverlapped);
private:
	void getAcceptFunc();

	HANDLE m_IOCPPort;
	Socket m_acceptSocket;
	std::shared_ptr<Channel> m_pAcceptChannel;

	//func ptr
	LPFN_ACCEPTEX m_lpfnAcceptEx;

	char acceptbuf[512];//����buf��÷��ڽ�β����Խ����ɵ�bug���Է���
};
