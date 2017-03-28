#include"stdafx.h"
#include"v_Record.h"
#include"log.h"



static char *dup_wchar_to_utf8(wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char *)av_malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	return s;
}

void show_dshow_device_option() {
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_options", "true", 0);
	AVInputFormat *iformat = av_find_input_format("dshow");
	printf("========Device Option Info======\n");
	avformat_open_input(&pFormatCtx, "video=Integrated Camera", iformat, &options);
	printf("================================\n");
}


Recorder::Recorder(int w, int h, SDL2Wrapper *psdl, const void* gwin) :m_framepts(0), m_localw(w), m_localh(h), m_pSdlwrapper(psdl) {
	m_pSdlwrapper->InitWindowL(w, h, gwin);

	av_register_all();
	avformat_network_init();
	m_pFormatCtxV = avformat_alloc_context();
	//Register Device  
	avdevice_register_all();
	show_dshow_device_option();
	m_ifmt = av_find_input_format("vfwcap");

	m_pFrameV = av_frame_alloc();
	m_packetV = (AVPacket *)av_malloc(sizeof(AVPacket));
	m_packetA = (AVPacket *)av_malloc(sizeof(AVPacket));
	m_pFrameYUV = av_frame_alloc();
}
Recorder::Recorder()
{
	av_register_all();
	avformat_network_init();
	//Register Device  
	avdevice_register_all();
	m_packetA = (AVPacket *)av_malloc(sizeof(AVPacket));
}
Recorder::~Recorder() {
	m_pSdlwrapper->DestroyWindowL();

	sws_freeContext(m_img_convert_ctx);
	av_free(m_pFrameYUV);
	sws_freeContext(m_local_ctx);
	/*av_free(m_pFrameLOC);*/
	avcodec_close(m_pCodecCtxV);
	avformat_close_input(&m_pFormatCtxV);
}
int Recorder::StartRecordV() {
	//摄像头输入
	if (avformat_open_input(&m_pFormatCtxV, "0", m_ifmt, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
	//获取信息
	if (avformat_find_stream_info(m_pFormatCtxV, NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	m_videoindex = -1;
	for (int i = 0; i<m_pFormatCtxV->nb_streams; i++)
		if (m_pFormatCtxV->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoindex = i;
			break;
		}
	if (m_videoindex == -1)
	{
		printf("Couldn't find a video stream.\n");
		return -1;
	}
	//初始化解码器
	m_pCodecCtxV = m_pFormatCtxV->streams[m_videoindex]->codec;
	m_pCodecV = avcodec_find_decoder(m_pCodecCtxV->codec_id);
	if (m_pCodecV == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(m_pCodecCtxV, m_pCodecV, NULL)<0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	//需要打开流获取视频信息才能初始化frame等结构。
	prepare_struct();

	m_local_ctx = sws_getContext(m_pCodecCtxV->width, m_pCodecCtxV->height, m_pCodecCtxV->pix_fmt, m_localw, m_localh, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	m_img_convert_ctx = sws_getContext(m_pCodecCtxV->width, m_pCodecCtxV->height, m_pCodecCtxV->pix_fmt, m_pCodecCtxV->width, m_pCodecCtxV->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);



	return 0;
}

int	Recorder::StartRecordA()
{
	//打开音频

	//查找输入方式
	AVInputFormat *pAudioInputFmt = av_find_input_format("dshow");

	//以Direct Show的方式打开设备，并将 输入方式 关联到格式上下文
	char * psDevName = dup_wchar_to_utf8(L"audio=麦克风 (Realtek High Definition Audio)");

	if (avformat_open_input(&m_pFormatCtxA, psDevName, pAudioInputFmt, NULL) < 0)
	{
		printf("Couldn't open input stream.（无法打开音频输入流）\n");
		return -1;
	}

	if (avformat_find_stream_info(m_pFormatCtxA, NULL)<0)
		return -1;
	m_pCodecCtxA = m_pFormatCtxA->streams[0]->codec;
	if (m_pFormatCtxA->streams[0]->codec->codec_type != AVMEDIA_TYPE_AUDIO)
	{
		printf("Couldn't find video stream information.（无法获取音频流信息）\n");
		return -1;
	}

	AVCodec *tmpCodec = avcodec_find_decoder(m_pCodecCtxA->codec_id);
	if (0 > avcodec_open2(m_pCodecCtxA, tmpCodec, NULL))
	{
		printf("can not find or open audio decoder!\n");
	}

}

void Recorder::prepare_struct() {
	uint8_t* out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_pCodecCtxV->width, m_pCodecCtxV->height, 1));
	av_image_fill_arrays(m_pFrameYUV->data, m_pFrameYUV->linesize, out_buffer,
		AV_PIX_FMT_YUV420P, m_pCodecCtxV->width, m_pCodecCtxV->height, 1);//初始化frame跟out_buffer内容无关
																		  //set width and heigh
	m_pFrameYUV->width = m_pCodecCtxV->width;
	m_pFrameYUV->height = m_pCodecCtxV->height;
	m_pFrameYUV->format = m_pCodecCtxV->pix_fmt;

	//m_pFrameLOC = av_frame_alloc();
	//m_pLocalBuffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_pCodecCtx->width, m_pCodecCtx->height, 1));
	//av_image_fill_arrays(m_pFrameLOC->data, m_pFrameLOC->linesize, m_pLocalBuffer,
	//	AV_PIX_FMT_YUV420P, m_localw, m_localh, 1);//初始化frame跟out_buffer内容无关
	//																	//set width and heigh
	//m_pFrameLOC->width = m_localw;
	//m_pFrameLOC->height = m_localh;
	//m_pFrameLOC->format = m_pCodecCtx->pix_fmt;


}


int Recorder::OutputH26Pkg(YUVEncoder* yuvEn, uint8_t* dstbuf) {
	int got_picture;
	int bsize = 0;
	if (av_read_frame(m_pFormatCtxV, m_packetV) >= 0) {
		if (m_packetV->stream_index == m_videoindex) {
			if (avcodec_decode_video2(m_pCodecCtxV, m_pFrameV, &got_picture, m_packetV) < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture) {
				/*
				sws_scale(m_local_ctx, (const unsigned char* const*)m_pFrame->data, m_pFrame->linesize, 0, m_pCodecCtx->height, m_pFrameLOC->data, m_pFrameLOC->linesize);*/

				sws_scale(m_local_ctx, (const unsigned char* const*)m_pFrameV->data, m_pFrameV->linesize, 0, m_pCodecCtxV->height, m_pFrameYUV->data, m_pFrameYUV->linesize);
				/*
				m_pFrameLOC->pts = m_framepts++;*/
				m_pFrameYUV->pts = m_framepts++;
				//m_pSdlwrapper->PlayOneFrameL(m_pFrameLOC);
				m_pSdlwrapper->PlayOneFrameL(m_pFrameYUV);
				sws_scale(m_img_convert_ctx, (const unsigned char* const*)m_pFrameV->data, m_pFrameV->linesize, 0, m_pCodecCtxV->height, m_pFrameYUV->data, m_pFrameYUV->linesize);


				if ((bsize = yuvEn->Encode1FrmV(m_pFrameYUV, dstbuf)) < 0) {
					printf("Encode Error.\n");
					return -1;
				}


			}
		}
		av_free_packet(m_packetV);
	}
	return bsize;
}
int	Recorder::OutputAACFrm(YUVEncoder* yuvEn, AVFrame** dstfrm)
{
	int got_picture;
	if (av_read_frame(m_pFormatCtxA, m_packetA) >= 0) {
		if (m_packetA->stream_index == m_videoindex) {
			if (avcodec_decode_audio4(m_pCodecCtxA, *dstfrm, &got_picture, m_packetA) < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (!got_picture) {
				return -1;
			}
		}
		av_free_packet(m_packetA);
	}
	return 1;

}
