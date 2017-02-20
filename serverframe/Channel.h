#pragma once
#include<functional>
#include<WinSock2.h>
#include"Logger.h"

struct overlappedplus;

class Channel
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(overlappedplus*)> AcceptCallback;

	Channel(SOCKET s);
	
	~Channel() {
		TRACE << "channel dtor\n";
	}
	enum Event
	{
		READ = 1,
		WRITE = 2,
		CLOSE = 4,
		ERR = 8,
		ACCEPT = 16
	};
	


	void setAcceptCallback(const AcceptCallback& cb) { m_AcceptCallback = cb; }
	void setReadCallback(const EventCallback& cb) { m_readCallback = cb; }
	void setWriteCallback(const EventCallback& cb) { m_writeCallback = cb; }
	void setCloseCallback(const EventCallback& cb) { m_closeCallback = cb; }
	void setErrorCallback(const EventCallback& cb) { m_errorCallback = cb; }
	

	void postEvent(overlappedplus* p_info, DWORD dwBytesTransfered);
	SOCKET getSocket() { return m_socket; }
	
	void setIdx(int idx) { m_idx = idx; }
	int getIdx() { return m_idx; }

	int	getDevent() { return m_devent; }
	void setDevent(int event_) { m_devent = event_; }

	int	getRevent() { return m_revent; }
	void setRevent(int event_) { m_revent = event_; }

private:
	AcceptCallback m_AcceptCallback;
	EventCallback m_readCallback;
	EventCallback m_writeCallback;
	EventCallback m_closeCallback;
	EventCallback m_errorCallback;
	SOCKET	m_socket;
	
	int		m_devent;//desired event
	int		m_revent;//real event
	int		m_idx;
};

typedef struct overlappedplus
{
	OVERLAPPED overlapped_;
	Channel::Event event_;

	//std::shared_ptr<Channel> pChannel;
	Channel* pChannel;
}OVERLAPPEDPLUS, *POVERLAPPEDPLUS;
