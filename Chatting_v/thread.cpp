#include"stdafx.h"
#include"thread.h"
#include"structure.h"
#include"func.h"
#include"ChattingDlg.h"
#include"resource.h"
#include"v_Encoder.h"
#include"v_Record.h"
#include"v_H264player.h"
#include"v_SDLwrapper.h"
//#include"vld.h"

#define M_DEBUG

void close_video(CChattingDlg* dlg);

bool is_connected(CChattingDlg *dlg); 

void fst_close_video(CChattingDlg* dlg);

void bend_close_video(CChattingDlg* dlg);

static int beclosed = 0;

UINT update_list( LPVOID pParam )
{
	CChattingDlg* dlg = (CChattingDlg*)pParam;
	char  buf[129];
	wchar_t name[128];
	CListBox *pLbox = (CListBox*)dlg->GetDlgItem(IDC_LIST1);

	while (thread_update) {
		int j = 0;
		pLbox->ResetContent();
		for (auto iter : clt_list)
		{
			if (iter->life == 0)
			{
				dlg->SetStatus(iter, STATUS_DEAD);
				continue;
			}
			if (iter->status & STATUS_UNREAD)
			{
				sprintf(buf, "*%s", iter->name);
				C2W(buf, name);
			}
			else
			{
				C2W(iter->name, name);
			}
			pLbox->InsertString(j++, name);
		}

		if (!j)
		{
			dlg->GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
			dlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
		}

		Sleep			(1000);
	}
	return 0;
}

UINT recv_msg( LPVOID pParam ){
	CChattingDlg* dlg = (CChattingDlg*)pParam;

	timeval		timeout;    // 定时变量  
	net_msg		msg;
	fd_set		rfd; 
	int			nfds;

	timeout.tv_sec	= 6;   
    timeout.tv_usec = 0;  
	while (thread_recv)  
    {  
        FD_ZERO(&rfd);
		memset(&msg,0,sizeof(msg));
        // 开始使用select  
		FD_SET(dlg->m_ndealer.s_rv, &rfd); // 单播  
		FD_SET(dlg->m_ndealer.s_rv_b, &rfd); //广播 
		//FD_SET(dlg->ndealer.s_tcp_ac, &rfd); // 
          
        int nRet = select(0, &rfd, NULL, NULL, &timeout);
        if (nRet == SOCKET_ERROR)     
        {  
			Log("select",WSAGetLastError());
            return -1;  
        }  
        else if (nRet == 0) // 超时  
        {  
			continue;
        }  
        else    // 可读  
        {  
            if (FD_ISSET(dlg->m_ndealer.s_rv, &rfd))  // 单播可读  
            {  
				dlg->m_ndealer.udprecv(dlg->m_ndealer.s_rv,&msg);
				switch (msg.sgn)
				{
				case SIGNAL_TEXT:
					dlg->HandleMsg(&msg); break;
				case SIGNAL_HEARDBEAT:
					dlg->HandleHeartbeats(msg.name); break;
				case SIGNAL_HEARDBEATACK:
					search_in_list(msg.name)->life = 3; break;
				case SIGNAL_VIDEOASK:
					dlg->HandleAsk(&msg); break;
				case SIGNAL_VIDEOACK:
					dlg->HandleAck(&msg); break;
				case SIGNAL_RE://对端要求重传
					dlg->m_ndealer.udpsend(msg.saddr.sin_addr.s_addr, "0", SIGNAL_VIDEOACK); break;
				case SIGNAL_END:
					break;
				default:
					break;
				}
            }  
            if (FD_ISSET(dlg->m_ndealer.s_rv_b, &rfd)) // 广播 可读  
            {  
				dlg->m_ndealer.udprecv(dlg->m_ndealer.s_rv_b,&msg);
#ifndef   M_DEBUG
				if (strcmp(msg.name, hostname) == 0)
					continue;
#endif
				switch (msg.sgn)
				{
				case STATUS_BUSY:
					dlg->SetStatus(msg.name, STATUS_BUSY); break;
				default:
					dlg->SetStatus(msg.name, STATUS_ALIVE);
					break;
				}
            }  
			/*if (FD_ISSET(dlg->ndealer.s_tcp_ac, &rfd)&&hoststatus == STATUS_ALIVE)
			{
				dlg->ndealer.tcpaccept();
			}*/
        }  
    }  
	
	return 0;
}
 
