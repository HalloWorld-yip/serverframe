#include"LogFile.h"
#include"Logger.h"
#include<iostream>
#include<memory>
#include<functional>
#include"TCPServer.h"

#include"Thread.h"
using namespace std;

WSABUF wbuf;
char buffer[512];

void background()
{
	logFile.run();
	logFile.workthread();
}

void readcallback(Connection* pConnection)
{
	static int c = 0;
	c++;
	std::cout << "client" <<c<< endl;
	wbuf.buf = buffer;
	wbuf.len = sizeof(buffer);
	pConnection->Recv(&wbuf);
}
void closecallback(Connection* pConnection)
{
	
}
void defaultcallback(Connection* pConnection){}

void Scallback(Connection* pConn)
{
	pConn->setWriteCallback(std::bind(defaultcallback, pConn));
	pConn->setReadCallback(std::bind(readcallback, pConn));
	pConn->setCloseCallback(std::bind(closecallback, pConn));
	pConn->setErrorCallback(std::bind(defaultcallback, pConn));
	//postIO
	readcallback(pConn);
}

void loopthread(Loop* loop)
{
	loop->loopping();
}

void serve(Loop* loop)
{
	TCPServer server(*loop);
	server.Start();
	std::function<void(Connection*)> funtor(Scallback);
	server.Serve(funtor);
	loop->Start(); 
	Thread thread1(std::bind(loopthread, loop));
	thread1.run(); 
	Thread thread2(std::bind(loopthread, loop));
	thread2.run();
}

int main()
{
	
	Thread thread1(&background);
	thread1.run();
	Loop loop;
	Thread thread2(std::bind(serve, &loop));
	thread2.run();


	Sleep(80000);
	loop.stop();
	logFile.stop();
	
	return 0;
}