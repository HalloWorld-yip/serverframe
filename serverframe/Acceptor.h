#pragma once
#include<WinSock2.h>
#include"Logger.h"
#include"Channel.h"
#include"Socket.h"
/*
	Acceptor存在于TCPServer之中，生命期应等同于TCPServer
	至少在服务中始终存活。
	因此accept socket 、的数据存在于Acceptor栈上，
	而IOsocket的管理应转移到对应的channel上。
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

	char acceptbuf[512];//这类buf最好放在结尾，当越界造成的bug难以发现
};