UINT video_send(LPVOID pParam) {
	CChattingDlg	*dlg = (CChattingDlg*)pParam;

	if (!is_connected(dlg))
	{
		//close_video(dlg);
		end_thread_vc();
		return -1;
	}


	char* dstbuf = new char[MAX_BUFSIZE];
	int		bsize;
	int		pkgnum;
	unsigned sendcount = 0;
	CRect	rect;
	dlg->m_video->GetDlgItem(IDC_STATIC1)->GetWindowRect(&rect);
	if (!dlg->m_wVideo)
	{
		dlg->MessageBox(L"!");
	}
	Recorder rec(rect.Width(), rect.Height(),&dlg->m_sdl2, dlg->m_lVideo);
	if (rec.StartRecordV() < 0) {
		Log("record start fail.");
		//close_video(dlg);
		end_thread_vc();
		return -1;
	}

	YUVEncoder yuvEn(rec.GetWidth(), rec.GetHeight());
	if (!yuvEn.is_init()) {
		Log("yueencode failed.\n");
		//close_video(dlg);
		end_thread_vc();
		return -1;
	}

	while (thread_video) {

		bsize = rec.OutputH26Pkg(&yuvEn, (uint8_t*)dstbuf);
		if (!bsize)
			continue;
		pkgnum = bsize / UDPBUF + bool(bsize%UDPBUF);

		for (int i = 0; i < pkgnum - 1; i++)
		{

			//dlg->ndealer.tcpsend(UDPBUF, dstbuf + i*UDPBUF);
			dlg->m_ndealer.udpsend(videoip, UDPBUF, dstbuf + i*UDPBUF, sendcount, SIGNAL_VIDEO);
			//Sleep(35);
		}
		
		//dlg->ndealer.tcpsend(bsize-(pkgnum-1)*UDPBUF, dstbuf + (pkgnum - 1)*UDPBUF);
		dlg->m_ndealer.udpsend(videoip, bsize - (pkgnum - 1)*UDPBUF, dstbuf + (pkgnum - 1)*UDPBUF,sendcount, SIGNAL_VIDEO);
		sendcount += 1500;
		Sleep(40);
	}
	//close_video(dlg);
	delete dstbuf;
	return 0;
}
 
UINT video_recv(LPVOID pParam)
{
	CChattingDlg	*dlg = (CChattingDlg*)pParam;

	if (!is_connected(dlg))
	{
		//close_video(dlg);
		end_thread_vc();
		return -1;
	}

	timeval	timeout;
	net_msg	msg;
	fd_set	rfd;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if (!dlg->m_wVideo)
	{
		dlg->MessageBox(L"!");
	}
	H264player hplayer(0);
	int recvcount=0;
	while (thread_video) {
		FD_ZERO(&rfd);
		memset(&msg, 0, sizeof(msg));
		// 开始使用select  
		//FD_SET(dlg->ndealer.s_tcp, &rfd);
		FD_SET(dlg->m_ndealer.s_rv_v, &rfd);

		//int nRet = select(dlg->ndealer.s_tcp, &rfd, NULL, NULL, &timeout);
		int nRet = select(dlg->m_ndealer.s_rv_v, &rfd, NULL, NULL, &timeout);
		if (nRet == SOCKET_ERROR)
		{
			Log("select", WSAGetLastError());
			//close_video(dlg);
			end_thread_vc();
			return -1;
			
		}
		else if (nRet == 0) // 超时  
		{
			//close_video(dlg);
			end_thread_vc();
			while (hplayer.flush(&dlg->m_sdl2) > 0);
			return -1;
		}
		else    // 可读  
		{
			//dlg->ndealer.tcprecv(&msg);
			dlg->m_ndealer.udprecv(&msg,1);

			recvcount++;
			if(msg.sgn == SIGNAL_VIDEO){
				hplayer.transform_and_play((uint8_t*)msg.buf, msg.size, &dlg->m_sdl2);
			}
			else if(msg.sgn == SIGNAL_END)//连接断开
			{
				//连接断开同时按下 bug
				//close_video(dlg);
				beclosed = 1;
				end_thread_vc();
				return -1;
			}
		}
	}

	while (hplayer.flush(&dlg->m_sdl2) > 0);
	
	//close_video(dlg);
	end_thread_vc(); 
}


