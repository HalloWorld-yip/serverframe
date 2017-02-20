#pragma once

#include<map>
#include<WinSock2.h>
#include<memory>
#include"Lock.h"

class Connection;
class LockMap
{
	typedef std::pair<SOCKET, std::shared_ptr<Connection> >  pSC;
public:
	auto insert(pSC &p) { LockGuard guard(lock); return m_connections.insert(p); }
	auto erase(SOCKET key) { LockGuard guard(lock); return m_connections.erase(key); }
private:
	Lock lock;
	std::map<SOCKET, std::shared_ptr<Connection> > m_connections;
};