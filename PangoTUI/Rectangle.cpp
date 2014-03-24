#include "stdafx.h"
#include "Rectangle.h"

using namespace PangoTUI;

Rectangle::Rectangle(void) :
	x(0),y(0),
	x2(0),y2(0),
	width(0),height(0) 
{

}

//creates a rectangle: x,y of top left then bottom right. optional bool, if true then param 3&4 taken to be width and height
Rectangle::Rectangle(int _x,int _y,int _x2,int _y2,bool widthnheight): x(_x),y(_y) {
	if(widthnheight){
		width=_x2; height=_y2;
		x2=x+width-1; y2=y+height-1;
	} else {
		x2=_x2; y2=_y2;
		width=x+x2+1;height=y+y2+1;
	}
//	std::cout << "Rectangle::Rectangle: x " << x << " y " << y << " x2 " << x2 << " y2 " << y2 << " h " << height << " w " << width << std::endl;
}


Rectangle::~Rectangle(void){
}
