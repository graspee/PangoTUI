#pragma once
#include "PangoTUI.h"
#include <functional>
//#include "Area.h"

namespace PangoTUI {

class PANGOTUI_API Area;



struct PANGOTUI_API Event {
	int CurrentScreenX, CurrentScreenY;
	int LastScreenX, LastScreenY;
	int DragBeginScreenX, DragBeginScreenY;
	bool IsInitialDragEventP, DragOutOfBoundsP, WasDragSuccessfulP;
	Uint8 mausbutton;
	//more to come, e.g. keys
	SDLKey key;
	Uint16 key_unicode;

	

	Event::Event(void):
		CurrentScreenX(0), CurrentScreenY(0),
		LastScreenX(0), LastScreenY(0),
		DragBeginScreenX(0), DragBeginScreenY(0),
		IsInitialDragEventP(false), DragOutOfBoundsP(false), WasDragSuccessfulP(false)
	{}
};

typedef std::function<void (Area* a, Event& e)> EventLambda;
typedef std::function<void (Area* a)> NonEventLambda;

}

#define EVENTLAMBDA [](Area* a,Event& e)->void
#define NONEVENTLAMBDA [](Area* a)->void