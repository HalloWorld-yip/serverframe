#include"stdafx.h"
#include"v_H264player.h"
#include"log.h"
#include"global.h"
#include"func.h"
//局部函数
void SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int bpp);

H264player::H264player(int st):state(st) {
	Init[0] = &H264player::initV;
	Init[1] = &H264player::initA;
	Decode[0] = &H264player::transform_and_playV;
	Decode[1] = &H264player::transform_and_playA;
	Flush[0] = &H264player::flushV;
	Flush[1] = &H264player::flushA;
	if (!st)
	{
		codec_id = AV_CODEC_ID_H264;
		first_time = 1;
	}
	else
	{
		av_register_all();
		avformat_network_init();
		//Register Device  
		avdevice_register_all();
		codec_id = AV_CODEC_ID_AAC;
	}
	init();
}
int H264player::initV() {
	avcodec_register_all();
	m_pCodecV = avcodec_find_decoder(codec_id);
	if (!m_pCodecV) {
		Log("Codec not found");
		return -1;
	}
	m_pCodecCtxV = avcodec_alloc_context3(m_pCodecV);
	if (!m_pCodecCtxV) {
		Log("Could not allocate video codec context\n");
		return -1;
	}
	pCodecParserCtx = av_parser_init(codec_id);
	if (!pCodecParserCtx) {
		Log("Could not allocate video parser context\n");
		return -1;
	}
	if (m_pCodecV->capabilities&CODEC_CAP_TRUNCATED)
		m_pCodecCtxV->flags |= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
	if (avcodec_open2(m_pCodecCtxV, m_pCodecV, NULL) < 0) {
		Log("Could not open codec\n");
		return -1;
	}
	pFrame = av_frame_alloc();
	av_init_packet(&m_pktV);
	return 0;
}

int H264player::transform_and_playV(uint8_t* in_buffer, int cur_size, SDL2Wrapper* sdl2) {
	if (cur_size == 0)
		return -1;
	cur_ptr = in_buffer;
	while (cur_size>0) {
		int len = av_parser_parse2(
			pCodecParserCtx, m_pCodecCtxV,
			&m_pktV.data, &m_pktV.size,
			cur_ptr, cur_size,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
		cur_ptr += len;
		cur_size -= len;
		if (m_pktV.size == 0)
			continue;
		//Some Info from AVCodecParserContext
		ret = avcodec_decode_video2(m_pCodecCtxV, pFrame, &got_picture, &m_pktV);
		if (ret < 0) {
			Log("Decode Error.(解码错误)\n");
			return ret;
		}
		if (got_picture) {
			if (first_time) {
				//SwsContext

				img_convert_ctx = sws_getContext(m_pCodecCtxV->width, m_pCodecCtxV->height, m_pCodecCtxV->pix_fmt,
					sdl2->GetWidthR(), sdl2->GetHeightR(), AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
				pFrameYUV = av_frame_alloc();
				out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, m_pCodecCtxV->width, m_pCodecCtxV->height));
				avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, m_pCodecCtxV->width, m_pCodecCtxV->height);
				y_size = m_pCodecCtxV->width*m_pCodecCtxV->height;
				first_time = 0;
			}
			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, m_pCodecCtxV->height,
				pFrameYUV->data, pFrameYUV->linesize);
			sdl2->PlayOneFrameR(pFrameYUV);
			//截图
			if (capscreen)
			{
				SwsContext *cap_convert_ctx = sws_getContext(m_pCodecCtxV->width, m_pCodecCtxV->height, m_pCodecCtxV->pix_fmt,
					m_pCodecCtxV->width, m_pCodecCtxV->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
				AVFrame *pFrameRGB = av_frame_alloc();
				uint8_t *cap_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_BGR24, m_pCodecCtxV->width, m_pCodecCtxV->height));
				avpicture_fill((AVPicture *)pFrameRGB, cap_buffer, AV_PIX_FMT_BGR24, m_pCodecCtxV->width, m_pCodecCtxV->height);

				pFrame->data[0] += pFrame->linesize[0] * (m_pCodecCtxV->height - 1);
				pFrame->linesize[0] *= -1;
				pFrame->data[1] += pFrame->linesize[1] * (m_pCodecCtxV->height / 2 - 1);
				pFrame->linesize[1] *= -1;
				pFrame->data[2] += pFrame->linesize[2] * (m_pCodecCtxV->height / 2 - 1);
				pFrame->linesize[2] *= -1;

				sws_scale(cap_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, m_pCodecCtxV->height, pFrameRGB->data, pFrameRGB->linesize);

				SaveAsBMP(pFrameRGB, m_pCodecCtxV->width, m_pCodecCtxV->height, 24);
				capscreen = 0;

				av_free(cap_buffer);
				av_frame_free(&pFrameRGB);
				sws_freeContext(cap_convert_ctx);
			}
		}
	}
	return 0;
}


int H264player::flushV(SDL2Wrapper* sdl2) {
	m_pktV.data = NULL;
	m_pktV.size = 0;
	ret = avcodec_decode_video2(m_pCodecCtxV, pFrame, &got_picture, &m_pktV);
	if (ret < 0) {
		Log("Decode Error.(解码错误)\n");
		return ret;
	}
	if (!got_picture)
		return 0;
	if (got_picture) {
		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, m_pCodecCtxV->height,
			pFrameYUV->data, pFrameYUV->linesize);
		sdl2->PlayOneFrameR(pFrameYUV);
	}
	return 1;
}