UINT audio_send(LPVOID pParam)
{
	CChattingDlg	*dlg = (CChattingDlg*)pParam;
	if (!is_connected(dlg))
	{
		//close_video(dlg);
		end_thread_vc();
		return -1;
	}

	AVFrame *pframe;

	pframe = av_frame_alloc();
	unsigned int sendcount = 0;
	Uint32 _FPS_Timer = 0;
	Uint32 _FPS_Accul = 0;
	//S16 2 FLTP
	SwrContext *pSwrContextE = NULL;
	audio_swr_resampling_audio_init(&pSwrContextE, AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_FLTP);

	Recorder rec;
	if (0 > rec.StartRecordA())
	{
		end_thread_vc();
		return -1;
	}
	YUVEncoder yuvEn;

	while (thread_video)
	{

		rec.OutputAACFrm(&yuvEn, &pframe);
		int alen = pframe->pkt_size;
		uint8_t* pcmbuf = pframe->data[0];
		uint8_t	 sendbuf[4096];
		AVFrame *FLTPframe = NULL, *S16frame = NULL;


		while (alen > 0)
		{
			//-----------------------------------------------------------------------------
			//一帧过大要分成1024小帧

			//dlg->m_sdl2.PlayOneFrame(pcmbuf, alen>4096?4096:alen);
			////定时降低延迟
			//Uint32 thistime = SDL_GetTicks();
			//if (thistime - _FPS_Timer<23) {
			//	//SDL_Delay(500 - SDL_GetTicks() + _FPS_Timer);
			//}
			//else {
			//	_FPS_Accul += thistime - _FPS_Timer - 23;
			//	if (_FPS_Accul > 50)
			//	{
			//		_FPS_Accul -= 23;
			//		_FPS_Timer = SDL_GetTicks();
			//		sendcount += 1024;
			//		continue;
			//	}

			//}
			uint8_t *pbuf[8];
			pbuf[0] = pcmbuf;
			audio_swr_resampling_audio(pSwrContextE, pbuf, &FLTPframe, AV_SAMPLE_FMT_FLTP);
			//ENCODE
			/*-------------------------------------------------*/
			int pktsize = yuvEn.Encode1FrmA(FLTPframe, sendbuf);
			/*if (pktsize <= 0)
				continue;*/
			dlg->m_ndealer.udpsend(videoip, pktsize, (const char*)sendbuf, sendcount, SIGNAL_AUDIO);

			sendcount += 1024;
			//-----------------------------------------------------------------------------
			//send

			alen -= 4096;
			pcmbuf += 4096;

		}
		av_frame_free(&S16frame);


	}
	av_frame_free(&pframe);

	audio_swr_resampling_audio_destory(&pSwrContextE);
	return 0;
}
UINT audio_recv(LPVOID pParam)
{
	CChattingDlg	*dlg = (CChattingDlg*)pParam;

	timeval	timeout;
	net_msg	msg;
	fd_set	rfd;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	int flag = 0;
	Uint32 _FPS_Timer = 0;
	Uint32 _FPS_Accul = 0;

	H264player hplayer(1);

	int recvcount = 0;
	while (thread_video) {
		FD_ZERO(&rfd);
		// 开始使用select  
		FD_SET(dlg->m_ndealer.s_rv_a, &rfd);

		//int nRet = select(dlg->ndealer.s_tcp, &rfd, NULL, NULL, &timeout);
		int nRet = select(dlg->m_ndealer.s_rv_a, &rfd, NULL, NULL, &timeout);
		if (nRet == SOCKET_ERROR)
		{

			return -1;

		}
		else if (nRet == 0) // 超时  
		{
			//close_video(dlg);
			return -1;
		}
		else    // 可读  
		{
			char buf[4096 + 2 * sizeof(int)];
			int size = sizeof(sockaddr);
			int recvnum;
			SOCKADDR_IN saddr;
			if ((recvnum = dlg->m_ndealer.udprecv(&msg,0)) == SOCKET_ERROR) {
				printf("recv:%d", WSAGetLastError());
				return -1;
			}
			net_pkg *pkg = (net_pkg*)buf;

			if (flag&&_FPS_Accul > 100)
			{
				//cur_pts_a = msg.pts;
				_FPS_Accul -= 23;
				_FPS_Timer = SDL_GetTicks();
				continue;
			}
			else
				flag = 0;

			recvcount++;
			//定时降低延迟
			Uint32 thistime = SDL_GetTicks();
			if (_FPS_Timer != 0 && thistime - _FPS_Timer>23) {
				_FPS_Accul += thistime - _FPS_Timer - 23;

			}
			_FPS_Timer = SDL_GetTicks();
			if (_FPS_Accul > 500)
			{
				flag = 1;
				_FPS_Accul -= 23;
				//cur_pts_a = msg.pts;
				continue;
			}
			if (hplayer.transform_and_play((uint8_t*)pkg->data, recvnum - 2 * sizeof(int), &dlg->m_sdl2)>0) {
				//cur_pts_a = msg.pts;
			}


		}
	}
	return 1;
}

