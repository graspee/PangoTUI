#include "stdafx.h"
#include "widgets.h"


using namespace PangoTUI;

//Window::Window(std::string _title, int _x, int _y, int _w, int _h, Uint8 windowflags, Area* aoi):
Window::Window(std::string _title, PangoTUI::Rectangle r, Uint8 windowflags, bool _visible, Area* aoi):
	//Area(Rectangle(_x,_y,_w,_h,true))
	Area(r)
{
	engine->desktop->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi;
	visible=_visible;;
	
	
	//if(_h<6 || _w<6){
	if(r.height<6 || r.width <6){
		std::cerr << "Area::CreateWindow: window too small (min 6x6)" << std::endl;
	}
	//DrawFrame(engine->colour_window_border,engine->colour_bg,0,0,_w,_h,true,false);
	DrawCustomFrame(engine->colour_window_border,engine->colour_bg,Rectangle(0,0,r.width,r.height,true),FrameSingleLine,true,false);
	if(windowflags & WFLAG_HAS_TITLEBAR){
		//int potential_w=_w-2;
		int potential_w=r.width-2;
		DrawChar(195,engine->colour_window_border,engine->colour_bg,0,2,true,false,false);
		//for(int t=1;t<_w-1;t++)
		for(int t=1;t<r.width-1;t++)
			DrawChar(196,engine->colour_window_border,engine->colour_bg,t,2,true,false,false);
		//DrawChar(180,engine->colour_window_border,engine->colour_bg,_w-1,2,true,false,false);
		DrawChar(180,engine->colour_window_border,engine->colour_bg,r.width-1,2,true,false,false);
		if(windowflags & WFLAG_HAS_CLOSE){
			--potential_w;
			//closebutton=new IconButton(this,"close button",_w-2,1,184,true);
			closebutton=new IconButton(this,"close button",r.width-2,1,184,true);
			closebutton->OnLeftClick=[](Area* a, Event& e) -> void  {std::cout << "close clicked";exit(1);};
		}
		if(windowflags & WFLAG_HAS_MAXIMIZE){
			--potential_w;
			//maxbutton=new IconButton(this,"maximize button",_w-3,1,216);
			maxbutton=new IconButton(this,"maximize button",r.width-3,1,216);
		}
		if(windowflags & WFLAG_HAS_MINIMIZE){
			--potential_w;
			//minbutton=new IconButton(this,"minimize button",_w-4,1,22);
			minbutton=new IconButton(this,"minimize button",r.width-4,1,22);
		}
		//std::cout << "potential w is " << potential_w << std::endl;
		
		titlebar=new Titlebar(this,_title,1,1,potential_w);
	}
	FullReDraw();
	
}

CustomWidget::CustomWidget(Area* _parent,std::string _title, const PangoTUI::Rectangle& r, bool _visible, Area* aoi):
	Area(r)
{
	_parent->AddSub(this);
	title=_title;
	AreaOfInterest=aoi; visible=_visible;
	OnFullReDraw=nullptr;
}

IconButton::IconButton(Area* _parent, std::string _title, int _x, int _y, Uint8 c, bool isclose, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y,1,1,true))
{
	_parent->AddSub(this);
	title=_title;
	AreaOfInterest=aoi; visible=_visible;

	DrawChar(c,engine->colour_icon,engine->colour_bg,0,0,true,true,true);
	if(isclose)
		OnMausEnter=[](Area* a, Event& e) -> void {a->DrawCharEx(0,engine->colour_icon_hi_close,0,0,0,true,true,true,false,true,true);};
	else
		OnMausEnter=[](Area* a, Event& e) -> void {a->DrawCharEx(0,engine->colour_icon_hi,0,0,0,true,true,true,false,true,true);};
		OnMausLeeb=[](Area* a, Event& e) -> void {a->DrawCharEx(0,engine->colour_icon,0,0,0,true,true,true,false,true,true);};		
	
}
void ScrollbarVertical::ReCalcSize(bool draggerposisright){
	window_ratio=							(AreaOfInterest->TOTALHEIGHT()*1000) / (AreaOfInterest->HEIGHTONSCREEN());
	sizeofdragger=							HEIGHTONSCREEN()*1000 / window_ratio;
	num_of_sb_positions=					HEIGHTONSCREEN()-sizeofdragger;
	line_positions=							AreaOfInterest->TOTALHEIGHT()-AreaOfInterest->HEIGHTONSCREEN();
	
	//std::cout << "size dragger " << sizeofdragger << " num sb pos " << num_of_sb_positions << " line positions " << line_positions << std::endl;
	
	if (num_of_sb_positions!=0){ //first attempt to stop divide by 0
		if(draggerposisright){
			AreaOfInterest->virtual_area.y=
				(  ((draggerpos*1000)/num_of_sb_positions)   *   line_positions)     /1000;
			//std::cout << "v.y " << AreaOfInterest->virtual_area.y << std::endl;
		} else {
			draggerpos=
				(  ((AreaOfInterest->virtual_area.y*1000)/line_positions)  *  num_of_sb_positions)  /1000;
			//std::cout << "dpos " << draggerpos << std::endl;
		}
	}

	//float lpb=(float)(AreaOfInterest->TOTALHEIGHT()) / ((float)HEIGHTONSCREEN());
	//float sod=(float)(AreaOfInterest->HEIGHTONSCREEN()) / lpb;
	//linesperblock=(int)lpb;
	//sizeofdragger=(int)sod;

}

