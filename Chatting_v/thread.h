#include<windef.h>
#include"ChattingDlg.h"
 UINT update_list( LPVOID pParam );

 UINT recv_msg( LPVOID pParam );
 
 UINT video_send( LPVOID pParam );
 UINT video_recv(LPVOID pParam);

 UINT audio_send(LPVOID pParam);
 UINT audio_recv(LPVOID pParam);

 UINT check_heartbeats( LPVOID pParam );

 UINT time_out(LPVOID pParam);

 UINT search_ip(LPVOID pParam);//遍历局域网内所有IP

 void start_thread_ul(CChattingDlg* pdlg);
 void end_thread_ul();

 void start_thread_rm(CChattingDlg* pdlg);
 void end_thread_rb();

 void start_thread_vc(CChattingDlg* pdlg);
 void end_thread_vc();

 void start_thread_ch(CChattingDlg* pdlg);
 void end_thread_ch();