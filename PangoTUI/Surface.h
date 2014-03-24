#pragma once
#include "PangoTUI.h" 

#include "PangoEngine.h"

#include "Algorithms.h"

#include "sdl_image.h"

#define UNLOCK_IF_NEEDED(x) if(SDL_MUSTLOCK(x)) {SDL_UnlockSurface(x);}
#define LOCK_OR_RET_NULL(x)  if(SDL_MUSTLOCK(x)) {if(SDL_LockSurface(x) < 0) {return NULL;}}
#define LOCK_OR_RET(x)       if(SDL_MUSTLOCK(x)) {if(SDL_LockSurface(x) < 0) {return;}}

namespace PangoTUI { 

	

//Wrapper for SDL_Surface
class PANGOTUI_API Surface
{
public:
	
	SDL_Surface* surface;
	static PangoEngine* pe;


	Surface();
	Surface(char* filename);
	Surface(char* filename, bool alpha);
	Surface(SDL_Surface*);

	~Surface(void);

	void Display(int x=0, int y=0);
	void DisplayCentered(void);
	void DisplayCenteredAsChars(void);
	void DisplayAsChars(int x,int y);

	void PartBlit(int sx,int sy, int w, int h,int dx,int dy);

	Surface* Copy(void);

	Surface* 
	SDL_GFX_FILTER_Dither_ErrorDiffusion(
			int algorithm[],
			Uint8 threshold,
			std::function<Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumFn);
	
	Surface* SDL_GFX_FILTER_ResizeBilinear(int newW, int newH, bool returncopy=false);
	Surface* IntelligentResize(int w, int h);
	Surface* SDL_GFX_FILTER_ConvMatrix(float kernel[], float kernel_factor, float divisor, float offset);
	Surface* Surface::FitToScreen(bool magnify=false);
	Surface* Surface::FitToArbRect(int _w, int _h, bool magnify=false);
	void ASCIIArtOnMainWindow(void);
	void Surface::DitherToMainWindow(void);

	
};

}