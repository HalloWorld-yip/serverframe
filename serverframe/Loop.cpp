#include "Loop.h"
#include"Logger.h"

const unsigned THREAD_NUM = 1;
const DWORD WAIT_TIME = 5000;

Loop::Loop()
	:m_running(false)
{
	m_IOCPport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (m_IOCPport == 0) 
	{
		WRONG << "create IOCP port";
	}
	

}

void Loop::start()
{
	m_running = true;

}

void Loop::loopping()
{
	TRACE << "start looping";

	Channel	*pChannel = NULL;

	LPOVERLAPPED	pOverlapped = NULL;
	POVERLAPPEDPLUS pInfo;
	DWORD		dwBytesTransfered = -1;



	while(m_running) {
		//一次epoll可能获得多次事件，但一次GetQueuedCompletionStatus只对应一个事件，直接处理。
		BOOL bReturn = GetQueuedCompletionStatus(m_IOCPport, &dwBytesTransfered, (PULONG_PTR)&pChannel, &pOverlapped, WAIT_TIME); //key 用地址的指针
																																		 //取出的结构里包含了投递这个请求时传入的信息，利用t_info知道是哪一个请求已经完成了
		if (bReturn) {
			pInfo = (POVERLAPPEDPLUS)pOverlapped;
			//这里的标志都是表示 已完成 的操作 
			pChannel->postEvent(pInfo, dwBytesTransfered);
		}
		else
			WRONG << WSAGetLastError();
	}
}


