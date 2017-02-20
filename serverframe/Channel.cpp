#include "Channel.h"

Channel::Channel(SOCKET s)
	:m_socket(s),m_devent(0),m_idx(-1),m_revent(0)
{
}

void Channel::postEvent(overlappedplus* pInfo,DWORD dwBytesTransfered)
{
	TRACE << "post channel event";
	switch (pInfo->event_) {
	case Channel::READ:
		if (dwBytesTransfered == 0)
		{

			if (m_devent&Event::CLOSE) m_closeCallback();
			break;
		}
		else 
		{
			if (m_devent&Event::READ) m_readCallback();
		}
	case Channel::ACCEPT:
		if (m_devent&Event::ACCEPT) m_AcceptCallback(pInfo);
		break;
	case Channel::WRITE:
		if (m_devent&Event::WRITE) m_writeCallback();
		break;
	case Channel::ERR:
		if (m_devent&Event::ERR) m_errorCallback();
		break;
	default:
		break;
	}
}