ScrollbarVertical::ScrollbarVertical(Area* _parent, std::string _title, int _x, int _y, int _h, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y+1,1,_h-2,true))//, range(_range), pos(_pos) //was h-1
{
	_parent->AddSub(this);
	title=_title;
	AreaOfInterest=aoi; visible=_visible;;
	
	up_button=new IconButton(parent,"up button",_x,_y,30,false,visible,this);
	up_button->OnClick=[](Area* a, Event& e) -> void {a->AreaOfInterest->AreaOfInterest->ScrollUpOneLineIfAppropriate();};		
	
	down_button=new IconButton(parent,"down button",_x,_y+_h-1,31,false,visible,this);
	down_button->OnClick=[](Area* a, Event& e) -> void {a->AreaOfInterest->AreaOfInterest->ScrollDownOneLineIfAppropriate();};		
	
	
	OnWheelUp=[](Area* a, Event& e) -> void {a->AreaOfInterest->ScrollUpOneLineIfAppropriate();};		
	OnWheelDown=[](Area* a, Event& e) -> void {a->AreaOfInterest->ScrollDownOneLineIfAppropriate();};	
	
	//linesperblock=AreaOfInterest->TOTALHEIGHT()/(_h-2); 
	//sizeofdragger=AreaOfInterest->HEIGHTONSCREEN()/linesperblock;
	draggerpos=0;
	ReCalcSize(true); //not sure 

	OnLeftDrag=[](Area* a, Event& e) -> void { 
		if(!e.DragOutOfBoundsP){
			int relativey=(a->SCREENYTOAREAY(e.CurrentScreenY)); //std::cout << "rely " << relativey <<std::endl;
			//if(relativey < a->TOTALHEIGHT()-((ScrollbarVertical*)a)->sizeofdragger+1){
			//	a->AreaOfInterest->virtual_area.y = relativey * ((ScrollbarVertical*)a)->linesperblock; //line offset= sb pos * linesperblock
			//	a->FullReDraw();
			//	a->AreaOfInterest->FullReDraw();
			//}
			
			//int number_of_sb_pos=a->HEIGHTONSCREEN()-((ScrollbarVertical*)a)->sizeofdragger+1; std::cout << "num sb pos = " << number_of_sb_pos << std::endl;
			//a->AreaOfInterest->virtual_area.y=
			//	( ((ScrollbarVertical*)a)->num_of_sb_positions  * relativey) / 
			//		((ScrollbarVertical*)a)->line_positions; //std::cout << " vir.y= " << a->AreaOfInterest->virtual_area.y << std::endl;

			
			if( relativey>(SCROLLBAR(a)->draggerpos )){
				if( (SCROLLBAR(a)->draggerpos + SCROLLBAR(a)->sizeofdragger) < SCROLLBAR(a)->TOTALHEIGHT() ){
					SCROLLBAR(a)->draggerpos++;
					SCROLLBAR(a)->ReCalcSize(true);
				}

			} else {
				if( (SCROLLBAR(a)->draggerpos > 0)){
					SCROLLBAR(a)->draggerpos--;
					SCROLLBAR(a)->ReCalcSize(true);
				}
			}


			//SCROLLBAR(a)->draggerpos=relativey;
			

			a->FullReDraw();
			a->AreaOfInterest->FullReDraw();

		}
	};

	FullReDraw();

}

Area*
ScrollbarVertical::FullReDraw(void)  {
	//std::cout << "SCROOLBAR OVERRIDE! LOLZ ";
	//don't draw scrollbar if control is not using the capacity, e.g. listbox
	//later comment on above comment wondering why i'm not checking. i think answer is that if listbox doesn't need a 
	//scrollbar then it's set to not visible and so fullredraw will never be called on it

	for(int i=0;i<this->TOTALHEIGHT();i++)DrawChar(178,engine->colour_window_border,engine->colour_bg,0,i);//was th-2
	
	//int draggerpos=AreaOfInterest->ORIGINY()/linesperblock;
	
	for(int i=0;i<sizeofdragger;i++){
		DrawChar(219,engine->colour_window_border,engine->colour_bg,0,i+draggerpos);
	}
	up_button->FullReDraw();
	down_button->FullReDraw();
	Area::FullReDraw();
	return this;
}

Area* BorderlessButton::FullReDraw(void){
	DrawString(title,TextColour,engine->colour_bg,0,0);
	Area::FullReDraw();
	return this;
}
Area* Button::FullReDraw(void){
	DrawString(title,TextColour,engine->colour_bg,1,1);
	Area::FullReDraw();
	return this;
}

