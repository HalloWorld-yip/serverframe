#pragma once
#include<WinSock2.h>
#include"key.h"
typedef unsigned char uint8_t;
struct net_pkg {
	int sgn;
	int pts;
	char data[1];
};

struct net_clt {
	char name[128];
	ULONG ip;
	int status;
	int life;
	SOCKET s;
	sockaddr_in saddr;
};

struct net_msg{
	sockaddr_in saddr;
	int size;
	int pts;
	//int port;
	int sgn;
	char name[128];
	char buf[UDPBUF];
};