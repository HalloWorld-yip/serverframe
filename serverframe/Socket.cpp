#pragma warning(disable:4996)
#include<WS2tcpip.h>
#include "Socket.h"
#include"Logger.h"

const char PORT[] = "3409";
const unsigned MAX_CLIENT = 5;
Socket::Socket() {
	TRACE << "Socket ctor";

	WSAData m_wd;
	if (0 != WSAStartup(MAKEWORD(1, 1), &m_wd))
	{
		WRONG << "wsaStartup failed\n";
	}
	m_s = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void Socket::bind()
{
	TRACE << "Socket bind";

	addrinfo hints, *serverinfo, *p;
	//set hints
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (0 != getaddrinfo(NULL, PORT, &hints, &serverinfo)) {
		WRONG << "getaddrinfo failed\n";
		return;
	}
	for (p = serverinfo; p != NULL; p = p->ai_next)
		if (SOCKET_ERROR != ::bind(m_s, serverinfo->ai_addr, serverinfo->ai_addrlen))
			break;
		else
			WRONG << "bind failed\n";
	freeaddrinfo(serverinfo);
	if (p == NULL)
		WRONG << "all bind failed\n";

}
void Socket::listen()
{
	TRACE << "Socket listen";

	if (0 != ::listen(m_s, MAX_CLIENT))
	{
		WRONG << "listen\n" ;
	}
}
;
