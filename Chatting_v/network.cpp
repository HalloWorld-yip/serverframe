#include"stdafx.h"
#include"network.h"
#include"global.h"


//#define INADDR_BROADCAST (inet_addr("172.18.103.255"))

network_dealer::network_dealer() {

	init();
	//非忙广播忽略sgn
	broadcast(0); 
	memset(targetname,0,sizeof(targetname));
}
network_dealer::~network_dealer() { 

	WSACleanup();

}



void network_dealer::set_saddr(SOCKADDR_IN* sa,ULONG addr,int port){

	sa->sin_family			= AF_INET;
	sa->sin_addr.s_addr		= addr;
	sa->sin_port			= htons(port);
}

void network_dealer::resolve_msg(net_pkg *pkg,net_msg* msg){
	int i;
	msg->sgn = pkg->sgn;
	msg->pts = pkg->pts;
	for(i = 0;pkg->data[i]!=':';i++){
		msg->name[i] = pkg->data[i];
	}
	strcpy(msg->buf,pkg->data+i+1);
	
	msg->size	 -= sizeof(int)+strlen(msg->name+1);
}

int network_dealer::udpsend(ULONG ip,const char *str,int sgn)//text
{
	char		buf[UDPBUF+2*sizeof(int)];
	net_pkg*	pkg = (net_pkg*)buf;
	SOCKADDR_IN sa;
	
	pkg->sgn		= sgn;
	set_saddr(&sa,ip,LOCAL_PORT+1);
	sprintf(pkg->data,"%s:%s",hostname,str);

	return udpsend(s_sd,&sa,buf,2*sizeof(int)+strlen(pkg->data)+1);
}

int network_dealer::udpsend(ULONG ip,int sgn)//cmd
{

	char	buf[UDPBUF + 2 * sizeof(int)];
	net_pkg *pkg = (net_pkg*)buf;
	SOCKADDR_IN sa;
	//strcpy(pkg->name,hostname);
	pkg->sgn = sgn;
	pkg->pts = 0;
	set_saddr(&sa,ip,LOCAL_PORT+1);
	sprintf(pkg->data,"%s:",hostname);
	return udpsend(s_sd,&sa,buf,2*sizeof(int)+strlen(pkg->data));
}

int network_dealer::udpsend(ULONG ip,int size,const char* data, int pts,int sgn)//a/v
{
	char		buf[UDPBUF+ 2 * sizeof(int)];
	net_pkg		*pkg = (net_pkg*)buf;
	SOCKADDR_IN sa;
	pkg->sgn = sgn;
	pkg->pts = pts;
	set_saddr(&sa, ip, (sgn==SIGNAL_VIDEO)?(VIDEO_PORT+1):(AUDIO_PORT+1));
	if(sgn == SIGNAL_VIDEO|| sgn == SIGNAL_AUDIO)
		memcpy(pkg->data, data, size);

	return udpsend((sgn == SIGNAL_VIDEO) ? s_sd_v:s_sd_a, &sa, buf, 2*sizeof(int)+size);
}

int network_dealer::udpsend(SOCKET s,SOCKADDR_IN* sa,char*buf,int size)
{
	if(SOCKET_ERROR == sendto(s,buf,size,0,(SOCKADDR*)sa,sizeof(SOCKADDR)))
	{
		Log("send",WSAGetLastError());
		return -1;
	}
	
	return 0;
}



int network_dealer::udprecv(SOCKET s,net_msg* msg){
	char buf[UDPBUF];
	msg->size = sizeof(sockaddr);
	int recvnum;
	if ((recvnum = recvfrom(s, buf, sizeof(buf), 0, (sockaddr*)&msg->saddr, &msg->size)) == SOCKET_ERROR) {
		Log("recv", WSAGetLastError());
		return -1;
	}
	msg->size = recvnum - sizeof(int);
	net_pkg* pkg = (net_pkg*)buf;

	//解包
	resolve_msg(pkg,msg);
	//	if (strcmp(hostname, msg.name) != 0)
	//	{
	//广播忙
/*
	if (strcmp(msg->name, hostname) == 0)
		return 0;
	*/

	add_client(msg->name, msg->saddr.sin_addr.S_un.S_addr, STATUS_ALIVE);
	//	}
	return 0;
}

