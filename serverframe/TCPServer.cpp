#include "TCPServer.h"
#include"logger.h"
TCPServer::TCPServer(Loop &loop)
	:m_loop(loop),
	m_acceptor(m_accetpSocket.socket(),m_loop.IOCPport(),m_connections)
{
	TRACE << "TCPServer ctor";

	

}

void TCPServer::start() 
{
	POVERLAPPEDPLUS	m_pOverlap(new OVERLAPPEDPLUS);
	m_pOverlap->event_ = Channel::Event::ACCEPT;
	m_acceptor.postAccept(m_pOverlap);
}






void TCPServer::removeConnection()
{
	
}



//void TCPServer::addChannel(std::shared_ptr<Channel> channel)
//{
//	TRACE << "add channel";
//	channel->setIdx(m_ChannelMap.size());
//	m_ChannelMap.insert(std::make_pair(channel->getIdx(), channel));
//}
//
//void TCPServer::deleteChannel(std::shared_ptr<Channel> channel)
//{
//	TRACE << "delete channel";
//
//	channel->setDevent(0);
//	m_ChannelMap.erase(channel->getIdx());
//
//}