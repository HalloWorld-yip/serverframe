#pragma once

extern "C" {
#include<libavcodec\avcodec.h>
#include<SDL.h>
}
//YUV only

struct SDLData
{
	int screen_w;
	int screen_h;
	SDL_Renderer* sdlRenderer;
	SDL_Window *screen;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;
};


class SDL2Wrapper {
public:
	SDL2Wrapper();
	~SDL2Wrapper();

	bool InitWindowL(int w, int h, const void* gwin = 0);
	void DestroyWindowL();
	void PlayOneFrameL(AVFrame* pFrameYUV);

	bool InitWindowR(int w, int h, const void* gwin = 0);
	void DestroyWindowR();
	void PlayOneFrameR(AVFrame* pFrameYUV);
	void PlayOneFrame(uint8_t* pcmbuf, int size);
	int	GetWidthR()const { return m_SDLDataRecv.screen_w; }
	int	GetHeightR()const { return m_SDLDataRecv.screen_h; }
	int	GetWidthL()const { return m_SDLDataLocal.screen_w; }
	int	GetHeightL()const { return m_SDLDataLocal.screen_h; }
private:
	SDLData			m_SDLDataLocal;
	SDLData			m_SDLDataRecv;
	SDL_AudioSpec	m_SDLAudioSpec;
	static int		m_AudioLen;
	static uint8_t*		m_AudioPos;
	uint8_t			m_AudioBuf[8192];

	static void  fill_audio(void *udata, Uint8 *stream, int len);
	void SetAudioSpec();
	bool InitWindow(int w, int h, SDLData* pdata, const void* gwin = 0);
	void DestroyWindow(SDLData* pdata);
	void PlayOneFrame(AVFrame* pFrameYUV, SDLData* pdata);
};
