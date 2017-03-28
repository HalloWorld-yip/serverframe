#include"stdafx.h"
#include"v_Encoder.h"
#include"log.h"


YUVEncoder::YUVEncoder(int w, int h) :in_w(w), in_h(h), m_framecntV(0), m_framecntA(0), init_success(true) {
	avcodec_register_all();

	m_pCodecV = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!m_pCodecV) {
		Log("Codec not found\n");
		init_success = false;
		return;
	}
	m_pCodecCtxV = avcodec_alloc_context3(m_pCodecV);
	if (!m_pCodecCtxV) {
		Log("Could not allocate video codec context\n");
		init_success = false;
		return;
	}
	int br = 200000;
	m_pCodecCtxV->bit_rate = 200000;//比特率 
	
	/*固定码率
	pCodecCtx->rc_min_rate =br;
	pCodecCtx->rc_max_rate = br;
	pCodecCtx->bit_rate_tolerance = br;
	pCodecCtx->rc_buffer_size = br;
	pCodecCtx->rc_initial_buffer_occupancy = pCodecCtx->rc_buffer_size * 3 / 4;
	pCodecCtx->rc_buffer_aggressivity = (float)1.0;
	pCodecCtx->rc_initial_cplx = 0.5;*/

	////可变码率
	//pCodecCtx->flags |= CODEC_FLAG_QSCALE;
	//pCodecCtx->rc_min_rate = 200000;
	//pCodecCtx->rc_max_rate = 600000;
	//pCodecCtx->bit_rate = br;

	m_pCodecCtxV->width = in_w;
	m_pCodecCtxV->height = in_h;
	m_pCodecCtxV->time_base.num = 1;//帧率
	m_pCodecCtxV->time_base.den = 20;
	m_pCodecCtxV->gop_size = 60;//一组连续画面
	m_pCodecCtxV->max_b_frames = 0;
	m_pCodecCtxV->pix_fmt = AV_PIX_FMT_YUV420P;

	av_opt_set(m_pCodecCtxV->priv_data, "preset", "ultrafast", 0);
	//av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);

	if (avcodec_open2(m_pCodecCtxV, m_pCodecV, NULL) < 0) {
		init_success = false;
		Log("Could not open codec\n");
		return;
	}

	//pFrame = av_frame_alloc();
	//if (!pFrame) {
	//	printf("Could not allocate video frame\n");
	//	init_success = false;
	//	return ;
	//}
	//pFrame->format = pCodecCtx->pix_fmt;
	//pFrame->width = pCodecCtx->width;
	//pFrame->height = pCodecCtx->height;

	//if (av_image_alloc(pFrame->data, pFrame->linesize, pCodecCtx->width, pCodecCtx->height,
	//	pCodecCtx->pix_fmt, 16) < 0) {
	//	printf("Could not allocate raw picture buffer\n");
	//	init_success = false;
	//	return ;
	//}

	y_size = m_pCodecCtxV->width * m_pCodecCtxV->height;

}
YUVEncoder::YUVEncoder()
{
	avcodec_register_all();
	//编码音频
	m_pCodecA = avcodec_find_encoder(AV_CODEC_ID_AAC);
	if (!m_pCodecA) {
		printf("Codec not found\n");
		init_success = false;
		return;
	}
	m_pCodecCtxA = avcodec_alloc_context3(m_pCodecA);

	m_pCodecCtxA->codec_id = AV_CODEC_ID_AAC;
	m_pCodecCtxA->codec_type = AVMEDIA_TYPE_AUDIO;
	m_pCodecCtxA->sample_fmt = AV_SAMPLE_FMT_FLTP;
	m_pCodecCtxA->sample_rate = 44100;
	m_pCodecCtxA->channel_layout = AV_CH_LAYOUT_STEREO;
	m_pCodecCtxA->channels = av_get_channel_layout_nb_channels(m_pCodecCtxA->channel_layout);
	m_pCodecCtxA->bit_rate = 64000;

	if (!m_pCodecCtxA) {
		printf("Could not allocate audio codec context\n");
		init_success = false;
		return;
	}
	if (avcodec_open2(m_pCodecCtxA, m_pCodecA, NULL) < 0) {
		printf("Could not open codec\n");
		init_success = false;
		return;
	}
}
int YUVEncoder::Encode1FrmV(AVFrame* psrc, uint8_t* dstbuf) {
	int got_output, ret, tmp = 0;

	av_init_packet(&m_pktV);
	m_pktV.data = NULL;    // packet data will be allocated by the encoder  
	m_pktV.size = 0;
	/*----------------------------------------------------------------------------------*/
	//Read raw YUV data  

	/*----------------------------------------------------------------------------------*/
	/* encode the image */
	ret = avcodec_encode_video2(m_pCodecCtxV, &m_pktV, psrc, &got_output);
	if (ret < 0) {
		Log("Error encoding frame\n");
		return -1;
	}
	if (got_output) {
		//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
		m_framecntV++;
		/*----------------------------------------------------------------------------------*/
		//fwrite(pdst->data, 1, pdst->size, fp_out);
		if (m_pktV.size > MAX_BUFSIZE) {
			Log("buf.\n");
		}
		memcpy_s(dstbuf, MAX_BUFSIZE, m_pktV.data, m_pktV.size);
		/*----------------------------------------------------------------------------------*/
		tmp = m_pktV.size;
		av_free_packet(&m_pktV);
	}
	return tmp;
}
int YUVEncoder::Flush_EncoderV(uint8_t* dstbuf) {
	int got_output, ret, tmp = 0;
	ret = avcodec_encode_video2(m_pCodecCtxV, &m_pktV, NULL, &got_output);
	if (ret < 0) {
		printf("Error encoding frame\n");
		return -1;
	}
	if (got_output) {
		printf("Succeed to encode frame: %5d\tsize:%5d\n", m_framecntV, m_pktV.size);
		m_framecntV++;
		/*----------------------------------------------------------------------------------*/
		//fwrite(pdst->data, 1, pdst->size, fp_out);
		if (m_pktV.size > MAX_BUFSIZE) {
			Log("buf.\n");
		}
		memcpy_s(dstbuf, MAX_BUFSIZE, m_pktV.data, m_pktV.size);
		/*----------------------------------------------------------------------------------*/
		tmp = m_pktV.size;
		av_free_packet(&m_pktV);
	}
	return tmp;
}