H264player::~H264player() {
	sws_freeContext(img_convert_ctx);
	av_parser_close(pCodecParserCtx);
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	av_free(out_buffer);
	avcodec_close(m_pCodecCtxV);
	av_free(m_pCodecCtxV);
	avcodec_close(m_pCodecCtxA);
	av_free(m_pCodecCtxA);
}
int H264player::initA()
{
	m_pFormatCtxD = avformat_alloc_context();
	//Open  
	if (avformat_open_input(&m_pFormatCtxD, "tdjm.aac", NULL, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
	// Retrieve stream information  
	if (avformat_find_stream_info(m_pFormatCtxD, NULL)<0) {
		printf("Couldn't find stream information.\n");
		return -1;
	}
	m_pCodecCtxA = m_pFormatCtxD->streams[0]->codec;
	pCodecA = avcodec_find_decoder(m_pCodecCtxA->codec_id);
	if (avcodec_open2(m_pCodecCtxA, pCodecA, NULL)<0) {
		printf("Could not open codec.\n");
		return -1;
	}
	m_pFLTPframe = av_frame_alloc();
	av_init_packet(&m_pktA);

	audio_swr_resampling_audio_init(&m_pSwrContextD, AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_S16);
}
int H264player::transform_and_playA(uint8_t* in_buffer, int cur_size, SDL2Wrapper* sdl2)
{
	
	av_init_packet(&m_pktA);
	m_pktA.data = in_buffer;
	m_pktA.size = cur_size;
	int got_output = 0;
	int ret = avcodec_decode_audio4(m_pCodecCtxA, m_pFLTPframe, &got_output, &m_pktA);

	av_free_packet(&m_pktA);
	if (ret < 0 || !got_output) {
		printf("Error decoding frame\n");
		av_frame_free(&m_pFLTPframe);
		av_frame_free(&m_pS16frame);
		return -1;
	}

	audio_swr_resampling_audio(m_pSwrContextD, m_pFLTPframe->extended_data, &m_pS16frame, AV_SAMPLE_FMT_S16);
	av_frame_free(&m_pFLTPframe);
	//-----------------------------------------------------------------------------
	if (m_pS16frame->linesize[0] < 1024)
	{
		//sdl2.PlayOneFrame(pcmbuf, alen);

		av_frame_free(&m_pS16frame);
		return 0;
	}


	sdl2->PlayOneFrame(m_pS16frame->data[0], m_pS16frame->linesize[0]);
	//cur_pts_a += 1024;

	av_frame_free(&m_pS16frame);

	return 1;
}
int H264player::flushA(SDL2Wrapper* sdl2)
{
	av_init_packet(&m_pktA);
	int got_output = 0;
	int ret = avcodec_decode_audio4(m_pCodecCtxA, m_pFLTPframe, &got_output, NULL);

	av_free_packet(&m_pktA);
	if (ret < 0 || !got_output) {
		printf("Error decoding frame\n");
		av_frame_free(&m_pFLTPframe);
		av_frame_free(&m_pS16frame);
		return -1;
	}

	audio_swr_resampling_audio(m_pSwrContextD, m_pFLTPframe->extended_data, &m_pS16frame, AV_SAMPLE_FMT_S16);
	av_frame_free(&m_pFLTPframe);
	//-----------------------------------------------------------------------------
	if (m_pS16frame->linesize[0] < 1024)
	{
		//sdl2.PlayOneFrame(pcmbuf, alen);

		av_frame_free(&m_pS16frame);
		return 0;
	}


	sdl2->PlayOneFrame(m_pS16frame->data[0], m_pS16frame->linesize[0]);
	//cur_pts_a += 1024;

	av_frame_free(&m_pS16frame);

	return 1;

}

int H264player::init()
{
	return (this->*Init[state])();
}
int H264player::transform_and_play(uint8_t* in_buffer, int cur_size, SDL2Wrapper* sdl2)
{
	return (this->*Decode[state])(in_buffer, cur_size, sdl2);
}
int H264player::flush(SDL2Wrapper* sdl2)
{
	return (this->*Flush[state])(sdl2);
}

void SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int bpp)
{
	char buf[5] = { 0 };
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	FILE *fp;

	char *filename = new char[255];
	struct tm* local;
	time_t t = time(0);
	local = localtime(&t);

	//文件存放路径，根据自己的修改  
	sprintf_s(filename, 255, "%s%d%d%d%d%d.bmp", capdir, local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	if ((fp = fopen(filename, "wb+")) == NULL) {
		printf("open file failed!\n");
		return;
	}

	bmpheader.bfType = 0x4d42;
	bmpheader.bfReserved1 = 0;
	bmpheader.bfReserved2 = 0;
	bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp / 8;

	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biWidth = width;
	bmpinfo.biHeight = height;
	bmpinfo.biPlanes = 1;
	bmpinfo.biBitCount = bpp;
	bmpinfo.biCompression = BI_RGB;
	bmpinfo.biSizeImage = (width*bpp + 31) / 32 * 4 * height;
	bmpinfo.biXPelsPerMeter = 100;
	bmpinfo.biYPelsPerMeter = 100;
	bmpinfo.biClrUsed = 0;
	bmpinfo.biClrImportant = 0;

	fwrite(&bmpheader, sizeof(bmpheader), 1, fp);
	fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);
	fwrite(pFrameRGB->data[0], width*height*bpp / 8, 1, fp);

	fclose(fp);
}