//MessageLog::MessageLog(Area* _parent, std::string _title, int _x, int _y, int _w, int _h, int total_lines, Area* aoi):
MessageLog::MessageLog(Area* _parent, std::string _title, PangoTUI::Rectangle r, int total_lines, bool _visible, Area* aoi):
	//Area(Rectangle(_x,_y,_w-1,_h,true),Rectangle(0,0,_w-1,total_lines,true),true)
	Area(Rectangle(r.x,r.y,r.width-1,r.height,true),Rectangle(0,0,r.width-1,total_lines,true),true)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;

	//SetOrigin(0,total_lines-_h);
	//SetOrigin(0,total_lines-r.height);

	//AssociatedVScroller=new ScrollbarVertical(parent,"vscroller",_x+_w-1,_y,_h,this);
	AssociatedVScroller=new ScrollbarVertical(parent,"vscroller",r.x+r.width-1,r.y,r.height,true,this);
	OnWheelUp=[](Area* a, Event& e) -> void {a->ScrollUpOneLineIfAppropriate();};		
	OnWheelDown=[](Area* a, Event& e) -> void {a->ScrollDownOneLineIfAppropriate();};
	SetOrigin(0,total_lines-r.height);
	AssociatedVScroller->ReCalcSize(false);
	AssociatedVScroller->FullReDraw();
}

void MessageLog::ScrollUpOneLineIfAppropriate(void){
	if(virtual_area.y>0){
		virtual_area.y--;
		AssociatedVScroller->ReCalcSize(false);
		FullReDraw();
		AssociatedVScroller->FullReDraw();
	}
}

void MessageLog::ScrollDownOneLineIfAppropriate(void){
	if(virtual_area.y < (virtual_area.height-visual_area.height)  ){
		virtual_area.y++;
		AssociatedVScroller->ReCalcSize(false);
		FullReDraw();
		AssociatedVScroller->FullReDraw();

	}
}
//just copies of above- not ideal, but works for now
void Listbochs::ScrollUpOneLineIfAppropriate(void){
	if(virtual_area.y>0){
		virtual_area.y--;
		AssociatedVScroller->ReCalcSize(false);
		FullReDraw();
		AssociatedVScroller->FullReDraw();
	}
}

void Listbochs::ScrollDownOneLineIfAppropriate(void){
	if(virtual_area.y < (virtual_area.height-visual_area.height)  ){
		virtual_area.y++;
		AssociatedVScroller->ReCalcSize(false);
		FullReDraw();
		AssociatedVScroller->FullReDraw();

	}
}

Titlebar::Titlebar(Area* _parent, std::string _title, int _x, int _y, int _w, bool _visible, Area* aoi) :
	Area(Rectangle(_x, _y, _w, 1, true))
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;
	DrawString(_title,engine->colour_window_border,engine->colour_bg,0,0);


	OnLeftDragBegin=[](Area* a, Event& e) -> void { 
		//std::cout << a->SCREENXTOAREAX(e.CurrentScreenX) << " " << a->SCREENYTOAREAY(e.CurrentScreenY) << std::endl;
		((Titlebar*)a)->draghandlex=a->SCREENXTOAREAX(e.CurrentScreenX);
		//((Titlebar*)a)->draghandley=a->SCREENXTOAREAY(e.CurrentScreenY);
	};

	OnLeftDrag=[](Area* a, Event& e) -> void {
		//std::cout << a->SCREENXTOAREAX(e.CurrentScreenX) << " " << a->SCREENYTOAREAY(e.CurrentScreenY) << std::endl;
		a->parent->visual_area.x=e.CurrentScreenX - ((Titlebar*)a)->draghandlex;
		a->parent->visual_area.y=e.CurrentScreenY ; //- a->SCREENYTOAREAY(e.CurrentScreenY);
		//a->parent->FullReDraw();
		a->engine->desktop->FullReDraw();
	};

	OnRightClick=[](Area* a, Event& e) -> void { exit(0); };
}

Button::Button(Area* _parent, std::string _title, Uint32 _butclr, int _x, int _y, bool isdefault, bool _visible, Area* aoi) : 
	Area(Rectangle(_x,_y,_title.length()+2,3,true)), TextColour(_butclr)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;

	DrawCustomFrame(engine->colour_window_border,engine->colour_bg,Rectangle(0,0,visual_area.width,visual_area.height,true),FrameButtonBorder);
	DrawString(_title,TextColour,engine->colour_bg,1,1);

	OnMausEnter=[](Area* a, Event& e) -> void {a->DrawString(a->title,engine->colour_icon_hi,engine->colour_bg,1,1);};
	OnMausLeeb=[](Area* a, Event& e) -> void {a->DrawString(a->title,BUTTON(a)->TextColour,engine->colour_bg,1,1);};		
}

