#pragma once

#ifdef PANGOTUI_EXPORTS
#define PANGOTUI_API __declspec(dllexport)
#else
#define PANGOTUI_API __declspec(dllimport)
#endif

//includes user of the dll will need get included
#include "sdl.h" 
#include <iostream>
#include <fstream>

//#include "PangoEngine.h"
//#include "Area.h"
//#include "Font.h"
//#include "Rectangle.h"
//#include "CharCell.h"
//#include "ExpandingArea.h"

