#include "stdafx.h"
#include "Area.h"
#include "win32specific.h"
#include "Algorithms.h"
#include <string>
using namespace PangoTUI;

PangoEngine* Area::engine;

FrameChars Area::FrameSingleLine={218,191,192,217,179,196};
FrameChars Area::FrameButtonBorder={130,131,129,128,179,196};

Area::Area(const PangoTUI::Rectangle& vis, const PangoTUI::Rectangle& virt, bool messlog) :
	parent(NULL),
	visual_area(vis),
	virtual_area(virt),
	messagelog(messlog),
	cursorx(0), 
	cursory(messlog?virt.height-1:0),
	logoffset(0),
	pen(White),paper(Black),
	NeedToCalcScreenToAreaOffsets(false)
{
	doesitscroll=true;
	//chargrid=new CharCell[virtual_area.height * virtual_area.width];
	CommonSetup();
}

Area::Area(const PangoTUI::Rectangle& vis, bool messlog) :
	parent(NULL),
	visual_area(vis),
	virtual_area(vis),
	messagelog(messlog),
	cursorx(0),
	cursory(messlog?vis.height-1:0),
	logoffset(0),
	pen(White),paper(Black),
	NeedToCalcScreenToAreaOffsets(false)
{
	virtual_area.x=0;
	virtual_area.y=0;
	doesitscroll=false;
	//chargrid=new CharCell[visual_area.height * visual_area.width];
	CommonSetup();
}

void Area::CommonSetup(void){
	chargrid=new CharCell[virtual_area.height * virtual_area.width]; //this was set to vis height by vis width?! how come it actually worked for mess log when i tested 10 lines high 5 lines vis?
	OnMausEnter=nullptr;
	OnMausLeeb=nullptr;
	OnDragBegin=nullptr;
	OnDragEnd=nullptr;
	OnKeyPress=nullptr;
	visible=true;
	//OnKeyUp=nullptr;
	//OnKeyDown=nullptr;

}
Area::~Area(void)
{

	delete [] chargrid;
	for(auto &o : children)delete o;

}

Area* Area::AddSub(Area* a){
	a->parent=this;
	children.push_back(a);
	return a;
}

//somearea->AddWidget(new IconButton(x,y,c