BorderlessButton::BorderlessButton(Area* _parent, std::string _title, Uint32 _butclr, int _x, int _y, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y,_title.length(),1,true)), TextColour(_butclr)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;

	DrawString(_title,TextColour,engine->colour_bg,0,0);

	OnMausEnter=[](Area* a, Event& e) -> void {a->DrawString(a->title,engine->colour_icon_hi,engine->colour_bg,0,0);};
	OnMausLeeb=[](Area* a, Event& e) -> void {a->DrawString(a->title,BORDERLESSBUTTON(a)->TextColour,engine->colour_bg,0,0);};		
}


Checkbochs::Checkbochs(Area* _parent, std::string _title, int _x, int _y, bool _value, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y,2+_title.length(),1,true)),
	value(_value)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;

	DrawChar(value?'x':254,engine->colour_bg,engine->colour_window_border,0,0);
	DrawString(title,engine->colour_icon,engine->colour_bg,2,0);
	
//((Checkbochs*)a)->value
	OnMausEnter=[](Area* a, Event& e) -> void {a->DrawString(a->title,engine->colour_bg,engine->colour_window_border,2,0);};
	OnMausLeeb= [](Area* a, Event& e) -> void {a->DrawString(a->title,engine->colour_window_border,engine->colour_bg,2,0);};

	OnLeftClick=[](Area* a, Event& e) -> void {
		((Checkbochs*)a)->value=!((Checkbochs*)a)->value;
		a->DrawChar(((Checkbochs*)a)->value?'x':254,engine->colour_bg,engine->colour_window_border,0,0);
	};
}

//RadioButtonz::RadioButtonz(Area* _parent, std::string _title, const std::vector<const std::string>& _labels,const PangoTUI::Rectangle& r, Uint8 _value, Area* aoi):
//RadioButtonz::RadioButtonz(Area* _parent, std::string _title,  std::vector< std::string> _labels,const PangoTUI::Rectangle& r, Uint8 _value, Area* aoi):
RadioButtonz::RadioButtonz(Area* _parent, std::string _title,  std::string _labels[],const PangoTUI::Rectangle& r, Uint8 _value, bool _visible, Area* aoi):
	Area(Rectangle(r)),
	value(_value)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;
	OnValueChange=nullptr;
	
	for(int down=0;down<r.height;down++){
		DrawChar(value==down?7:9,engine->colour_icon,engine->colour_bg,0,down);
		DrawString(_labels[down],engine->colour_window_border,engine->colour_bg,2,down);
	}

	OnLeftClick=[](Area* a, Event& e) -> void {
		Uint8 ansa=a->SCREENYTOAREAY(e.CurrentScreenY);
		Uint8 current=((RadioButtonz*)a)->value;
		if(ansa!=current){
			a->DrawChar(9,engine->colour_icon,engine->colour_bg,0,current);
			a->DrawChar(7,engine->colour_icon,engine->colour_bg,0,ansa);
			((RadioButtonz*)a)->value=ansa;
			if(((RadioButtonz*)(a))->OnValueChange!=nullptr){
				((RadioButtonz*)(a))->OnValueChange(a,e);
			}

		}
	};

	OnMausMoob=[](Area* a, Event& e) -> void {
		if(e.CurrentScreenY!=e.LastScreenY){
			//std::cout << "THIS " << e.CurrentScreenY << " LAST " << e.LastScreenY << std::endl;
			//highlight new line
			Uint8 current=a->SCREENYTOAREAY(e.CurrentScreenY);
			Uint8 last=a->SCREENYTOAREAY(e.LastScreenY);
			for(int x=2;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,engine->colour_bg,engine->colour_icon,x,current,true,true,true,false,false,true);
			//check if old line is on the control and if so whack it
			if(last>=0 && last<a->HEIGHTONSCREEN()){
				for(int x=2;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,engine->colour_icon,engine->colour_bg,x,last,true,true,true,false,false,true);
			}

		}

	};

	OnMausLeeb=[](Area* a, Event& e) -> void {
		Uint8 last=a->SCREENYTOAREAY(e.LastScreenY);
		for(int x=2;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,engine->colour_icon,engine->colour_bg,x,last,true,true,true,false,false,true);
	};
	OnMausEnter=[](Area* a, Event& e) -> void {
		Uint8 current=a->SCREENYTOAREAY(e.CurrentScreenY);
		for(int x=2;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,engine->colour_bg,engine->colour_icon,x,current,true,true,true,false,false,true);
	};
}

ProgressBar::ProgressBar(Area* _parent, std::string _title, Ecrayola fg, int _x,int _y,int _w, int _min, int _max, int _value, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y,_w,1,true)),
	minimum(_min), maximum(_max), value(_value), perblock( (_max-_min) / _w ),
	colour(engine->palette->u32vals[fg])
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;

	FullReDraw();
}
void ProgressBar::ReCalc(void){
	perblock=  ((float)maximum-(float)minimum) / (float)WIDTHONSCREEN() ;
}

