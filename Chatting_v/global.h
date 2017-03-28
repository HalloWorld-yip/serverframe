#pragma once
#include<vector>
#include"structure.h"
using std::string;

#ifdef GLOBAL_CPP
#define EXTERN
#else
#define EXTERN extern
#endif
//客户列表
EXTERN std::vector<net_clt*> clt_list;

//用来寻找列表中的客户
EXTERN char		find_name[128];
EXTERN ULONG	find_ip;

//点分十进制储存IP地址
//EXTERN LPCSTR psz;

//主机名
EXTERN char hostname[NAMELEN];
EXTERN char targetname[NAMELEN];
EXTERN ULONG videoip;
//主机状态
EXTERN int	hoststatus;
//控制进程循环
EXTERN bool	thread_recv;
EXTERN bool	thread_update;
EXTERN bool	thread_video;
EXTERN bool	thread_heartbeat;

//截图
EXTERN bool capscreen;
EXTERN char capdir[256];