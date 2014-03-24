#pragma once

#include "PangoTUI.h"

namespace PangoTUI {

//represents one character on the screen, its bg and fg colour plus the character it is
class PANGOTUI_API CharCell
{
public:

	Uint32 fg,bg;
	Uint8 c;

	CharCell();
	~CharCell(void);
};

}