Area* ProgressBar::FullReDraw  (void){
	float drawp=(float)value/perblock;

	for(int x=0;x< (int)drawp; x++)
		DrawChar(219,colour,engine->colour_bg,x,0);

	for(int x=(int)drawp; x<WIDTHONSCREEN(); x++)
		DrawChar(178,engine->colour_icon,engine->colour_bg,x,0);
return this;
}

Label::Label(Area* _parent, std::string _title,Ecrayola fg, Ecrayola bg, const PangoTUI::Rectangle& r,  bool _visible, Area* aoi):
	Area(r),
	colour_bg(engine->palette->u32vals[bg]), colour_fg(engine->palette->u32vals[fg]),
	cfg(fg), cbg(bg)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;

	DrawFilledRect(' ',colour_fg,colour_bg,0,0,r.width-1,r.height-1);

	PrintString(title,cfg,cbg);
}

SingleLineTextEntry::SingleLineTextEntry(Area* _parent, std::string _title, int _x, int _y, int _width, int _maxlen, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y,_width+2,3,true),Rectangle(0,0,(_maxlen>=_width)?_maxlen+2:_width+2,3,true)),
	cursorx(0),
	value("")
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;

	//DrawFrame(engine->colour_window_border,engine->colour_bg,0,0,_width+2,3);
	FullReDraw();

	OnLeftClick=EVENTLAMBDA {
		a->Focus();
	};

	OnKeyDown=EVENTLAMBDA {
		//ESC lose focus
		//TAB lose focus, setting focus to areaofinterest if set (for chains of controls on a form)
		//backspace delete last char if not len 0
		//'normal' characters add to value if not len maxlen (max visible width)
		std::string &s=(SINGLELINETEXTENTRY(a)->value);
		switch(e.key){
			case SDLK_ESCAPE:
				if(SINGLELINETEXTENTRY(a)->OnSubmit!=nullptr)SINGLELINETEXTENTRY(a)->OnSubmit(a);
				a->DeFocus();
				break;
			case SDLK_TAB: case SDLK_KP_ENTER: case SDLK_RETURN:
				if(SINGLELINETEXTENTRY(a)->OnSubmit!=nullptr)SINGLELINETEXTENTRY(a)->OnSubmit(a);
				if(a->AreaOfInterest!=nullptr)a->AreaOfInterest->Focus(); else a->DeFocus();
				break;
			case SDLK_BACKSPACE:
				
				if(SINGLELINETEXTENTRY(a)->cursorx>0){
						s.erase(s.size() - 1);
					
						if(	SINGLELINETEXTENTRY(a)->cursorx<a->TOTALWIDTH()-2)
							a->DrawChar(32,engine->colour_window_border,engine->colour_icon_hi,SINGLELINETEXTENTRY(a)->cursorx+1,1); //space where cursor was
						SINGLELINETEXTENTRY(a)->cursorx--;
						a->DrawChar(219,engine->colour_window_border,engine->colour_icon_hi,SINGLELINETEXTENTRY(a)->cursorx+1,1); //space where cursor was

						if(SINGLELINETEXTENTRY(a)->OnValueChangedManually!=nullptr)SINGLELINETEXTENTRY(a)->OnValueChangedManually(a);
				}
				break;
			default:
				
				if(	SINGLELINETEXTENTRY(a)->cursorx<a->TOTALWIDTH()-2){//s.length()
					s+=((e.key_unicode));
					
					a->DrawChar(e.key_unicode,engine->colour_window_border,engine->colour_icon_hi,SINGLELINETEXTENTRY(a)->cursorx+1,1); //space where cursor was
					SINGLELINETEXTENTRY(a)->cursorx++;
					if(	SINGLELINETEXTENTRY(a)->cursorx<a->TOTALWIDTH()-2)a->DrawChar(219,engine->colour_window_border,engine->colour_icon_hi,SINGLELINETEXTENTRY(a)->cursorx+1,1); //space where cursor was

					
					if(SINGLELINETEXTENTRY(a)->OnValueChangedManually!=nullptr)SINGLELINETEXTENTRY(a)->OnValueChangedManually(a);
				} else {
					//briefly flash red my friend. to let you know the blood is flowing. flowing. flowing dripping down.
					SINGLELINETEXTENTRY(a)->ErrorDraw();
					engine->Pause(500);
					a->FullReDraw();
				}


			break;
		}

	};



}