UINT check_heartbeats( LPVOID pParam )
{

	CChattingDlg* dlg = (CChattingDlg*)pParam;

	while (thread_heartbeat)
	{
		for (auto iter : clt_list)
		{
			if (iter->life > 0)
			{
				iter->life--;
				dlg->m_ndealer.udpsend(iter->ip, SIGNAL_HEARDBEAT);
			}
		}
		Sleep(3000);
	}
	return 0;
}


UINT wait_stop(LPVOID pParam)
{
	CChattingDlg	*dlg = (CChattingDlg*)pParam;

#ifndef M_DEBUG
	WaitForMultipleObjects(2, dlg->m_hthread + 3, true, INFINITE);
#else
	WaitForSingleObject(dlg->m_hthread[3], INFINITE);
#endif // !M_DEBUG

	dlg->m_sdl2.DestroyWindowR();

	dlg->m_ndealer.clear_recvbuf();

	close_video(dlg);
	return 0;
}

UINT search_ip(LPVOID pParam)
{
	CChattingDlg* dlg	= (CChattingDlg*)pParam;
	dlg->m_startip		= ntohl(dlg->m_startip);
	dlg->m_endip		= ntohl(dlg->m_endip);

	if (dlg->m_endip - dlg->m_startip > MAX_SEARCHIP)
		return -1;

	for (ULONG i = dlg->m_startip+1; i < dlg->m_endip; i = i + 1)
	{
		dlg->m_ndealer.udpsend(htonl(i), SIGNAL_HEARDBEAT);
	}
	return 0;
}

UINT time_out(LPVOID pParam)
{
	CChattingDlg* dlg = (CChattingDlg*)pParam;
	while (true)
	{
		Sleep(4000);
		if (hoststatus == STATUS_BUSY)
			return 0;
		if (IDYES == dlg->MessageBoxW(L"对方无应答，是否继续等待", 0, MB_YESNO | MB_DEFBUTTON1))
		{
			if (hoststatus != STATUS_WAITING)
				return 0;
		}
		else
		{
			hoststatus = STATUS_ALIVE;
			return 0;
		}
	}
}

void start_thread_ul(CChattingDlg* pdlg)
{
	thread_update = true;
	pdlg->m_wthread[0] = AfxBeginThread(update_list, pdlg);
	pdlg->m_hthread[0] = pdlg->m_wthread[0]->m_hThread;
}
void end_thread_ul()
{
	thread_update = false;
}

