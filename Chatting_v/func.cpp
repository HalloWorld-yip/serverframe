#include"stdafx.h"
#include"func.h"


bool findname(net_clt* p) {
	return (0==memcmp(p->name, find_name, strlen(find_name)));
}


bool findip(net_clt* p) {
	return p->ip == find_ip ;
}


net_clt* search_in_list(const char* name) {
	strcpy(find_name,name);
	std::vector<net_clt*>::iterator tmp;
	return ((tmp = find_if(clt_list.begin(), clt_list.end(), findname)) == clt_list.end()) ? 0 : (*tmp);
}

net_clt* searchip_in_list(ULONG ip) {
	find_ip = ip;
	std::vector<net_clt*>::iterator tmp;
	return ((tmp = find_if(clt_list.begin(), clt_list.end(), findip)) == clt_list.end()) ? 0 : (*tmp);
}

void add_client(const char* name,ULONG ip,int status){

	net_clt* pclt;
	if (pclt = search_in_list(name))
	{
		pclt->life = 3;
		return;
	}
	pclt = new net_clt;
	//建立客户资料
	pclt->s			= 0;
	pclt->status	= status;
	pclt->ip		= ip;
	pclt->life		= 3;
	strcpy(pclt->name, name);
	clt_list.push_back(pclt);

	return;
}


bool C2W(const char* str,wchar_t* wstr)
{
int len=MultiByteToWideChar(CP_OEMCP,0,str,-1/*null terminated*/,wstr,0);
return len==MultiByteToWideChar(CP_OEMCP,0,str,-1/*null terminated*/,wstr,len);
}
 bool W2C(const wchar_t* wstr,char* str)
{
int len=WideCharToMultiByte(CP_OEMCP,0,wstr,-1/*null terminated*/,str,0,0,0);
return len==WideCharToMultiByte(CP_OEMCP,0,wstr,-1/*null terminated*/,str,len,0,0);
}

void GetTime(char* str)
{
	struct tm* local;
	time_t t;
	t = time(NULL); 
	local = localtime(&t);
	sprintf(str,"%d/%d %02d:%02d:%02d",local->tm_mon,local->tm_mday,local->tm_hour,
			local->tm_min , local->tm_sec );
		
}


void audio_swr_resampling_audio_init(SwrContext **swr_ctx, AVSampleFormat ifmt, AVSampleFormat ofmt) {

	if (*swr_ctx) {
		swr_free(swr_ctx);
	}
	*swr_ctx = swr_alloc();

	if (!*swr_ctx) {
		printf("swr_alloc failed");
		return;
	}

	/* set options */

	av_opt_set_int(*swr_ctx, "ich", 2, 0);
	av_opt_set_int(*swr_ctx, "in_sample_rate", 44100, 0);
	av_opt_set_sample_fmt(*swr_ctx, "in_sample_fmt", ifmt, 0);

	av_opt_set_int(*swr_ctx, "och", 2, 0);
	av_opt_set_int(*swr_ctx, "out_sample_rate", 44100, 0);
	av_opt_set_sample_fmt(*swr_ctx, "out_sample_fmt", ofmt, 0);// AV_SAMPLE_FMT_S16

															   /* initialize the resampling context */

	int ret = 0;

	if ((ret = swr_init(*swr_ctx)) < 0) {

		printf("Failed to initialize the resampling context\n");

		if (*swr_ctx) {

			swr_free(swr_ctx);

			*swr_ctx = NULL;
		}
		return;
	}
}


int audio_swr_resampling_audio(SwrContext *swr_ctx, uint8_t **srcbuf, AVFrame **dstFrame, AVSampleFormat ofmt) {//for AAC 1024samples

	int src_nb_samples = 1024;
	int dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, 44100) + src_nb_samples,
		44100, 44100, AV_ROUND_UP);

	if (*dstFrame == NULL)
	{
		*dstFrame = av_frame_alloc();
		(*dstFrame)->nb_samples = dst_nb_samples;
		(*dstFrame)->channel_layout = 3;
		(*dstFrame)->format = ofmt;
		(*dstFrame)->sample_rate = 44100;
		av_frame_get_buffer(*dstFrame, 0);
	}


	int len = swr_convert(swr_ctx, (*dstFrame)->data, dst_nb_samples, (const uint8_t**)srcbuf, 1024);

	if (len < 0) {

		printf("error swr_convert");

		goto end;

	}



	int dst_bufsize = len * 3 * av_get_bytes_per_sample(ofmt);

	//printf(" dst_bufsize:%d", dst_bufsize);

	return dst_bufsize;

end:

	return -1;

}

void audio_swr_resampling_audio_destory(SwrContext **swr_ctx) {

	if (*swr_ctx) {

		swr_free(swr_ctx);

		*swr_ctx = NULL;

	}

}