Area* SingleLineTextEntry::FullReDraw(void){
	//if it doesn't have focus it should be bg and fg colour normal, show the value, no clue on bounds of box
	//if it has focus it should be dark blue background white fg
	//if you try to type when it's full it should briefly have a red bg (icon hi close) (not handled by this method)
	
	Uint32 bgcol;
	bgcol=(engine->Focus==this)?engine->colour_icon_hi:engine->colour_bg;

	DrawFrame(engine->colour_window_border,engine->colour_bg,0,0,WIDTHONSCREEN(),3);


	int x=1;
	for(auto i:value){
		DrawChar(i,engine->colour_window_border,bgcol,x++,1);
	}
	//draw the remainder of the string as pure bg
	if(engine->Focus==this && x<TOTALWIDTH()-1)DrawChar(' ',bgcol,engine->colour_window_border,x++,1);

	for (; x < TOTALWIDTH()-1; x++)
	{
		DrawChar(' ',0,bgcol,x,1);
	}

	return this;
}
void SingleLineTextEntry::ErrorDraw(void){

	int x=1;
	for(auto i:value){
		DrawChar(i,engine->colour_window_border,engine->colour_icon_hi_close,x++,1);
	}
	//draw the remainder of the string as pure bg
	for (; x < TOTALWIDTH()-2; x++)
	{
		DrawChar(' ',0,engine->colour_icon_hi_close,x,1);
	}

}
void SingleLineTextEntry::SetValue(std::string v){
	value=v;
	cursorx=v.length();
	FullReDraw();
	if(OnValueChange!=nullptr)OnValueChange(this);
}


Listbochs::Listbochs(Area* _parent, std::string _title, const PangoTUI::Rectangle& r, int _capacity, bool _visible, Area* aoi):
	Area(Rectangle(r.x,r.y,r.width-1,r.height,true),Rectangle(0,0,r.width-1,_capacity,true)),
	capacity(_capacity), 
	value (-1),
	size (0)
{
	_parent->AddSub(this);
	this->title=_title;
	AreaOfInterest=aoi; visible=_visible;;

	//we've already created our "area" at its max size, so the chargrid capacity is there
	//now we rein it back in so it appears to be the size of the control, empty
	virtual_area.height=visual_area.height;
	AssociatedVScroller=new ScrollbarVertical(parent,"vscroller",r.x+r.width-1,r.y,r.height,false,this);
	OnWheelUp=[](Area* a, Event& e) -> void {a->ScrollUpOneLineIfAppropriate();};		
	OnWheelDown=[](Area* a, Event& e) -> void {a->ScrollDownOneLineIfAppropriate();};
	
	OnLeftClick=[](Area* a, Event& e) -> void {
		int traction=a->SCREENYTOAREAY(e.CurrentScreenY);
		if(traction==LISTBOCHS(a)->value){ //if we clicked selected
			LISTBOCHS(a)->value=-1;
			for(int i=0;i<a->TOTALWIDTH();i++)a->DrawCharEx(0,engine->colour_window_border,engine->colour_bg,i,traction,true,true,true,false,false,true);
		} else {
			if(LISTBOCHS(a)->value!=-1)for(int i=0;i<a->TOTALWIDTH();i++)a->DrawCharEx(0,engine->colour_window_border,engine->colour_bg,i,LISTBOCHS(a)->value,true,true,true,false,false,true);
			LISTBOCHS(a)->value=traction;
			for(int i=0;i<a->TOTALWIDTH();i++)a->DrawCharEx(0,engine->colour_icon_hi,engine->colour_bg,i,traction,true,true,true,false,false,true);
		}
	};
	//almost the same as radio buttons for moob enter and leave- amalgamate somehow TODO
	OnMausMoob=[](Area* a, Event& e) -> void {
		if(e.CurrentScreenY!=e.LastScreenY){
			Uint8 current=a->SCREENYTOAREAY(e.CurrentScreenY);
			Uint8 last=a->SCREENYTOAREAY(e.LastScreenY);
			//std::cout << "healthy " << (int)current << " " << (int)last << std::endl;
			for(int x=0;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,
				(LISTBOCHS(a)->value==current)?engine->colour_icon_hi:engine->colour_bg,
				engine->colour_icon,
				x,current,false,true,true,false,false,true);
			if(last>=0 && last<a->TOTALHEIGHT()){
				for(int x=0;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,
					(LISTBOCHS(a)->value==last)?engine->colour_icon_hi:engine->colour_icon,
					engine->colour_bg,
					x,last,false,true,true,false,false,true);
			}

		}

	};
	OnMausLeeb=[](Area* a, Event& e) -> void {
		Uint8 last=a->SCREENYTOAREAY(e.LastScreenY);
		for(int x=0;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,
			(LISTBOCHS(a)->value==last)?engine->colour_icon_hi:engine->colour_icon,
			engine->colour_bg,
			x,last,false,true,true,false,false,true);
	};
	OnMausEnter=[](Area* a, Event& e) -> void {
		Uint8 current=a->SCREENYTOAREAY(e.CurrentScreenY);
		for(int x=0;x<a->WIDTHONSCREEN();x++)a->DrawCharEx(0,
			(LISTBOCHS(a)->value==current)?engine->colour_icon_hi:engine->colour_bg,
			engine->colour_icon,
			x,current,false,true,true,false,false,true);
	};

	//OnRightClick=[](Area* a, Event& e) -> void {
	//	int traction=a->SCREENYTOAREAY(e.CurrentScreenY);
	//	LISTBOCHS(a)->RemoveItem(traction);
	//	std::cout << "rem " << traction << std::endl;
	//};
	
	//OnMiddleClick=[](Area* a, Event& e) -> void {
	//	LISTBOCHS(a)->RemoveAll();
	//};

	//AssociatedVScroller->visible=false;
	//AssociatedVScroller->up_button->visible=false;
	//AssociatedVScroller->down_button->visible=false;
	//FullReDraw();
}
void Listbochs::AddItem(std::string s){
	//check sanity
	if(size==capacity){
		std::cerr << "Listbochs::Additem: Attempt to add item when at capacity ( " << capacity << " )." << std::endl;
		return;
	}
	//list
	items.push_front(s);
	//vars
	size++;
	//visually
	if(size<visual_area.height+1){
		virtual_area.height=visual_area.height;
	} else {
		virtual_area.height=size;

		AssociatedVScroller->ReCalcSize(false);

		AssociatedVScroller->visible=true;
		AssociatedVScroller->up_button->visible=true;
		AssociatedVScroller->down_button->visible=true;
		AssociatedVScroller->FullReDraw();
	}


	
	DrawString(s,engine->colour_window_border,engine->colour_bg,0,size-1);

}