int network_dealer::udprecv(net_msg* msg,bool video)
{
	char buf[UDPBUF+2*sizeof(int)];
	msg->size = sizeof(sockaddr);
	int recvnum;
	if ((recvnum = recvfrom(video?s_rv_v:s_rv_a, buf, sizeof(buf), 0, (sockaddr*)&msg->saddr, &msg->size)) == SOCKET_ERROR) {
		Log("recv", WSAGetLastError());
		return -1;
	}
	msg->size = recvnum-2*sizeof(int);
	net_pkg* pkg = (net_pkg*)buf;
	msg->pts = pkg->pts;
	msg->sgn = pkg->sgn;
	memcpy(msg->buf, pkg->data, msg->size);

	return 0;
}

//int network_dealer::tcpsend(int size, const char* data)
//{
//	return send(s_tcp, data, size, 0);
//}
//
//
//int network_dealer::tcprecv(net_msg* msg)
//{
//	if ((msg->size = recv(s_tcp, msg->buf, UDPBUF, 0)) == SOCKET_ERROR)
//	{
//		Log("recv", WSAGetLastError());
//		return -1;
//	}
//
//	return msg->size;
//}
//
//int network_dealer::tcpconnect(ULONG ip)
//{
//	fd_set		wtfds;
//	timeval		_timout;
//	SOCKADDR_IN	sa;
//	ULONG		opt = 1;
//	int			ret;
//	FD_ZERO(&wtfds);
//	_timout.tv_sec = 10;
//	_timout.tv_usec = 0;
//
//	s_tcp = socket(AF_INET, SOCK_STREAM, 0);
//	set_saddr(&sa, ip, VIDEO_PORT+1);
//
//	if (ioctlsocket(s_tcp, FIONBIO, &opt) < 0)
//	{
//		Log("ioctlsocket error\n");
//	}
//	FD_SET(s_tcp, &wtfds);
//
//	ret = connect(s_tcp, (SOCKADDR*)&sa, sizeof(sa));
//	if (!ret)
//	{
//		return 0;
//	}
//	else if (ret<0 && WSAGetLastError() == WSAEWOULDBLOCK)
//	{
//		int iRet1 = select(0, NULL, &wtfds, NULL, &_timout);
//		if (iRet1 < 0)
//		{
//			Log("connect error\n");
//			return -1;
//		}
//		else if (!iRet1)
//		{
//			Log("timeout error\n");
//			return -1;
//		}
//		else
//		{
//			if (FD_ISSET(s_tcp, &wtfds))
//			{
//				return 0;
//			}
//		}
//	}
//}
//
//int network_dealer::tcpaccept()
//{
//	SOCKADDR_IN	sa;
//	int			len = sizeof(sa);
//	s_tcp = accept(s_tcp_ac, (sockaddr*)&sa, &len);
//	if (SOCKET_ERROR == s_tcp)
//	{
//		Log("accept");
//		return -1;
//	}
//	return 0;
//}

int network_dealer::broadcast(int sgn) {

	return broadcast(INADDR_BROADCAST, sgn);

}
int network_dealer::broadcast(ULONG ip, int sgn)
{
	SOCKADDR_IN saddr;
	SOCKET		s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bool		bOpt = true;
	char		buf[UDPBUF];
	net_pkg		*pkg = (net_pkg*)buf;

	if (INVALID_SOCKET == s) {
		Log("socket", WSAGetLastError());
		return -1;
	}
	setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
	set_saddr(&saddr, ip, BROADCAST_PORT+1);

	//主机名和IP封包
	pkg->sgn = sgn;
	sprintf(pkg->data, "%s:", hostname);
	// 发送数据  
	int ret = udpsend(s, &saddr, buf, 2*sizeof(int) + strlen(pkg->data));
	closesocket(s);
	return ret;
}


void network_dealer::clear_recvbuf()
{
	fd_set			rfd;
	timeval			timeout;
	net_msg			msg;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	while (1)
	{
		FD_ZERO(&rfd);
		FD_SET(s_rv_v, &rfd);
		FD_SET(s_rv_a, &rfd);
		int lRet = select(FD_SETSIZE, &rfd, NULL, NULL, &timeout);
		if (lRet == 0)
			break;
		udprecv(&msg,0);
		udprecv(&msg,1);
	}
}

