#pragma once
#include <stdio.h>  

#define __STDC_CONSTANT_MACROS  

//Windows  
extern "C"
{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h"  
#include "libswscale/swscale.h"  
#include "libavutil/imgutils.h"  
#include "libavdevice/avdevice.h"  
#include "SDL.h"  
};
#include"v_Encoder.h"
#include"v_SDLwrapper.h"
void show_dshow_device_option();

class Recorder {
private:
	AVFormatContext		*m_pFormatCtxV, *m_pFormatCtxA;
	int					m_videoindex, m_framepts;
	AVCodecContext		*m_pCodecCtxV, *m_pCodecCtxA;
	AVCodec				*m_pCodecV;
	AVInputFormat		*m_ifmt;
	AVFrame				*m_pFrameV, *m_pFrameA, *m_pFrameYUV, *m_pFrameLOC;
	AVPacket			*m_packetV, *m_packetA;
	struct SwsContext	*m_img_convert_ctx;
	struct SwsContext	*m_local_ctx;
	int					m_localw;
	int					m_localh;
	SDL2Wrapper			*m_pSdlwrapper;

	void prepare_struct();
public:
	Recorder(int w, int h, SDL2Wrapper *psdl, const void* gwin = 0);
	Recorder();
	~Recorder();
	int			StartRecordV();
	int			StartRecordA();
	int			OutputH26Pkg(YUVEncoder* yuvEn, uint8_t* dstbuf);
	int			OutputAACFrm(YUVEncoder* yuvEn, AVFrame** dstfrm);

	int			GetWidth()const { return m_pCodecCtxV->width; }
	int			GetHeight()const { return m_pCodecCtxV->height; }
	bool		IsSamestream()const { return (m_packetV) ? m_packetV->stream_index == m_videoindex : 0; }
};