void Listbochs::RemoveItem(int index){
	//check sanity
	if(index>(size-1)){
		std::cerr << "Listbochs::Removeitem: Error specified Index > size ( index " << index << " size " << size  << " )." << std::endl;
		return;
	}
	//list
	std::list<std::string>::iterator i=items.begin();
	std::advance(i,index);
	items.erase(i);
	//vars
	
	if(value==index)value=-1; //if it was the selected thing that was deleted, nothing is selected now
	//visually
	

	for(int y=index;y<size-1;y++)
		for(int x=0; x<WIDTHONSCREEN(); x++)
			CopyChar(x,y+1,x,y,true,true,true);

	for(int x=0; x<WIDTHONSCREEN(); x++)
			DrawChar(' ',engine->colour_window_border,engine->colour_bg,x,size-1);
	//std::cout << "size was " << size << std::endl;


	size--;

	if(size<visual_area.height+1){										//if size now is less than or eq a screen
		virtual_area.height=visual_area.height;							//set virtual height

		if(AssociatedVScroller->visible==true){							//if scrollbar was visible before...
			AssociatedVScroller->visible=false;								//make it invisible and such
			AssociatedVScroller->up_button->visible=false;
			AssociatedVScroller->down_button->visible=false;
			AssociatedVScroller->FullReDraw();
		}

	} else {															//if we are bigger than screen now
		virtual_area.height=size;			

		AssociatedVScroller->ReCalcSize(false);							//bored of typing comments

		if(AssociatedVScroller->visible==false){
			AssociatedVScroller->visible=true;
			AssociatedVScroller->up_button->visible=true;
			AssociatedVScroller->down_button->visible=true;	
		}
		AssociatedVScroller->FullReDraw();
	}
}

void Listbochs::RemoveAll(void){
	//check sanity
	
	//list
	items.clear();
	//vars
	size=0;
	value=-1;
	//visually
	
	virtual_area.height=visual_area.height;		

	if(AssociatedVScroller->visible==true){							//if scrollbar was visible before...
		AssociatedVScroller->visible=false;								//make it invisible and such
		AssociatedVScroller->up_button->visible=false;
		AssociatedVScroller->down_button->visible=false;
		AssociatedVScroller->FullReDraw();
	}

	

	Clear(engine->colour_window_border,engine->colour_bg);
	FullReDraw();
}
void Slider::SetValue(int v){

	if(v!=value){
		DrawChar(196,engine->colour_window_border,engine->colour_bg,value,0);
		value=v;
		DrawChar(186,engine->colour_window_border,engine->colour_bg,value,0);
		NonEventLambda nel=OnValueChange;
		if(nel!=nullptr)nel(this);
	}
}

