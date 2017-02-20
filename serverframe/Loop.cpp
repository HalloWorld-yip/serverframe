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
		//һ��epoll���ܻ�ö���¼�����һ��GetQueuedCompletionStatusֻ��Ӧһ���¼���ֱ�Ӵ���
		BOOL bReturn = GetQueuedCompletionStatus(m_IOCPport, &dwBytesTransfered, (PULONG_PTR)&pChannel, &pOverlapped, WAIT_TIME); //key �õ�ַ��ָ��
																																		 //ȡ���Ľṹ�������Ͷ���������ʱ�������Ϣ������t_info֪������һ�������Ѿ������
		if (bReturn) {
			pInfo = (POVERLAPPEDPLUS)pOverlapped;
			//����ı�־���Ǳ�ʾ ����� �Ĳ��� 
			pChannel->postEvent(pInfo, dwBytesTransfered);
		}
		else
			WRONG << WSAGetLastError();
	}
}


