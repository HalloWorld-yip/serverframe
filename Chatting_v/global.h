#pragma once
#include<vector>
#include"structure.h"
using std::string;

#ifdef GLOBAL_CPP
#define EXTERN
#else
#define EXTERN extern
#endif
//�ͻ��б�
EXTERN std::vector<net_clt*> clt_list;

//����Ѱ���б��еĿͻ�
EXTERN char		find_name[128];
EXTERN ULONG	find_ip;

//���ʮ���ƴ���IP��ַ
//EXTERN LPCSTR psz;

//������
EXTERN char hostname[NAMELEN];
EXTERN char targetname[NAMELEN];
EXTERN ULONG videoip;
//����״̬
EXTERN int	hoststatus;
//���ƽ���ѭ��
EXTERN bool	thread_recv;
EXTERN bool	thread_update;
EXTERN bool	thread_video;
EXTERN bool	thread_heartbeat;

//��ͼ
EXTERN bool capscreen;
EXTERN char capdir[256];