Slider::Slider(Area* _parent, std::string _title, int _x, int _y, int _w, bool _visible, Area* aoi):
	Area(Rectangle(_x,_y,_w,1,true))//, range(_range), pos(_pos) //was h-1
{
	_parent->AddSub(this);
	title=_title;
	AreaOfInterest=aoi; visible=_visible;;
	
	value=0;
	
	//draw it
	DrawChar(186,engine->colour_window_border,engine->colour_bg,0,0);
	for(int i=1;i<_w;i++){
		DrawChar(196,engine->colour_window_border,engine->colour_bg,i,0);
	}


	OnLeftDrag=[](Area* a, Event& e) -> void { 
		//std::cout << e.DragOutOfBoundsP << " " ;
		if(!e.DragOutOfBoundsP){
			int relativex=(a->SCREENXTOAREAX(e.CurrentScreenX)); 
			if(relativex!=SLIDER(a)->value){
				a->DrawChar(196,engine->colour_window_border,engine->colour_bg,SLIDER(a)->value,0);
				SLIDER(a)->value=relativex;
				a->DrawChar(186,engine->colour_window_border,engine->colour_bg,SLIDER(a)->value,0);
				NonEventLambda nel=SLIDER(a)->OnValueChange;
				if(nel!=nullptr)nel(a);
				nel=SLIDER(a)->OnValueChangedManually;
				if(nel!=nullptr)nel(a);
		}
		}
	};

	OnLeftClick=[](Area* a,Event& e) -> void {
		int relativex=(a->SCREENXTOAREAX(e.CurrentScreenX)); 
		if(relativex!=SLIDER(a)->value){
			a->DrawChar(196,engine->colour_window_border,engine->colour_bg,SLIDER(a)->value,0);
			SLIDER(a)->value=relativex;
			a->DrawChar(186,engine->colour_window_border,engine->colour_bg,SLIDER(a)->value,0);
			NonEventLambda nel=SLIDER(a)->OnValueChange;
			if(nel!=nullptr)nel(a);
			nel=SLIDER(a)->OnValueChangedManually;
			if(nel!=nullptr)nel(a);
		}
	};


}
ScrollbarHorizontal::ScrollbarHorizontal(Area* _parent, std::string _title, int _x, int _y, int _w, bool _visible, Area* aoi):
	Area(Rectangle(_x+1,_y,_w-2,1,true))//, range(_range), pos(_pos) //was h-1
{
	_parent->AddSub(this);
	title=_title;
	AreaOfInterest=aoi; visible=_visible;;
	
	left_button=new IconButton(parent,"left button",_x,_y,17,false,visible,this);
	left_button->OnClick=[](Area* a, Event& e) -> void {a->AreaOfInterest->AreaOfInterest->ScrollLeftOneLineIfAppropriate();};		
	
	right_button=new IconButton(parent,"right button",_x+_w-1,_y,16,false,visible,this);
	right_button->OnClick=[](Area* a, Event& e) -> void {a->AreaOfInterest->AreaOfInterest->ScrollRightOneLineIfAppropriate();};		
	
	
	//OnWheelUp=[](Area* a, Event& e) -> void {a->AreaOfInterest->ScrollUpOneLineIfAppropriate();};		
	//OnWheelDown=[](Area* a, Event& e) -> void {a->AreaOfInterest->ScrollDownOneLineIfAppropriate();};	//TODO maybe shift mouse wheel for left right
	
	//linesperblock=AreaOfInterest->TOTALHEIGHT()/(_h-2); 
	//sizeofdragger=AreaOfInterest->HEIGHTONSCREEN()/linesperblock;
	draggerpos=0;
	ReCalcSize(true); //not sure 

	OnLeftDrag=[](Area* a, Event& e) -> void { 
		if(!e.DragOutOfBoundsP){
			int relativex=(a->SCREENXTOAREAX(e.CurrentScreenX)); 
				
			if( relativex>(SCROLLBARH(a)->draggerpos )){
				if( (SCROLLBARH(a)->draggerpos + SCROLLBARH(a)->sizeofdragger) < SCROLLBARH(a)->TOTALWIDTH() ){
					SCROLLBARH(a)->draggerpos++;
					SCROLLBARH(a)->ReCalcSize(true);
				}

			} else {
				if( (SCROLLBARH(a)->draggerpos > 0)){
					SCROLLBARH(a)->draggerpos--;
					SCROLLBARH(a)->ReCalcSize(true);
				}
			}

			a->FullReDraw();
			a->AreaOfInterest->FullReDraw();

		}
	};

	FullReDraw();

}

Area*
ScrollbarHorizontal::FullReDraw(void)  {
	

	for(int i=0;i<this->TOTALWIDTH();i++)DrawChar(178,engine->colour_window_border,engine->colour_bg,i,0);
	
	for(int i=0;i<sizeofdragger;i++){
		DrawChar(219,engine->colour_window_border,engine->colour_bg,i+draggerpos,0);
	}
	left_button->FullReDraw();
	right_button->FullReDraw();
	Area::FullReDraw();
	return this;
}
void ScrollbarHorizontal::ReCalcSize(bool draggerposisright){
	window_ratio=							(AreaOfInterest->TOTALWIDTH()*1000) / (AreaOfInterest->WIDTHONSCREEN());
	sizeofdragger=							WIDTHONSCREEN()*1000 / window_ratio;
	num_of_sb_positions=					WIDTHONSCREEN()-sizeofdragger;
	line_positions=							AreaOfInterest->TOTALWIDTH()-AreaOfInterest->WIDTHONSCREEN();
	
	
	if (num_of_sb_positions!=0){ //first attempt to stop divide by 0
		if(draggerposisright){
			AreaOfInterest->virtual_area.x =
				(  ((draggerpos*1000)/num_of_sb_positions)   *   line_positions)     /1000;
		} else {
			draggerpos=
				(  ((AreaOfInterest->virtual_area.x  *1000)/line_positions)  *  num_of_sb_positions)  /1000;
		}
	}

}

Area* CustomWidget::FullReDraw(void){
	NonEventLambda n = OnFullReDraw;
	if(n!=nullptr) n(this);
	return this;
}
