#pragma once
#pragma warning(disable:4996)
#include <stdio.h>
#include"v_SDLWrapper.h"

#define in_buffer_size  4096
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include"libavformat/avformat.h"
};
class H264player;
typedef int(H264player::*pfunc_init[2])();
typedef int(H264player::*pfunc_decode[2])(uint8_t* , int, SDL2Wrapper*);
typedef int(H264player::*pfunc_flush[2])(SDL2Wrapper* sdl2);

class H264player {
private:
	AVFormatContext *m_pFormatCtxD;
	AVCodec *m_pCodecV,*pCodecA;
	AVCodecContext *m_pCodecCtxV, *m_pCodecCtxA;
	AVCodecParserContext *pCodecParserCtx;
	SwrContext *m_pSwrContextD;
	int frame_count;
	AVFrame	*pFrame, *pFrameYUV, *m_pS16frame, *m_pFLTPframe;
	uint8_t *out_buffer;
	//uint8_t in_buffer[in_buffer_size + FF_INPUT_BUFFER_PADDING_SIZE];
	uint8_t *cur_ptr;
	//int cur_size;
	AVPacket m_pktV, m_pktA;
	AVCodecID codec_id;
	int ret, got_picture;
	int y_size;
	int first_time;

	const int state;
	struct SwsContext *img_convert_ctx;

	int initV();
	int transform_and_playV(uint8_t* in_buffer, int cur_size, SDL2Wrapper* sdl2);
	int flushV(SDL2Wrapper* sdl2);	
	int initA();
	int transform_and_playA(uint8_t* in_buffer, int cur_size, SDL2Wrapper* sdl2);
	int flushA(SDL2Wrapper* sdl2);

	pfunc_init Init;
	pfunc_decode Decode;
	pfunc_flush  Flush;
public:
	H264player(int st);
	~H264player();
	int init();
	int transform_and_play(uint8_t* in_buffer, int cur_size, SDL2Wrapper* sdl2);
	int flush(SDL2Wrapper* sdl2);
};
