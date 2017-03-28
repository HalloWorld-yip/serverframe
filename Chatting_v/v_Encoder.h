#pragma once
#include <stdio.h>  
#include"key.h"
#pragma warning(disable:4996)
#define __STDC_CONSTANT_MACROS  

//  todo

extern "C"
{
#include "libavutil/opt.h"  
#include "libavcodec/avcodec.h"  
#include "libavutil/imgutils.h"  
};



class YUVEncoder {
public:
	YUVEncoder(int w,int h);
	YUVEncoder();
	//return size of dstbuf if success,or -1;
	int Encode1FrmV(AVFrame* psrc, uint8_t* dstbuf);
	//return size of dstbuf if success,or -1;
	int Flush_EncoderV(uint8_t* dstbuf);

	//return size of dstbuf if success,or -1;
	int Encode1FrmA(AVFrame* psrc, uint8_t* dstbuf);
	//return size of dstbuf if success,or -1;
	int Flush_EncoderA(uint8_t* dstbuf);

	bool is_init()const { return init_success; }
	~YUVEncoder();
private:
	AVCodec *m_pCodecV, *m_pCodecA;
	AVPacket m_pktV, m_pktA;
	AVCodecContext *m_pCodecCtxV, *m_pCodecCtxA;
	int m_framecntV, m_framecntA;
	int y_size;
	int in_w, in_h;
	bool init_success;
};
