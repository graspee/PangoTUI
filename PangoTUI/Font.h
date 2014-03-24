#pragma once
#include "PangoTUI.h"

namespace PangoTUI {

class PANGOTUI_API Font
{
public:
	
	static const unsigned char Amstrad437[256][8];
	static const unsigned char Halfwidth[256][8];
	static const unsigned char Hobbit[256][8];

	Uint8 sizex,sizey; //previously fontsizex/y is size in pixels
	Uint8 dataperchar; //needed in calculations
	unsigned char* fontdata;
	

	Font(unsigned char* _fontdata, Uint8 _sizex, Uint8 _sizey);
	~Font(void);
	
};

}