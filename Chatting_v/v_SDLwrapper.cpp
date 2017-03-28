#include"stdafx.h"
#include"v_SDLwrapper.h"
#include"log.h"

uint8_t* SDL2Wrapper::m_AudioPos = NULL;
int SDL2Wrapper::m_AudioLen = 0;

void  SDL2Wrapper::fill_audio(void *udata, Uint8 *stream, int len) {
	//SDL 2.0  
	SDL_memset(stream, 0, len);
	if (m_AudioLen == 0)
		return;
	len = (len>m_AudioLen ? m_AudioLen : len);

	SDL_MixAudio(stream, m_AudioPos, len, SDL_MIX_MAXVOLUME);
	m_AudioPos += len;
	m_AudioLen -= len;
}



SDL2Wrapper::SDL2Wrapper()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		Log(SDL_GetError());
		return;
	}
	SetAudioSpec();
	SDL_OpenAudio(&m_SDLAudioSpec, NULL);
};

bool SDL2Wrapper::InitWindowL(int w, int h, const void* gwin)
{
	return InitWindow(w, h, &m_SDLDataLocal, gwin);
}
void SDL2Wrapper::DestroyWindowL()
{
	DestroyWindow(&m_SDLDataLocal);
}
void SDL2Wrapper::PlayOneFrameL(AVFrame* pFrameYUV)
{
	PlayOneFrame(pFrameYUV, &m_SDLDataLocal);
}

bool SDL2Wrapper::InitWindowR(int w, int h, const void* gwin)
{
	return InitWindow(w, h, &m_SDLDataRecv, gwin);
}
void SDL2Wrapper::DestroyWindowR()
{
	DestroyWindow(&m_SDLDataRecv);
}
void SDL2Wrapper::PlayOneFrameR(AVFrame* pFrameYUV)
{
	PlayOneFrame(pFrameYUV, &m_SDLDataRecv);
}

void SDL2Wrapper::SetAudioSpec()
{
	m_SDLAudioSpec.freq = 44100;
	m_SDLAudioSpec.format = AUDIO_S16SYS;
	m_SDLAudioSpec.channels = 2;
	m_SDLAudioSpec.silence = 0;
	m_SDLAudioSpec.samples = 1024;
	m_SDLAudioSpec.callback = fill_audio;
}


void SDL2Wrapper::PlayOneFrame(AVFrame* pFrameYUV, SDLData* pdata)
{
	SDL_UpdateYUVTexture(pdata->sdlTexture, &pdata->sdlRect,
		pFrameYUV->data[0], pFrameYUV->linesize[0],
		pFrameYUV->data[1], pFrameYUV->linesize[1],
		pFrameYUV->data[2], pFrameYUV->linesize[2]);
	SDL_RenderClear(pdata->sdlRenderer);
	SDL_RenderCopy(pdata->sdlRenderer, pdata->sdlTexture, NULL, &pdata->sdlRect);
	SDL_RenderPresent(pdata->sdlRenderer);
	//Delay 40ms  
	//SDL_Delay(40);
}



void SDL2Wrapper::PlayOneFrame(uint8_t* pcmbuf, int size)
{
	memcpy_s(m_AudioBuf, 8192, pcmbuf, size);
	while (m_AudioLen > 0)
	{
		SDL_Delay(10);
	}
	m_AudioLen = size;
	m_AudioPos = m_AudioBuf;
	SDL_PauseAudio(0);
	//SDL_PauseAudio(1);
}

SDL2Wrapper::~SDL2Wrapper()
{
	SDL_Quit();
};


bool SDL2Wrapper::InitWindow(int w, int h, SDLData* pdata, const void* gwin)
{
	pdata->screen_w = w;
	pdata->screen_h = h;
	if (gwin)
	{
		pdata->screen = SDL_CreateWindowFrom(gwin);
	}
	else
	{
		pdata->screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			pdata->screen_w, pdata->screen_h, SDL_WINDOW_OPENGL);
	}
	if (!pdata->screen) {
		Log(SDL_GetError());
		return false;
	}
	pdata->sdlRenderer = SDL_CreateRenderer(pdata->screen, -1, 0);
	pdata->sdlTexture = SDL_CreateTexture(pdata->sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pdata->screen_w, pdata->screen_h);


	pdata->sdlRect.x = 0;
	pdata->sdlRect.y = 0;
	pdata->sdlRect.w = pdata->screen_w;
	pdata->sdlRect.h = pdata->screen_h;

}
void SDL2Wrapper::DestroyWindow(SDLData* pdata) {
	if (pdata)
	{
		SDL_DestroyTexture(pdata->sdlTexture);
		SDL_DestroyRenderer(pdata->sdlRenderer);
		SDL_DestroyWindow(pdata->screen);

	}
}