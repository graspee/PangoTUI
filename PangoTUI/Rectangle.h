#pragma once
#include "PangoTUI.h"

namespace PangoTUI {

class PANGOTUI_API Rectangle {

public:
	int x,y,x2,y2;
	int height,width;
	
	Rectangle(void);
	Rectangle(int _x,int _y,int _x2,int _y2,bool widthnheight=false);
	~Rectangle(void);
};

}