void Area::DumpToConsole(void){
		concol(FOREGROUND_BLUE|FOREGROUND_INTENSITY);
		std::cout << std::endl;
		std::cout << "dump: logoffset= " << logoffset << std::endl << std::endl;
		for(int y=0;y<TOTALHEIGHT();y++){
			for(int x=0;x<TOTALWIDTH();x++){
				//int yy=(y+logoffset)%TOTALHEIGHT();
				CharCell* cc=chargrid+((y*TOTALWIDTH())+x);			
				std::cout << (char)cc->c ;
			}
			std::cout << std::endl;
		}
		
		concol(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
}
Area*
Area::FullReDraw(void){
	if(!visible)return this;//new! and brought to you by tasty breakfasts limited!

	//std::cout << "fullredraw base on " << title.c_str() << std::endl;

	//insert complechs drawing coed heer
	//this is what we have to work with:
	//this->visual_area.x/y  POSITION ON PARENT
	//this->visual_area.width/height VISUAL WIDTH AND HEIGHT ON SCREEN 
	//this->doesitscroll; true/false
	//this->virtual_area.x/y ORIGINX,ORIGINY = WHERE SCROLLING AREA IS SCROLLED TO
	//this->virtual_area.width/height TRUE HEIGHT AND WIDTH OF AREA
	//this->parent NULL (for desktop) or parent 
//y+logoffset)%TOTALHEIGHT()

	
	//DumpToConsole();

	int saved=logoffset;
	logoffset=0;

#ifdef wankoff
	
		//loop through visible area and draw it
		for(int y=ORIGINY(); y< ORIGINY()+HEIGHTONSCREEN(); y++){
			for(int x=ORIGINX(); x< ORIGINX()+WIDTHONSCREEN(); x++){
				CharCell* cc=chargrid+((y*TOTALWIDTH())+x);			//keep the original loop but add an offset USED TO BE cc=chargrid+((y*TOTALWIDTH())+x);	
				DrawChar(cc->c,cc->fg,cc->bg,x,y,false,true,true); //was y also
			}
		}
	
#else
	
		//for(int y=0;y<TOTALHEIGHT();y++){
		for(int ycounter=0;ycounter<HEIGHTONSCREEN();ycounter++){
			int y=(ORIGINY()+ycounter)%TOTALHEIGHT();
			for(int x=0;x<TOTALWIDTH();x++){
				int yy=(y+saved)%TOTALHEIGHT();
			//	std::cout << logoffset << " " ;
				CharCell* cc=chargrid+((yy*TOTALWIDTH())+x);				//getting charcell for y+logoffset mod totalheight
				//logoffset=saved; //!!!!!!
				DrawChar(cc->c,cc->fg,cc->bg,x,y,false,true,true);			//drawing char for ordinary y (which draws it for y+logoffset mod totalheight)
			}																			//(but logoffset is set to 0)
		}
	
#endif
	logoffset=saved;

	//i would draw the above chars as false,true, false, do an sdlupdate rect on them but i'd have to recurse backwards to 
	//find where their real origin is. later

	//recursively draw all children
	for(auto o: this->children) o->FullReDraw();
	return this;
}

int Area::PARENTX(void)					{ return visual_area.x; }
int Area::PARENTY(void)					{ return visual_area.y; }
int Area::ORIGINX(void)					{ return virtual_area.x; }
int Area::ORIGINY(void)					{ return virtual_area.y; }
int Area::TOTALWIDTH(void)				{ return virtual_area.width; }
int Area::TOTALHEIGHT(void)				{ return virtual_area.height; }
int Area::WIDTHONSCREEN(void)			{ return visual_area.width; }
int Area::HEIGHTONSCREEN(void)			{ return visual_area.height; }

int Area::SCREENXTOAREAX(int x)			{ return x+ScreenToAreaX; }
int Area::SCREENYTOAREAY(int y)			{ return y+ScreenToAreaY; }	

void Area::DrawCharEx(
	unsigned char c,
	Uint32 fg,Uint32 bg,
	int x,int y,
	bool updategrid,bool updatescreen, bool sdlrectupdate,
	bool usefg, bool usebg, bool usec) {

		CharCell* cc=chargrid+((y*TOTALWIDTH())+x);

		Uint32 oobg=usebg?cc->bg:bg;
		Uint32 oofg=usefg?cc->fg:fg;
		Uint8 ooc=usec?cc->c:c;

	DrawChar(ooc,oofg,oobg,x,y,updategrid,updatescreen,sdlrectupdate);

}

void Area::CopyChar(int sx,int sy,int dx,int dy,bool a,bool b, bool c){
	CharCell* cc=chargrid+((sy*TOTALWIDTH())+sx);
	DrawChar(cc->c,cc->fg,cc->bg,dx,dy,a,b,c);
}



//The lowest level drawing on an area
void Area::DrawChar(unsigned char c,Uint32 fg,Uint32 bg,int x,int _y,bool updategrid,bool updatescreen, bool sdlrectupdate){


	int y=(_y+logoffset)%TOTALHEIGHT();
	

	//check if out of bounds for the area
	if( x>TOTALWIDTH()-1 || x<0 || y<0 || y>TOTALHEIGHT()-1 ){
		concol(FOREGROUND_INTENSITY|FOREGROUND_RED);
		std::cerr << "Area::DrawChar " << title << " Attempt to draw a character out of bounds." << std::endl;
		std::cerr << "X " << x << "Y " << y 
			<< "width " << TOTALWIDTH() << "height " << TOTALHEIGHT() 
			<< std::endl;
		concol(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
		return;//used to be exit(1)
	}


	//set the chargrid regardless of whether character will be visible
	if( updategrid ){
		CharCell* cc=chargrid+((y* TOTALWIDTH() )+x);
		cc->bg=bg;
		cc->fg=fg;
		cc->c=c;
	}

	if( ! updatescreen ) return;

	

	Area* aq=this;
	int xq=x;
	int yq=y;	//so xq, yq and aq change as we go back. x and y are constant 

	while(true){
		//TEST AREA 01- if the current area we are looking at (starting at current area and going backwards) scrolls and this is off the edge, forget it
		if( aq->doesitscroll &&
			( xq < aq->ORIGINX()
			|| yq < aq->ORIGINY()
			|| xq >= aq->ORIGINX() + aq->WIDTHONSCREEN()
			|| yq >= aq->ORIGINY() + aq->HEIGHTONSCREEN() ))
			return; 
		//TEST AREA 02
			//i don't think there is a test 2

		if(aq->parent==NULL) break;
		
		
		xq+=aq->PARENTX()-aq->ORIGINX();
		yq+=aq->PARENTY()-aq->ORIGINY();
		aq=aq->parent;
	}
	//so now aq is the desktop or top area
	//xq and yq=should be x and y of what we are drawing wrt to 0,0 desktop model. i hope
	if( xq > aq->TOTALWIDTH()-1 || xq<0 || yq<0 || yq > aq->TOTALHEIGHT()-1 ) return;

	//you came through unscathed!
	//draw it to the screen now
	
	//Uint32 newcol=SDL_MapRGB(engine->screen->format,rndz(75,255),rndz(75,255),rndz(75,255));
	
	ScreenToAreaX=	x  - xq;
	ScreenToAreaY=	_y - yq;

	if(visible){
		engine->DrawChar(xq,yq,c,fg,bg,sdlrectupdate);//newcol was fg
		//put the square in the klikmap
		engine->KlikMap[(yq*engine->desktop->TOTALWIDTH())+xq]=this;
	} else {
		engine->DrawChar(xq,yq,' ',fg,bg,sdlrectupdate);
	}


	//DumpToConsole();
	//engine->Pause(75);


}

//This is as low level as DrawChar, it's just a simple loop of it
void Area::DrawString(std::string s,Uint32 fg,Uint32 bg,int x,int y,bool updategrid, bool updatescreen){
	for(auto c : s){
		DrawChar(c,fg,bg,x++,y,updategrid,updatescreen);
	}
}



/*
//This draws a string on an area. x and y default to cursor pos,colours default to last used. Wraps; Supports \n.
void Area::PrintString(std::string s,Ecrayola fg,Ecrayola bg,int x,int y){
	if(fg==DEFAULTCOLOUR)fg=pen; else pen=fg;
	if(bg==DEFAULTCOLOUR)bg=paper; else paper=bg;
	if(x==32500)x=cursorx; 
	if(y==32500)y=cursory;

							//std::cout << "Area::PrintString: y= " << y << " logoffset= " << logoffset << " totalheight= " << TOTALHEIGHT() << std::endl;
	if(y==TOTALHEIGHT()){
							//std::cout << "Area::Printstring: y==totalheight" << std::endl;
		y--; //works for both, overprinting nastily on bottom line or writing on what used to be the top line but scrolled round
		if(messagelog){
			logoffset++;
			logoffset=logoffset % TOTALHEIGHT(); //so logoffset varies from 0 to totalheight-1
			//FullReDraw();
		} else {
							//std::cerr << "Area::PrintString: attempt to print off the bottom of a non logtype area" << std::endl;
		}
							//std::cout << "Area::Printstring: y is now " << y << " and logoffset is now " << logoffset << std::endl;
	}
							//std::cout << "Area::PrintString: now printing: >";
	for(auto c : s){
							//std::cout << (char)c;
		if(c=='\n'){//treating \n as CRLF is this ok?
			y++;x=0;
		} else { 
			DrawChar(c,engine->palette->u32vals[fg],engine->palette->u32vals[bg],x++,y,true,false,false);
			if(x==TOTALWIDTH())x=0,y++;
		}
	}
	
	cursorx=x;cursory=y;
	FullReDraw();
	//DumpToConsole();
	
}*/
void Area::PrintString(std::string s,Ecrayola fg,Ecrayola bg,int x,int y){
	if(fg==DEFAULTCOLOUR)fg=pen; else pen=fg;
	if(bg==DEFAULTCOLOUR)bg=paper; else paper=bg;
	if(x==32500)x=cursorx; 
	if(y==32500)y=cursory;

	if(y==TOTALHEIGHT()){y--;if(messagelog){logoffset++;logoffset=logoffset % TOTALHEIGHT();}}
							
	for(auto c : s){				
		if(c=='\n'){
			y++;x=0;	if(y==TOTALHEIGHT()){y--;if(messagelog){logoffset++;logoffset=logoffset % TOTALHEIGHT();}}

		} else { 
			DrawChar(c,engine->palette->u32vals[fg],engine->palette->u32vals[bg],x++,y,true,false,false);
			if(x==TOTALWIDTH()){x=0,y++;	if(y==TOTALHEIGHT()){y--;if(messagelog){logoffset++;logoffset=logoffset % TOTALHEIGHT();}}}
		}
	}
	
	cursorx=x;cursory=y;
	for(int f=x;f<TOTALWIDTH();f++)DrawChar(' ',engine->palette->u32vals[fg],engine->palette->u32vals[bg],f,y,true,false,false);

	FullReDraw();
}

void Area::DrawCustomFrame(Uint32 fg,Uint32 bg, const PangoTUI::Rectangle& r,const FrameChars& fc, bool updategrid, bool updatescreen){
	DrawChar(fc.topleft,fg,bg,r.x,r.y,updategrid,updatescreen); //top left corner
	for(int f=r.x+1;f<r.x+r.width-1;f++){
		DrawChar(196,fg,bg,f,r.y,updategrid,updatescreen); // top or bottom edge 154
		DrawChar(196,fg,bg,f,r.y+r.height-1,updategrid,updatescreen);
	}
	DrawChar(fc.topright,fg,bg,r.x+r.width-1,r.y,updategrid,updatescreen); // top right corner
	for(int f=r.y+1;f<r.y+r.height-1;f++){
		DrawChar(fc.vert,fg,bg,r.x,f,updategrid,updatescreen); // left or right edge
		DrawChar(fc.vert,fg,bg,r.x+r.width-1,f,updategrid,updatescreen); // left or right edge
	}
	DrawChar(fc.botleft,fg,bg,r.x,r.y+r.height-1,updategrid,updatescreen); //bottom left corner
	DrawChar(fc.botright,fg,bg,r.x+r.width-1,r.y+r.height-1,updategrid,updatescreen); // bottom right corner 
}

void Area::DrawFrame(Uint32 fg,Uint32 bg,int x,int y,int w,int h,bool updategrid, bool updatescreen){
	
	//some sanity check that frame won't clip
	//if(x<0 || x>widthchar-w || y<0 || y>heightchar-h){
	//	showerror(winhandle,L"frame off screen");
	//}

	DrawChar(218,fg,bg,x,y,updategrid,updatescreen); //top left corner
	for(int f=x+1;f<x+w-1;f++){
		DrawChar(196,fg,bg,f,y,updategrid,updatescreen); // top or bottom edge 154
		DrawChar(196,fg,bg,f,y+h-1,updategrid,updatescreen);
	}
	DrawChar(191,fg,bg,x+w-1,y,updategrid,updatescreen); // top right corner
	for(int f=y+1;f<y+h-1;f++){
		DrawChar(179,fg,bg,x,f,updategrid,updatescreen); // left or right edge
		DrawChar(179,fg,bg,x+w-1,f,updategrid,updatescreen); // left or right edge
	}
	DrawChar(192,fg,bg,x,y+h-1,updategrid,updatescreen); //bottom left corner
	DrawChar(217,fg,bg,x+w-1,y+h-1,updategrid,updatescreen); // bottom right corner 
}

void Area::BresLine(Uint8 c,Uint32 fg,Uint32 bg,int x0,int y0,int x1,int y1,bool updategrid, bool updatescreen){
	 
   int dx = abs(x1-x0);
   int dy = abs(y1-y0); 
   int sx=(x0 < x1)?1:-1;
   int sy=(y0 < y1)?1:-1;
   int err= dx-dy;
 
   while(1){
		DrawChar(c,fg,bg,x0,y0,updategrid,updatescreen);
		if(x0 == x1 && y0 == y1)break;
		int e2 = 2*err;
		if (e2 > -dy){
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx){
			err += dx;
			y0 += sy;
		}
   }
}

void Area::DrawRect(Uint8 character,Uint32 fg,Uint32 bg,int x,int y,int x2,int y2,bool updategrid, bool updatescreen){
	int swap;
	if(x2<x){swap=x;x=x2;x2=swap;}
	if(y2<y){swap=y;y=y2;y2=swap;}

	//some sanity check that rect won't clip
	//if(x<0 || x2>widthchar-1 || y<0 || y2>heightchar-1){
	//	showerror(winhandle,L"rect off screen");
	//}

	for(int f=x;f<x2+1;f++){
		DrawChar(character,fg,bg,f,y,updategrid, updatescreen);
		DrawChar(character,fg,bg,f,y2,updategrid, updatescreen);
	}
	for(int g=y+1;g<y2;g++){
		DrawChar(character,fg,bg,x,g,updategrid, updatescreen);
		DrawChar(character,fg,bg,x2,g,updategrid, updatescreen);
	}
	
}

void Area::DrawFilledRect(Uint8 character,Uint32 fg,Uint32 bg,int x,int y,int x2,int y2,bool updategrid, bool updatescreen){
	
	int swap;
	if(x2<x){swap=x;x=x2;x2=swap;}
	if(y2<y){swap=y;y=y2;y2=swap;}

	//some sanity check that rect won't clip
	//if(x<0 || x2>widthchar-1 || y<0 || y2>heightchar-1){
	//	showerror(winhandle,L"rect off screen");
	//}
	for(int f=x;f<x2+1;f++){
		for(int g=y;g<y2+1;g++){
			DrawChar(character,fg,bg,f,g,updategrid,updatescreen,true);//was false
		}
	}
	
	//engine->SDLUpdateRect(x,y,x2,y2); this update rect doesn't work because it could be relative
}

void Area::DrawCircle(Uint8 c,Uint32 fg,Uint32 bg,int x0, int y0, int radius,bool updategrid, bool updatescreen)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
 
  DrawChar(c,fg,bg,x0, y0 + radius,updategrid, updatescreen);
  DrawChar(c,fg,bg,x0, y0 - radius,updategrid, updatescreen);
  DrawChar(c,fg,bg,x0 + radius, y0,updategrid, updatescreen);
  DrawChar(c,fg,bg,x0 - radius, y0,updategrid, updatescreen);
 
  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    DrawChar(c,fg,bg,x0 + x, y0 + y,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 - x, y0 + y,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 + x, y0 - y,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 - x, y0 - y,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 + y, y0 + x,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 - y, y0 + x,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 + y, y0 - x,updategrid, updatescreen);
    DrawChar(c,fg,bg,x0 - y, y0 - x,updategrid, updatescreen);
  }
}

