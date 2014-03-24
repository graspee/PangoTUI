#pragma once
#include "PangoTUI.h"


#include "Rectangle.h"
#include "CharCell.h"
#include <vector>
#include "PangoEngine.h"
#include "Palette.h"
#include "Bitmap.h"
#include "Events.h"

namespace PangoTUI {

	enum WindowFlags {
		WFLAG_HAS_TITLEBAR	= 0x01,
		WFLAG_HAS_RESIZE	= 0x02,
		WFLAG_HAS_MINIMIZE	= 0x04,
		WFLAG_HAS_MAXIMIZE	= 0x08,
		WFLAG_HAS_CLOSE		= 0x10,
		WFLAG_HAS_STATUSBAR	= 0x20,
		WFLAG_HAS_HSCROLL	= 0x40,
		WFLAG_HAS_VSCROLL	= 0x80
	};

	struct FrameChars {
		Uint8 topleft, topright, botleft, botright, vert, horiz;
		FrameChars(Uint8 a,Uint8 b,Uint8 c,Uint8 d, Uint8 e,Uint8 f):
			topleft(a),topright(b),botleft(c),botright(d),vert(e),horiz(f){}
	};

	
	
	class PANGOTUI_API PangoEngine; //fwd ref, circular include

//An area represents anything on the screen from the desktop itself, through windows to widgets like buttons and scrollbars
class PANGOTUI_API Area
{
public:
	bool visible;
	//static FrameChars FrameSingleLine={218,191,192,217,179,196};
	static FrameChars FrameSingleLine;
	static FrameChars FrameButtonBorder;

	Rectangle visual_area;		
	Rectangle virtual_area;
	bool doesitscroll;
	CharCell* chargrid;
	Area* parent;
	Area* AreaOfInterest;
	std::vector<Area*> children;
	static PangoEngine* engine;//!
	Ecrayola pen,paper;
	int cursorx,cursory;
	bool messagelog;
	int logoffset;
	int ScreenToAreaX, ScreenToAreaY; //modifiers to add to screen co-ords to get area-based co-ords
	bool NeedToCalcScreenToAreaOffsets; //catchy. it's a flag we set so we only calc the screen to area modifiers once. if we move the area we set it again
	

	EventLambda OnDragBegin,OnLeftDragBegin,OnRightDragBegin,	OnMiddleDragBegin;
	EventLambda OnDragEnd,	OnLeftDragEnd,	OnRightDragEnd,		OnMiddleDragEnd;
	EventLambda OnDrag,		OnLeftDrag,		OnRightDrag,		OnMiddleDrag;

	EventLambda OnClick,	OnLeftClick,	OnRightClick,		OnMiddleClick;
	EventLambda OnMausDown,	OnLeftMausDown,	OnRightMausDown,	OnMiddleMausDown;
	EventLambda OnMausUp,	OnLeftMausUp,	OnRightMausUp,		OnMiddleMausUp;
	
	EventLambda OnMausEnter;
	EventLambda OnMausLeeb;
	EventLambda OnMausMoob;																			//maus moob is when the maus changes pos but doesn't enter or leave an area
	
	EventLambda OnKeyPress;
	EventLambda OnKeyUp;
	EventLambda OnKeyDown;

	EventLambda OnWheelUp;
	EventLambda OnWheelDown;

	NonEventLambda OnGainFocus, OnLoseFocus;

	

	std::string title;
	Area(const Rectangle& vis, const Rectangle& virt, bool messlog=false);
	Area(const Rectangle& vis,  bool messlog=false);
	~Area(void);

	inline int TOTALWIDTH(void);
	inline int TOTALHEIGHT(void);
	inline int PARENTX(void);
	inline int PARENTY(void);
	inline int ORIGINX(void);
	inline int ORIGINY(void);
	inline int WIDTHONSCREEN(void);
	inline int HEIGHTONSCREEN(void);

	inline int SCREENXTOAREAX(int x);
	inline int SCREENYTOAREAY(int y);

	friend class PANGOTUI_API PangoEngine;
	friend class PANGOTUI_API Surface;

	virtual Area* FullReDraw(void);
	void DrawChar(unsigned char c,Uint32 fg,Uint32 bg,int x,int y,bool updategrid=true,bool updatescreen=true, bool sdlupdaterect=true);
	void DrawString(std::string s,Uint32 fg,Uint32 bg,int x,int y,bool updategrid=true,bool updatescreen=true);

	void DrawCharEx(unsigned char c,Uint32 fg,Uint32 bg,int x,int y,bool updategrid=true,bool updatescreen=true, bool sdlupdaterect=true,bool usefg=false,bool usebg=false,bool usec=false);
	void PrintString(std::string s,Ecrayola fg=DEFAULTCOLOUR,Ecrayola bg=DEFAULTCOLOUR,int x=32500,int y=32500);
	void Area::DrawFrame(Uint32 fg,Uint32 bg,int x,int y,int w,int h,bool updategrid=true, bool updatescreen=true);
	void Area::DrawCustomFrame(Uint32 fg,Uint32 bg, const Rectangle& r,const FrameChars& fc=FrameSingleLine, bool updategrid=true, bool updatescreen=true);
	void Area::BresLine(Uint8 c,Uint32 fg,Uint32 bg,int x0,int y0,int x1,int y1,bool updategrid=true, bool updatescreen=true);
	void Area::DrawRect(Uint8 character,Uint32 fg,Uint32 bg,int x,int y,int x2,int y2,bool updategrid=true, bool updatescreen=true);
	void Area::DrawFilledRect(Uint8 character,Uint32 fg,Uint32 bg,int x,int y,int x2,int y2,bool updategrid=true, bool updatescreen=true);
	void Area::DrawCircle(Uint8 c,Uint32 fg,Uint32 bg,int x0, int y0, int radius,bool updategrid =true, bool updatescreen=true);
	void Area::DrawFilledCircle(Uint8 c,Uint32 fg,Uint32 bg,int x0, int y0, int radius,bool updategrid=true, bool updatescreen=true);
	void Area::DrawSprite(int x,int y,int w,int h,unsigned char* array, bool updategrid=true, bool updatescreen=true);
	void Area::DrawBitmap(int x,int y, Bitmap* source, bool updategrid=true, bool updatescreen=true);
	void Area::SetOrigin(int _x, int _y);
	virtual void Area::ScrollUpOneLineIfAppropriate(void);
	virtual void Area::ScrollDownOneLineIfAppropriate(void);
	virtual void Area::ScrollLeftOneLineIfAppropriate(void);
	virtual void Area::ScrollRightOneLineIfAppropriate(void);
	void Area::DumpToConsole(void);
	void CommonSetup(void);
	Area* Area::AddSub(Area* a);
	void Area::Clear(Uint32 fg, Uint32 bg);
	void Area::CopyChar(int sx,int sy, int dx, int dy,bool a,bool b,bool c);
	void Area::Focus(void);
	void Area::DeFocus(void);
};

}