int network_dealer::init() {
	WORD wVR = MAKEWORD(2, 2);
	if (WSAStartup(wVR, &wd) != 0) {
		Log("start up.",WSAGetLastError());
		return -1;
	}
	
	bool bOpt = true;
	ULONG opt = 1;

	//初始化广播结构
	s_rv_b	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == s_rv_b) {
		Log("socket s_rv_b", WSAGetLastError());
		return -1;
	}
	//设置广播
	setsockopt(s_rv_b, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
	//广播bind地址
	set_saddr(&saddr_bind_b,0,BROADCAST_PORT);
	if (0 != bind(s_rv_b, (const sockaddr*)&saddr_bind_b, sizeof(sockaddr_in))) {
		Log("bind", WSAGetLastError());
	}


	//设置单播
	s_rv	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	s_sd	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == s_rv) {
		Log("socket s_rv", WSAGetLastError());
		return -1;
	}
	if (INVALID_SOCKET == s_sd) {
		Log("socket s_sd", WSAGetLastError());
		return -1;
	}	
	//单播bind地址
	set_saddr(&saddr_bind_l, INADDR_ANY, LOCAL_PORT);
	if (0 != bind(s_rv, (const sockaddr*)&saddr_bind_l, sizeof(sockaddr_in))) {
		Log("bind", WSAGetLastError());
		return -1;
	}


	//视频
	s_rv_v	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	s_sd_v	= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//s_tcp_ac = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == s_rv_v) {
		Log("socket s_rv", WSAGetLastError());
		return -1;
	}
	ioctlsocket(s_rv_v, FIONBIO, (unsigned long*)&opt);
	if (INVALID_SOCKET == s_sd_v) {
		Log("socket s_sd", WSAGetLastError());
		return -1;
	}
	ioctlsocket(s_sd_v, FIONBIO, (unsigned long*)&opt);
	//if (INVALID_SOCKET == s_tcp_ac){
	//	Log("socket s_tcp_ac", WSAGetLastError());
	//	return -1;
	//}
	//ioctlsocket(s_tcp_ac, FIONBIO, (unsigned long*)&opt);
	//单播video地址
	/*set_saddr(&saddr_v, INADDR_ANY, VIDEO_PORT);
	if (0 != bind(s_tcp_ac, (const sockaddr*)&saddr_v, sizeof(saddr_v))) {
		Log("bind", WSAGetLastError());
	}
	if (SOCKET_ERROR == listen(s_tcp_ac, 2))
	{
		Log("listen error");
		return -1;
	}*/
	int bufopt = 65536;
	if (SOCKET_ERROR == setsockopt(s_rv_v, SOL_SOCKET, SO_RCVBUF, (char*)&bufopt, sizeof(bufopt)))
	{
		bufopt = WSAGetLastError();
		return -1;
	}
	if (SOCKET_ERROR == setsockopt(s_sd_v, SOL_SOCKET, SO_SNDBUF, (char*)&bufopt, sizeof(bufopt)))
	{
		bufopt = WSAGetLastError();
		return -1;
	}
	set_saddr(&saddr_v, INADDR_ANY, VIDEO_PORT);
	if (0 != bind(s_rv_v, (const sockaddr*)&saddr_v, sizeof(saddr_v))) {
		Log("bind", WSAGetLastError());
		return -1;
	}

	//音频
	s_rv_a = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	s_sd_a = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == s_rv_a) {
		Log("socket s_rv", WSAGetLastError());
		return -1;
	}
	ioctlsocket(s_rv_a, FIONBIO, (unsigned long*)&opt);
	if (INVALID_SOCKET == s_sd_a) {
		Log("socket s_sd", WSAGetLastError());
		return -1;
	}
	bufopt = 65536;
	if (SOCKET_ERROR == setsockopt(s_rv_a, SOL_SOCKET, SO_RCVBUF, (char*)&bufopt, sizeof(bufopt)))
	{
		bufopt = WSAGetLastError();
		return -1;
	}
	if (SOCKET_ERROR == setsockopt(s_sd_a, SOL_SOCKET, SO_SNDBUF, (char*)&bufopt, sizeof(bufopt)))
	{
		bufopt = WSAGetLastError();
		return -1;
	}
	set_saddr(&saddr_a, INADDR_ANY, AUDIO_PORT);
	if (0 != bind(s_rv_a, (const sockaddr*)&saddr_a, sizeof(saddr_v))) {
		Log("bind", WSAGetLastError());
		return -1;
	}





	//获取本机名
	if (0 > gethostname(hostname, 128)) {
		Log("gethostname");
		return -1;
	}
	//struct hostent* pHost;//获得本地IP地址  
	//pHost = gethostbyname(hostname);//pHost返回的是指向主机的列表  
	//for (int i = 0; pHost != NULL&&pHost->h_addr_list[i] != NULL; i++)
	//	psz = inet_ntoa(*(struct in_addr *)pHost->h_addr_list[i]);//得到指向ip的psz变量  
	//

	return 0;
}




