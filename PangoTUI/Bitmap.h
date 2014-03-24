#pragma once
#include "PangoTUI.h"


namespace PangoTUI {

class PANGOTUI_API Bitmap
{
public:
	int w,h;
	unsigned char* rgbdata;

	Bitmap(void);
	~Bitmap(void);

	Bitmap(int _w, int _h);

	bool Bitmap::ExportArray( char* filename, Uint8 tripletsperline=3);
};

}//area::DrawBitmap