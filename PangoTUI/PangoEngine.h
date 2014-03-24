#pragma once

#undef CreateWindow //fuck you, Microsoft, you global namespace polluting cunts.
#undef LoadBitmap //Microsoft! Stahp! Seriously, you just cost me ten minutes you fucking buffoonz

#include "PangoTUI.h"
#include <vector>
#include "Font.h"
#include "Area.h"
#include "Rectangle.h"
#include "Palette.h"
#include "Bitmap.h"
#include <functional>
#include "Events.h"
#include "Sprite16x16.h"
#include <time.h>
#include <random>

namespace PangoTUI {
	
	//class PANGOTUI_API Window;
	//class PANGOTUI_API Font;
	class PANGOTUI_API Area;
	

//This class represents the whole engine and includes the main graphical window
class PANGOTUI_API PangoEngine
{ 
public:
	friend class PANGOTUI_API Area;
	friend class PANGOTUI_API Surface;

	SDL_Surface* screen;
	bool fullscreen;
	
	
	Uint8 fontzoomx,fontzoomy; //1 is normal size, 2 is double size, 3 is triple. non square is allowed
	
	Font* font; 
	Uint8 fontramp[256];
	float fontramp2[256];
	
	Area** KlikMap;
	Area* Focus;
	//int LastSeenMausX, LastSeenMausY;

	void PangoEngine::CommonSetup(void);

//public:
	EventLambda OnAppGoDown;
	int xres,yres;
	Palette* palette;
	Area* desktop;

	Uint32 colour_bg, colour_window_border, colour_icon, colour_icon_hi,colour_icon_hi_close; 
	
	PangoEngine(int _xres,int _yres,int _fontzoomx,int _fontzoomy,bool _fullscreen=false); //constructor using built-in font
	PangoEngine(int _xres,int _yres,Font* font,int _fontzoomx,int _fontzoomy,bool _fullscreen=false); //constructor specifying your own font
	~PangoEngine(void);

	void DrawChar(int x,int y,unsigned char c,Uint32 fg,Uint32 bg, bool sdlrectrefresh=true);
	void DrawString_halfwidth(std::string s,int x,int y,Uint32 fg,Uint32 bg);
	void DrawCharEx(int x,int y,unsigned char c,Uint32 fg,Uint32 bg, bool sdlrectrefresh,bool existingfg=false,bool existingbg=false,bool existingchar=false);
	void Pause(Uint32 milliseconds);
	void SDLUpdateRect(int x, int y, int x2, int y2);
	void SDLUpdateRectPixels(int x, int y, int w, int h);
	
	void PangoEngine::SetAppTitle(char* title);

	void PangoEngine::test01(char* filename);
	void PangoEngine::CLS_bypixel(void);
	void PangoEngine::CLS_allcells(void);
	void MakeFontRamp(void);
	Uint8 WaitForMausClik(void);
	CharCell* PangoEngine::WaitForMausClik_RetCharCell(void);
	Uint8 WaitForAnyKey(void);
	void DoEvents(void);
	void DrawPixelRect(int _x, int _y, int _w, int _h,Uint32 col);
	//draws a pixel that is 1x1,2x2 or 3x3 depending on resolution
	void DrawScaledPixel(int _x,int _y,Uint32 _colour,bool rectupdate=true);
	int SetKeyRepeat(int a,int b);
	void Pause(int ms);
	Uint32 MapRGB(Uint8 r,Uint8 g,Uint8 b);
	void GetRGB(Uint32 xx,Uint8* r,Uint8* g,Uint8* b);
	bool BreakOutSwitch;

	Uint32 fluctuate(Uint32 x);
	void PangoEngine::DrawSprite16x16(Sprite16x16* s,int _x, int _y,bool drawtransparentp=false, bool alpha50p=false);
};

}