int YUVEncoder::Encode1FrmA(AVFrame* psrc, uint8_t* dstbuf)
{
	int got_output, ret, tmp = 0;

	av_init_packet(&m_pktA);
	m_pktA.data = NULL;    // packet data will be allocated by the encoder  
	m_pktA.size = 0;
	/*----------------------------------------------------------------------------------*/
	//Read raw YUV data  

	/*----------------------------------------------------------------------------------*/
	/* encode the image */
	ret = avcodec_encode_audio2(m_pCodecCtxA, &m_pktA, psrc, &got_output);
	if (ret < 0) {
		printf("Error encoding frame\n");
		return -1;
	}
	if (got_output) {
		//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
		m_framecntA++;
		/*----------------------------------------------------------------------------------*/
		//fwrite(pdst->data, 1, pdst->size, fp_out);
		if (m_pktA.size >(4096 << 4)) {
			printf("buf.\n");
		}
		memcpy_s(dstbuf, (4096 << 4), m_pktA.data, m_pktA.size);
		/*----------------------------------------------------------------------------------*/
		tmp = m_pktA.size;
		av_free_packet(&m_pktA);
	}
	return tmp;

}
int YUVEncoder::Flush_EncoderA(uint8_t* dstbuf)
{
	int got_output, ret, tmp = 0;
	ret = avcodec_encode_audio2(m_pCodecCtxA, &m_pktA, NULL, &got_output);
	if (ret < 0) {
		printf("Error encoding frame\n");
		return -1;
	}
	if (got_output) {
		//printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, pkt.size);
		m_framecntA++;
		/*----------------------------------------------------------------------------------*/
		//fwrite(pdst->data, 1, pdst->size, fp_out);
		if (m_pktA.size >(4096 << 4)) {
			printf("buf.\n");
		}
		memcpy_s(dstbuf, (4096 << 4), m_pktA.data, m_pktA.size);
		/*----------------------------------------------------------------------------------*/
		tmp = m_pktA.size;
		av_free_packet(&m_pktA);
	}
	return tmp;

}


YUVEncoder::~YUVEncoder() {
	avcodec_close(m_pCodecCtxV);
	av_free(m_pCodecCtxV);
	avcodec_close(m_pCodecCtxA);
	av_free(m_pCodecCtxA);
	//av_freep(&pFrame->data[0]);
	//av_frame_free(&pFrame);

}