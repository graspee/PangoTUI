#include "stdafx.h"
#include "Palette.h"

using namespace PangoTUI;

#include "PaletteStaticData.h"

Palette::Palette(int numcol,unsigned char* rgbdata,SDL_Surface* screen) :
	numberofcolours(numcol)
{
	rgbvals=new rgbtriplet[numberofcolours];
	u32vals=new Uint32[numberofcolours];

	for(int f=0;f<numberofcolours;f++){
		rgbvals[f].r=*rgbdata++;
		rgbvals[f].g=*rgbdata++;
		rgbvals[f].b=*rgbdata++;
		u32vals[f]=SDL_MapRGB(screen->format,rgbvals[f].r,rgbvals[f].g,rgbvals[f].b);
	}
}


Palette::~Palette(void){
	delete [] rgbvals;
	delete [] u32vals;
}

