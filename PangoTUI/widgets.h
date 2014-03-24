#pragma once
#include "PangoTUI.h"
#include "Area.h"
#include <list>


namespace PangoTUI {

#define SCROLLBAR(x) ((ScrollbarVertical*)x)
#define SCROLLBARH(x) ((ScrollbarHorizontal*)x)
#define LISTBOCHS(x) ((Listbochs*)x)
#define BUTTON(x) ((Button*)x)
#define BORDERLESSBUTTON(x) ((BorderlessButton*)x)
#define SLIDER(x) ((Slider*)x)
#define SINGLELINETEXTENTRY(x) ((SingleLineTextEntry*)x)

class PANGOTUI_API SingleLineTextEntry : public Area {
public:

	std::string value;
	int cursorx;
	NonEventLambda OnValueChange;
	NonEventLambda OnValueChangedManually;
	NonEventLambda OnSubmit;

	SingleLineTextEntry(Area* _parent, std::string _title, int _x, int _y, int _width, int _maxlen, bool _visible=true, Area* aoi=NULL);
	Area* FullReDraw(void);
	void ErrorDraw(void);
	void SetValue(std::string v);

};

class PANGOTUI_API BorderlessButton : public Area {
public:
	Uint32 TextColour;
	BorderlessButton(Area* _parent, std::string _title,Uint32 _butclr, int _x, int _y, bool _visible=true, Area* aoi=NULL);
	Area* FullReDraw (void);
};

class PANGOTUI_API CustomWidget : public Area {
public:
	CustomWidget(Area* _parent,std::string _title, const Rectangle& r, bool _visible=true, Area* aoi=NULL);
	NonEventLambda OnFullReDraw;
	Area* FullReDraw(void);
};


class PANGOTUI_API Label : public Area {
public:
	Ecrayola cfg,cbg;
	Uint32 colour_fg, colour_bg;
	Label(Area* _parent, std::string _title,Ecrayola fg, Ecrayola bg, const Rectangle& r,  bool _visible=true, Area* aoi=NULL);
};

class PANGOTUI_API ProgressBar : public Area {
public:
	int minimum, maximum, value;
	float perblock;
	Uint32 colour;
	ProgressBar(Area* _parent, std::string _title,Ecrayola fg, int _x,int _y,int _w, int _min, int _max, int _value,  bool _visible=true, Area* aoi=NULL);
	Area* FullReDraw (void);
	void ReCalc(void);
};


class PANGOTUI_API RadioButtonz : public Area {
public:
	Uint8 value;
	EventLambda OnValueChange;
	//std::vector<std::string>* labels;
	//RadioButtonz(Area* _parent, std::string _title, const std::vector<const std::string>& _labels,const Rectangle& r, Uint8 value, Area* aoi=NULL);
	//RadioButtonz(Area* _parent, std::string _title,  std::vector< std::string> _labels,const Rectangle& r, Uint8 value, Area* aoi=NULL);
	RadioButtonz(Area* _parent, std::string _title, std::string _labels[],const Rectangle& r, Uint8 value, bool _visible=true,Area* aoi=NULL);
	~RadioButtonz(void);
};

class PANGOTUI_API Checkbochs : public Area {
public:
	bool value;
	Checkbochs(Area* _parent, std::string _title, int _x, int _y, bool value, bool _visible=true,Area* aoi=NULL);
};

class PANGOTUI_API Button : public Area {
public:
	Uint32 TextColour;
	Button(Area* _parent, std::string _title,Uint32 buttculurhururh,  int _x, int _y, bool isdefault=false, bool _visible=true,Area* aoi=NULL);
	Area* FullReDraw (void);
};

class PANGOTUI_API Titlebar  : public Area {
public:
	int draghandlex;
	//int draghandley;
	Titlebar(Area* _parent, std::string _title, int _x, int _y, int _w,  bool _visible=true,Area* aoi=NULL);

};

class PANGOTUI_API IconButton : public Area {
public:
	IconButton(Area* _parent, std::string _title, int _x, int _y, Uint8 c, bool isclose=false,  bool _visible=true, Area* aoi=NULL);
};


class PANGOTUI_API ScrollbarVertical : public Area {
public:
	IconButton* up_button, * down_button;
	//int range, pos;
	//Area* textarea;
	int sizeofdragger, draggerpos, num_of_sb_positions, window_ratio, line_positions;
	//int bottomcheck;
	ScrollbarVertical(Area* _parent, std::string _title, int _x, int _y, int _h,  bool _visible=true, Area* aoi=NULL);
	Area* FullReDraw(void) override;
	void ReCalcSize(bool draggerposisright);
};
class PANGOTUI_API ScrollbarHorizontal: public Area { //TODO: THIS HAS NEVER BEEN TESTED IT COULD BE BROKEN. IT'S NOT TIED IN TO WINDOWS YET
public:
	IconButton* left_button, * right_button;
	//int range, pos;
	//Area* textarea;
	int sizeofdragger, draggerpos, num_of_sb_positions, window_ratio, line_positions;
	//int bottomcheck;
	ScrollbarHorizontal(Area* _parent, std::string _title, int _x, int _y, int _w,  bool _visible=true, Area* aoi=NULL);
	Area* FullReDraw(void) override;
	void ReCalcSize(bool draggerposisright);
};

class PANGOTUI_API Slider: public Area { //TODO: THIS HAS NEVER BEEN TESTED IT COULD BE BROKEN. IT'S NOT TIED IN TO WINDOWS YET ??i think this comment is from when it was hor sbar
public:
	int value;
	NonEventLambda OnValueChange;
	NonEventLambda OnValueChangedManually;
	Slider(Area* _parent, std::string _title, int _x, int _y, int _w,  bool _visible=true, Area* aoi=NULL);
	void SetValue(int v);
	//Area* FullReDraw(void) override;
};

class PANGOTUI_API Listbochs : public Area {
public:
	std::list<std::string> items;
	int value;						//selected line
	int size;						//how many items are in it at the moment	
	int capacity;					//max number of lines possible
	ScrollbarVertical* AssociatedVScroller;
	void AddItem(std::string s);
	void RemoveItem(int index);
	void RemoveAll(void);
	Listbochs(Area* _parent, std::string _title, const Rectangle& r, int _capacity, bool _visible=true, Area* aoi=NULL);
	void Listbochs::ScrollUpOneLineIfAppropriate(void);
	void Listbochs::ScrollDownOneLineIfAppropriate(void);

};

class PANGOTUI_API Window : public Area {
public:
	IconButton* closebutton;
	IconButton* maxbutton;
	IconButton* minbutton;
	Titlebar* titlebar;
	//Window(std::string _title, int _x, int _y, int _w, int _h, Uint8 windowflags, Area* aoi=NULL);
	Window(std::string _title, Rectangle r, Uint8 windowflags, bool _visible=true,Area* aoi=NULL);
};

class PANGOTUI_API MessageLog : public Area {
public:
	ScrollbarVertical* AssociatedVScroller;
	//MessageLog(Area* _parent, std::string _title, int _x, int _y, int _w, int _h, int total_lines, Area* aoi=NULL);
	MessageLog(Area* _parent, std::string _title, Rectangle r, int total_lines,bool _visible=true, Area* aoi=NULL);
	void MessageLog::ScrollUpOneLineIfAppropriate(void);
	void MessageLog::ScrollDownOneLineIfAppropriate(void);
};





}