void Area::DrawFilledCircle(Uint8 c,Uint32 fg,Uint32 bg,int x0, int y0, int radius,bool updategrid, bool updatescreen)
{
	//TODO it's ridiculous to use bresline for these straight lines really. sort it out
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;
 
  DrawChar(c,fg,bg,x0, y0 + radius,updategrid, updatescreen);
  DrawChar(c,fg,bg,x0, y0 - radius,updategrid, updatescreen);
  BresLine(c,fg,bg,x0 + radius, y0,x0 - radius, y0,updategrid, updatescreen);
 
  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    BresLine(c,fg,bg,x0 + x, y0 + y,x0 - x, y0 + y,updategrid, updatescreen);
    BresLine(c,fg,bg,x0 + x, y0 - y,x0 - x, y0 - y,updategrid, updatescreen);
    BresLine(c,fg,bg,x0 + y, y0 + x,x0 - y, y0 + x,updategrid, updatescreen);
    BresLine(c,fg,bg,x0 + y, y0 - x,x0 - y, y0 - x,updategrid, updatescreen);
  }
}

//draws a "sprite" from an array of rgb bg and rgb fg plus character
void Area::DrawSprite(int x,int y,int w,int h,unsigned char* array, bool updategrid, bool updatescreen){
	//if(x<0 || x>widthchar-w || y<0 || y>heightchar-h){
	//	showerror(winhandle,L"sprite off screen");
	//}


	unsigned char* marker=array;
	Uint8 r,g,b;
	for(int gg=0;gg<h;gg++){
		for(int ff=0;ff<w;ff++){
			r=*array++;g=*array++;b=*array++;
			Uint32 bg=SDL_MapRGB(engine->screen->format,r,g,b);
			r=*array++;g=*array++;b=*array++;
			Uint32 fg=SDL_MapRGB(engine->screen->format,r,g,b);
			DrawChar(*array++,fg,bg,x+ff,y+gg,updategrid, updatescreen, false);
		}
	}

	engine->SDLUpdateRect(x,y,x+w-1,y+h-1);

}

