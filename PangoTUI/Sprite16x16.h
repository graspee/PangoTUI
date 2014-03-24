#pragma once
#include "PangoTUI.h"

namespace PangoTUI {

class PANGOTUI_API Sprite16x16
{
public:

	Uint32 colours[16][16];
	bool transparency[16][16];

	Sprite16x16(void);
	Sprite16x16(SDL_Surface* screen,const char* filename);
	~Sprite16x16(void);

	void SaveToFile(SDL_Surface* screen,const char* filename);
};

}