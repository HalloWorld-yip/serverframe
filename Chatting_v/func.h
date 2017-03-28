#pragma once
#include<string.h>
#include<algorithm>
#include"key.h"
#include"global.h"
#include"structure.h"
#include"log.h"
extern "C"
{
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"  
#include "SDL.h"  
};

#pragma comment(lib, "swresample.lib")
bool findname(net_clt* p);

bool findip(net_clt* p);

net_clt* search_in_list(const char* name);

net_clt* searchip_in_list(ULONG ip);

void add_client(const char* name,ULONG ip,int status=STATUS_ALIVE);


 bool W2C(const wchar_t* wstr,char* str);

 bool C2W(const char* str,wchar_t* wstr);

 void GetTime(char* str);

 //ÖØ²ÉÑù
 void audio_swr_resampling_audio_init(SwrContext **swr_ctx, AVSampleFormat ifmt, AVSampleFormat ofmt);
 int audio_swr_resampling_audio(SwrContext *swr_ctx, uint8_t **srcbuf, AVFrame **dstFrame, AVSampleFormat ofmt);
 void audio_swr_resampling_audio_destory(SwrContext **swr_ctx);