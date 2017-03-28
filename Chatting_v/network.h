#pragma once
#include"log.h"
#include"key.h"
#include"global.h"
#include"func.h"
#include<WinSock2.h>

#pragma comment(lib,"ws2_32.lib")


class network_dealer {
private:
	WSADATA wd;
	//�㲥
	SOCKET s_rv_b;
	//����
	SOCKET s_sd,s_rv;
	//��Ƶ
	SOCKET s_sd_v, s_rv_v,s_sd_a,s_rv_a;
	//tcp����
	//SOCKET	s_tcp_ac, s_tcp;
	//b �㲥��ַ l ������ַ
	SOCKADDR_IN saddr_a,saddr_v,saddr_bind_b,saddr_bind_l;

	void set_saddr(SOCKADDR_IN* sa,ULONG addr,int port);

	void resolve_msg(net_pkg *pkg,net_msg* msg);

	int init();
public:
	network_dealer() ;
	~network_dealer();

	int broadcast(int sgn);

	int broadcast(ULONG ip,int sgn);
	//�ı���video�ظ�
	int udpsend(ULONG ip,const char *str,int sgn);
	//���ӡ�ȷ�ϡ�����
	int udpsend(ULONG ip,int sgn);
	//���ݰ�
	int udpsend(ULONG ip, int size, const char* data, int pts, int sgn);
	
	int udpsend(SOCKET s,SOCKADDR_IN* sa,char*buf,int size);

	int udprecv(SOCKET s,net_msg* msg);
	//���ݰ�
	int udprecv(net_msg* msg,bool video);

	//int tcpsend(int size, const char* data);

	//int tcprecv(net_msg* msg);

	//int tcpconnect(ULONG ip);

	//int tcpaccept();

	//void tcpend() { closesocket(s_tcp); }

	void clear_recvbuf();

	friend UINT recv_msg( LPVOID pParam );
	friend UINT video_recv(LPVOID pParam);
	friend UINT audio_recv(LPVOID pParam);
	friend UINT	wait_stop(LPVOID pParam);
};