void start_thread_rm(CChattingDlg* pdlg)
{
	thread_recv = true;
	pdlg->m_wthread[1] = AfxBeginThread(recv_msg, pdlg);
	pdlg->m_hthread[1] = pdlg->m_wthread[1]->m_hThread;
}
void end_thread_rb()
{
	thread_recv = false;
}

void start_thread_vc(CChattingDlg* pdlg)
{
	thread_video = true;

	pdlg->m_sdl2.InitWindowR(VIDEO_W, VIDEO_H, (void *)pdlg->m_wVideo);//在wait_stop中销毁
	pdlg->m_ndealer.clear_recvbuf();
	pdlg->m_wthread[3] = AfxBeginThread(video_send, pdlg);
	pdlg->m_hthread[3] = pdlg->m_wthread[3]->m_hThread;
	pdlg->m_wthread[5] = AfxBeginThread(audio_send, pdlg);
	pdlg->m_hthread[5] = pdlg->m_wthread[5]->m_hThread;
#ifndef M_DEBUG
	pdlg->m_wthread[4] = AfxBeginThread(video_recv, pdlg);
	pdlg->m_hthread[4] = pdlg->m_wthread[4]->m_hThread;
	pdlg->m_wthread[6] = AfxBeginThread(audio_recv, pdlg);
	pdlg->m_hthread[6] = pdlg->m_wthread[5]->m_hThread;
#endif // !
	AfxBeginThread(wait_stop, pdlg);

	/* --------------------------------- */
}
void end_thread_vc()
{
	thread_video = false;
}

void start_thread_ch(CChattingDlg* pdlg)
{

	thread_heartbeat = true;
	pdlg->m_wthread[2] = AfxBeginThread(check_heartbeats, pdlg);
	pdlg->m_hthread[2] = pdlg->m_wthread[2]->m_hThread;
}
void end_thread_ch()
{
	thread_heartbeat = false;
}





void close_video(CChattingDlg* dlg)
{
	if (!beclosed)
		fst_close_video(dlg);
	else
		bend_close_video(dlg);
	beclosed = 0;
}

void fst_close_video(CChattingDlg* dlg)
{
	hoststatus = STATUS_ALIVE;
	dlg->m_ndealer.udpsend(videoip, 0, 0, SIGNAL_END);
	videoip = 0;
	//thread_video = false;
	if (dlg->m_video) {
		dlg->m_video->SendMessageW(WM_CLOSE, 0, 0);
		//dlg->m_video->SendMessageW(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), NULL);
		//dlg->m_video->GetDlgItem(IDC_BUTTON3)->SetWindowTextW(L"连接中断，关闭视频");
		delete dlg->m_video;
		dlg->m_video = 0;
		dlg->m_wVideo = 0;
	}
	//dlg->ndealer.tcpend();
	CString str(L"\n -----------------视频关闭------------------- \n");
	dlg->AddText(str);

	dlg->m_ndealer.broadcast(0);
}
void bend_close_video(CChattingDlg* dlg)
{
	hoststatus = STATUS_ALIVE;
	//dlg->ndealer.udpsend(videoip, 0, 0, SIGNAL_VIDEOEND);
	videoip = 0;
	//thread_video = false;
	if (dlg->m_video) {
		dlg->m_video->SendMessageW(WM_CLOSE, 0, 0);
		//dlg->m_video->SendMessageW(WM_COMMAND, MAKEWPARAM(IDC_BUTTON3, BN_CLICKED), NULL);;
		delete dlg->m_video;
		dlg->m_video = 0;
		dlg->m_wVideo = 0;
	}
	//dlg->ndealer.tcpend();
	CString str(L"\n -----------------视频关闭------------------- \n");
	dlg->AddText(str);

	dlg->m_ndealer.broadcast(0);
}

bool is_connected(CChattingDlg *dlg)
{
	for (int i = 0; i < 9; i++)
	{
		if (hoststatus == STATUS_BUSY)
			return true;
		Sleep(1000);
	}
	return false;
}