//draws a bitmap as coloured spaces from a bitmap object
void Area::DrawBitmap(int x,int y,Bitmap* source, bool updategrid, bool updatescreen){
	//if(x<0 || x>widthchar-w || y<0 || y>heightchar-h){
	//	showerror(winhandle,L"sprite off screen");
	//}
	
	unsigned char* marker=source->rgbdata;
	Uint8 r,g,b;
	for(int gg=0;gg<source->h;gg++){
		for(int ff=0;ff<source->w;ff++){
			r=*source->rgbdata++;g=*source->rgbdata++;b=*source->rgbdata++;
			Uint32 bg=SDL_MapRGB(engine->screen->format,r,g,b);
			DrawChar(' ',bg,bg,x+ff,y+gg, updategrid, updatescreen, false);
		}
	}

	engine->SDLUpdateRect(x,y,x+source->w-1,y+source->h-1);

}

void Area::SetOrigin(int _x, int _y){
	virtual_area.x= _x;
	virtual_area.y= _y;	
	FullReDraw();
	
}

void Area::ScrollUpOneLineIfAppropriate(void){
	if(virtual_area.y>0){
		virtual_area.y--;
		FullReDraw();
		//AssociatedVScroller->FullReDraw();
	}
}

void Area::ScrollDownOneLineIfAppropriate(void){
	if(virtual_area.y < (virtual_area.height-visual_area.height)  ){
		virtual_area.y++;
		FullReDraw();
		//AssociatedVScroller->FullReDraw();

	}
}
void Area::ScrollLeftOneLineIfAppropriate(void){
	if(virtual_area.x>0){
		virtual_area.x--;
		FullReDraw();
		//AssociatedVScroller->FullReDraw();
	}
}

void Area::ScrollRightOneLineIfAppropriate(void){
	if(virtual_area.x < (virtual_area.width-visual_area.width)  ){
		virtual_area.x++;
		FullReDraw();
		//AssociatedVScroller->FullReDraw();

	}
}
void Area::Clear(Uint32 fg, Uint32 bg){
	
	DrawFilledRect(' ',fg,bg,0,0,TOTALWIDTH()-1,TOTALHEIGHT()-1);
	

}

void Area::Focus(void){
	if(engine->Focus==this)return; //already focused
	//call lose focus event on what is focused at the moment
	NonEventLambda a=engine->Focus->OnLoseFocus;
	if(a!=nullptr)a(engine->Focus);
	//change focus to this and call gain focus event
	engine->Focus=this;
	a=OnGainFocus;
	if(a!=nullptr)a(this);
	FullReDraw();
}

void Area::DeFocus(void){
	engine->Focus=engine->desktop;
	NonEventLambda a=OnLoseFocus;
	if(a!=nullptr)a(this);
	FullReDraw();
}
