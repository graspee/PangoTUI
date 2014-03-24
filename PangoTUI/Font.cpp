#include "stdafx.h"
#include "Font.h"


using namespace PangoTUI;

#include "FontStaticData.h" //inline that motherfucking amstrad 437 font!

Font::Font(unsigned char* _fontdata, Uint8 _sizex, Uint8 _sizey) :
	fontdata(_fontdata),
	sizex(_sizex), sizey(_sizey),
	dataperchar(((_sizex/8)*(_sizex/8))*8) //FIXME assumes square font. 
{
	if(sizex<8)dataperchar=((8/8)*(8/8))*8;
}


Font::~Font(void)
{
}

