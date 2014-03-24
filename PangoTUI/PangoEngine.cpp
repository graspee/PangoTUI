#include "stdafx.h"
#include "PangoEngine.h"
#include "Rectangle.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include "Surface.h"
#include "win32specific.h"

//#include <iostream> 

using namespace PangoTUI; 



//PangoEngine(int _xres,int _yres,int _fontzoomx,int _fontzoomy,bool _fullscreen=false); //constructor using built-in font
	//PangoEngine(int _xres,int _yres,Font* font,int _fontzoomx,int _fontzoomy,bool _fullscreen=false); //constructor specifying your own font

void
PangoEngine::CommonSetup(void){
	BreakOutSwitch=false;
	
	if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    atexit(SDL_Quit);

	if( fullscreen ) {
		screen = SDL_SetVideoMode(xres, yres, 32, SDL_SWSURFACE|SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(xres, yres, 32,SDL_SWSURFACE );//SDL_DOUBLEBUF|SDL_HWSURFACE
		//screen = SDL_SetVideoMode(xres, yres, 32,SDL_DOUBLEBUF|SDL_HWSURFACE );
	}

    if ( screen == NULL ) {
        fprintf(stderr, "Unable to set video mode video: %s\n", SDL_GetError());
		exit(1);
    }

	SDL_EnableUNICODE( 1 );

	//strip_window();
	//Pause(5000);
	//exit(1);

	//create the desktop- an area that fills the screen.

	int widthchar=xres/(font->sizex*fontzoomx);
	int heightchar=yres/(font->sizey*fontzoomy);

	//KlikMap=new Area*[widthchar*heightchar];

	//no way to set alternative palette for now
	palette=new Palette(133,(unsigned char*)Palette::palCrayola,screen);
	colour_bg=palette->u32vals[Black];
	colour_icon=palette->u32vals[White];
	colour_icon_hi=palette->u32vals[11];
	colour_icon_hi_close=palette->u32vals[Red];
	colour_window_border=palette->u32vals[White];

	
	PangoTUI::Rectangle r(0,0,widthchar,heightchar,true);
	desktop=new Area(r,r);
	desktop->engine=this;
	Focus=desktop;
	desktop->OnClick=[](Area* a,Event &e)->void{ a->Focus(); };
	KlikMap=new Area*[widthchar*heightchar];
	for(int i=0;i< widthchar*heightchar;i++)
		KlikMap[i]=desktop;

	Surface::pe=this;
	MakeFontRamp();
	//LastSeenMausX=0;
	//LastSeenMausY=0;

	std::srand(time(0));
}

//constructor specifying your own font
PangoEngine::PangoEngine(int _xres, int _yres, Font* _font, int _fontzoomx, int _fontzoomy, bool _fullscreen):
	xres(_xres), yres(_yres),
	font(_font),
	fontzoomx(_fontzoomx), fontzoomy(_fontzoomy),
	fullscreen(_fullscreen)
{
	CommonSetup();

}

//constructor using the built-in font (Amstrad 437)
PangoEngine::PangoEngine(int _xres, int _yres, int _fontzoomx, int _fontzoomy, bool _fullscreen):
	xres(_xres), yres(_yres),
	fontzoomx(_fontzoomx), fontzoomy(_fontzoomy),
	fullscreen(_fullscreen)
{
	font=new Font((unsigned char*)Font::Amstrad437,8,8);
	CommonSetup();
	
}

PangoEngine::~PangoEngine(void) {
	delete desktop;
	delete font;
	delete palette;
	delete [] KlikMap;
}

//This is the lowest level drawing command for PangoConsole. It draws on the screen basically. It doesn't update a buffer

void PangoEngine::DrawCharEx(
	int x,int y,
	unsigned char c,
	Uint32 fg,Uint32 bg,
	bool sdlrectrefresh,
	bool existingfg, bool existingbg, bool existingchar){

		CharCell* cc=desktop->chargrid+((y*desktop->TOTALWIDTH())+x);

		Uint32 oobg=existingbg?cc->bg:bg;
		Uint32 oofg=existingfg?cc->fg:fg;
		Uint8 ooc=existingchar?cc->c:c;

		DrawChar(x,y,ooc,oofg,oobg,sdlrectrefresh);

}
void PangoEngine::DrawString_halfwidth(std::string s,int _x,int _y,Uint32 fg,Uint32 bg){
	LOCK_OR_RET(screen);
	
	
	for(auto c:s){
		if(_x<0 || _y<0 || _x>(desktop->TOTALWIDTH()*2)-1 || _y>desktop->TOTALHEIGHT()-1){
				std::cerr << "PangoConsole: Attempt to draw a halfwdith character out of bounds." << std::endl
					<< "x " << _x << "y " << _y << std::endl;

		}
		 int x=_x;int y=_y;
		
		Uint32 *bufp;
	
		x*=4*fontzoomx;y*=8*fontzoomy;
		bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
	//	*bufp = c;
		for(int line=0;line<font->sizey;line++){ //old ver was <8
			for(Uint8 repeety=0;repeety<fontzoomy;repeety++){
				for(char across=4-1;across>-1;across--){ //was across=7
						//new for zoomversion the repeet loop
						for (Uint8 repeetx=0;repeetx<fontzoomx;repeetx++){
							if(  *((unsigned char*)Font::Halfwidth+(c*font->dataperchar)+line*1+((1?(across<8):0)*0))&(1<<(across%8))  ){
								*bufp++=fg;
							} else {
								*bufp++=bg;
							}
						} //repeetx
				}//across
			bufp+=(screen->pitch/4)-(4*fontzoomx); //was -8 on the end
		} //repeety
		}//line
		
		
		SDL_UpdateRect(screen, x, y, 4*fontzoomx, font->sizey*fontzoomy);
		
		
		_x++;
	}//end of auto c in s string
	
	UNLOCK_IF_NEEDED(screen);
}

void PangoEngine::DrawChar(
	int x, int y,
	unsigned char c,
	Uint32 fg, Uint32 bg,
	bool sdlrectrefresh) {
	
	if(x<0 || y<0 || x>desktop->TOTALWIDTH()-1 || y>desktop->TOTALHEIGHT()-1){
			std::cerr << "PangoConsole: Attempt to draw a character out of bounds." << std::endl
				<< "x " << x << "y " << std::endl;

	}

	//update the character grid NO LONGER DONE LIKE THIS so updategrid not used //updatescreen is used though=refresh sdl rect y/n
	//if(updategrid){
	//	charcell* cc=chargrid+((y*widthchar)+x);
	//	cc->bg=bg;
	//	cc->fg=fg;
	//	cc->c=c;
	//}

	//lock screen
	LOCK_OR_RET(screen);
   
	Uint32 *bufp;

	//calc these only once
	
	Uint8 add1=(font->sizex<=8)?1:2; //ugh assumes square font
	Uint8 add2=(font->sizex<=8)?0:1;

	x*=font->sizex*fontzoomx;y*=font->sizey*fontzoomy; //old ver was *=8 twice
    bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
   // *bufp = c;

	for(int line=0;line<font->sizey;line++){ //old ver was <8
		for(Uint8 repeety=0;repeety<fontzoomy;repeety++){
			for(char across=font->sizex-1;across>-1;across--){ //was across=7
					//new for zoomversion the repeet loop
					for (Uint8 repeetx=0;repeetx<fontzoomx;repeetx++){
						//if(font[c][line]&(1<<across)){//amsfont
						//if(  *(font+(c*dataperchar)+line)&(1<<(across%8))  ){ //works for 8x8
						//if(  *(font+(c*dataperchar)+line*2+(1?(across<8):0))&(1<<(across%8))  ){//works for 16x16 brcks may be wrong
						if(  *(font->fontdata+(c*font->dataperchar)+line*add1+((1?(across<8):0)*add2))&(1<<(across%8))  ){//pretty sure this will fail for 16 wide font because of wrong way round ternary use
							
							//Uint32 fgweird=fluctuate(fg);//turn on for noise

							*bufp++=fg;//make fgweird for noise
						} else {
							*bufp++=bg;
						}
					} //repeetx
			}//across
		
		bufp+=(screen->pitch/4)-(font->sizex*fontzoomx); //was -8 on the end
	} //repeety
	}//line

	//unlock screen
	UNLOCK_IF_NEEDED(screen);

	if(sdlrectrefresh)SDL_UpdateRect(screen, x, y, font->sizex*fontzoomx, font->sizey*fontzoomy); //was screen,x,y,8,8


	
}

void PangoEngine::Pause(Uint32 milliseconds){

	Uint32 start=SDL_GetTicks();

	while (SDL_GetTicks()<start+milliseconds) {}
	return;


}

void PangoEngine::SDLUpdateRect(int x, int y, int x2, int y2){

	int tempx=font->sizex   *   fontzoomx,
		tempy=font->sizey   *   fontzoomy;
	//SDL_UpdateRect(screen, x*tempx, y*tempy, (x2-(x-1))*tempx,(y2-(y-1))*tempy);
	SDL_UpdateRect(screen, x*tempx, y*tempy, ((x2+1)-x)*tempx,((y2+1)-y)*tempy);
}
void PangoEngine::SDLUpdateRectPixels(int x, int y, int w, int h){

	SDL_UpdateRect(screen,x, y, w,h);
}

void PangoEngine::test01(char* filename){
	
	
	Surface* pix=new Surface(filename);
	//"c:\\Users\\graspee\\devel\\testingPangoTUI\\Debug\\giraffe.bmp"
	float edgedetect[]={	0, 1,0,
							1,-4,1,
							0, 1,0};
	float identity[]={0,0,0,0,1,0,0,0,0};
	float emboss[]={-2,-1,0,-1,1,1,0,1,2};
	float graspee2[]={-1,-1,-1,-1,4,-1,-1,-1,-1};
	float graspee[]={-1,-1,0,0,4,0,0,0,0};
	float sharpen[]={0,-2,0,-2,11,-2,0,-2,0};
	//pix->FitToScreen();
	//pix->SDL_GFX_FILTER_ResizeBilinear(xres,yres);
	//pix->SDL_GFX_FILTER_ConvMatrix(sharpen,1,1,0);
	//pix->SDL_GFX_FILTER_Dither_ErrorDiffusion(algorithm::JarvisJudiceNinke,128,algorithm::lumLumaCorrectionBT601);
	//pix->DisplayCentered();

	//pix->ASCIIArtOnMainWindow(); //original
	
	pix->DitherToMainWindow();

	return;

	



	



}


//new function to make ramp
//you would know the font already
//so you pass in a pointer to an array of characters and a length
//and 

//make an array with the 256 characters ordered by number of pixels in them
void PangoEngine::MakeFontRamp(void){
	std::vector <std::pair <int,float>> vals;
	float percentpiece=100.00f /(float)(font->sizex * font->sizey);
	Uint8 add1=(font->sizex==8)?1:2; //ugh assumes square font
	Uint8 add2=(font->sizex==8)?0:1;

	for(int c=0;c<256;c++){
		int howmany=0;

			for(int line=0;line<font->sizey;line++){ 
				for(char across=font->sizex-1;across>-1;across--){ 
					if(  *(font->fontdata+(c*font->dataperchar)+line*add1+((1?(across<8):0)*add2))&(1<<(across%8))  )
						howmany++;
				}//across
			}//line
			float percent = (float)howmany * percentpiece;
			vals.push_back(std::make_pair(c,percent));
			//std::cout << "font " << (int)c << " " << howmany << " ";
	}

	std::sort(vals.begin(),vals.end(),[] ( std::pair<int,float> a, std::pair<int,float> b ) -> bool { return a.second < b.second; });
	
	int i=0;
	for( auto &x : vals ){
	//	std::cout << "sort "  << " " << x.second << " ( " << x.first << " ) " << std::endl;
		fontramp[i]=x.first;
		fontramp2[i]=x.second;
		i++;
	}


}


Uint8 PangoEngine::WaitForMausClik(void){
	SDL_Event event;
	while(true){
		while( SDL_WaitEvent(&event) ){
			switch( event.type ){
				case SDL_MOUSEBUTTONUP:
				{
					return event.button.button;
				}
			}
		}
	}	
}

CharCell* PangoEngine::WaitForMausClik_RetCharCell(void){
	SDL_Event event;
	while(true){
		while( SDL_WaitEvent(&event) ){
			switch( event.type ){
				case SDL_MOUSEBUTTONUP:
				{
					int x=event.motion.x/(font->sizex*fontzoomx);
					int y=event.motion.y/(font->sizey*fontzoomy);
					CharCell* cc=desktop->chargrid+((y*desktop->TOTALWIDTH())+x);	
					return cc;
				}
			}
		}
	}	
}

Uint8 PangoEngine::WaitForAnyKey(void){
	SDL_Event event;
	while(true){
		while( SDL_WaitEvent(&event) ){
			switch( event.type ){
			case SDL_KEYDOWN:
				{
					goto funloop;
				}
			}
		}
	}	

	funloop:

	while(true){
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
			case SDL_KEYUP:
				{
					return event.key.keysym.sym;
				}
			}
		}
	}	

}
void PangoEngine::CLS_bypixel(void){
	SDL_FillRect(screen,NULL,palette->u32vals[Black]);
	SDL_UpdateRect(screen,0,0,0,0);
}

void PangoEngine::CLS_allcells(void){
	SDL_FillRect(screen,NULL,palette->u32vals[Black]);
	SDL_UpdateRect(screen,0,0,0,0);
	//same as by pixel but then also clear the cells:
	CharCell* cc=desktop->chargrid;
	for(int i=0;i < desktop->TOTALHEIGHT()*desktop->TOTALWIDTH(); i++){
		//CharCell* cc=desktop->chargrid;
		cc->bg=desktop->chargrid->fg=desktop->chargrid->c=0;
		cc++;
	}

		//chargrid=new CharCell[virtual_area.height * virtual_area.width];
}

void PangoEngine::DoEvents(void){
	SDL_Event		event;
	Event			myevent;

	static int		LastSeenMausX=			0;
	static int		LastSeenMausY=			0;
	static Area*	LastArea=				desktop;
	
	static Area*	TheOneFromMausDown[3]=	{NULL,NULL,NULL};										//the control you were in when you press maus button down
	static Area*	DragFocus[3]=			{NULL,NULL,NULL};										//the control you were in when drag started

	static bool		mausdown[3]=			{false,false,false};									//is maus button down or up
	static bool		dragging[3]=			{false,false,false};									//are we draggerizing with this button
	static int		dragfromx[3]=			{0,0,0};												//where drag started x
	static int		dragfromy[3]=			{0,0,0};												//and y

	static Uint16	lastunicode=0;

	while(true){
		while( SDL_WaitEvent(&event) ){

			int x=event.motion.x/(font->sizex*fontzoomx);
			int y=event.motion.y/(font->sizey*fontzoomy);
			Area* a=KlikMap[(y*desktop->TOTALWIDTH())+x];
			EventLambda el;
			
			myevent.CurrentScreenX			=		x;
			myevent.CurrentScreenY			=		y;
			myevent.LastScreenX				=		LastSeenMausX;
			myevent.LastScreenY				=		LastSeenMausY;
			myevent.mausbutton				=		event.button.button;

			if(BreakOutSwitch){BreakOutSwitch=false;return;}

			//keysym.mod is sdlmod .scancode is uint8 .sym is sdl key.unicode uint16 translated
			switch( event.type ){
			case SDL_QUIT:
				el=OnAppGoDown; if (el!=nullptr)el(desktop,myevent);
				break;

				case SDL_MOUSEBUTTONUP:{
					Uint8 ebb=event.button.button;
					if(ebb==SDL_BUTTON_WHEELUP){
						el=a->OnWheelUp; if(el!=nullptr)el(a,myevent);
					} else {
						if(ebb==SDL_BUTTON_WHEELDOWN){
							el=a->OnWheelDown; if(el!=nullptr)el(a,myevent);
						} else {
							if(ebb<4){
								mausdown[ebb-1]=false;
								el=a->OnMausUp; if(el!=nullptr)el(a,myevent);
								switch(ebb){
								case 1:
									el=a->OnLeftMausUp; if(el!=nullptr)el(a,myevent);
									break;
								case 3:
									el=a->OnRightMausUp; if(el!=nullptr)el(a,myevent);
									break;
								case 2:
									el=a->OnMiddleMausUp; if(el!=nullptr)el(a,myevent);
									break;
								}
								
								if(dragging[ebb-1]){
									dragging[ebb-1]=false;
									myevent.DragBeginScreenX=dragfromx[ebb-1];
									myevent.DragBeginScreenY=dragfromy[ebb-1];
									if(DragFocus[ebb-1]!=a)myevent.WasDragSuccessfulP=false; else myevent.WasDragSuccessfulP=true; //if dragfocus is not current area, drag fail?
									el=DragFocus[ebb-1]->OnDragEnd; if(el!=nullptr)el(DragFocus[ebb-1],myevent);
									switch(ebb){
										case 1:
											el=DragFocus[0]->OnLeftDragEnd; if(el!=nullptr)el(DragFocus[0],myevent);
											break;
										case 3:
											el=DragFocus[2]->OnRightDragEnd; if(el!=nullptr)el(DragFocus[2],myevent);
											break;
										case 2:
											el=DragFocus[1]->OnMiddleDragEnd; if(el!=nullptr)el(DragFocus[1],myevent);
											break;
									}
								} else {

									if(a==TheOneFromMausDown[ebb-1]){
										el=a->OnClick; if(el!=nullptr)el(a,myevent);
										switch(ebb){
											case 1:
												el=a->OnLeftClick; if(el!=nullptr)el(a,myevent);
												break;
											case 3:
												el=a->OnRightClick; if(el!=nullptr)el(a,myevent);
												break;
											case 2:
												el=a->OnMiddleClick; if(el!=nullptr)el(a,myevent);
												break;
										}
									}//end if a eq the one from maus down

								}
							}
						}
					}
				}
				break;
				case SDL_MOUSEBUTTONDOWN:{
					Uint8 ebb=event.button.button;
					if(ebb<4){
						mausdown[ebb-1]=true;
						el=a->OnMausDown; if(el!=nullptr)el(a,myevent);
						TheOneFromMausDown[ebb-1]=a;
						switch(ebb){
								case 1:
									el=a->OnLeftMausDown; if(el!=nullptr)el(a,myevent);
									break;
								case 3:
									el=a->OnRightMausDown; if(el!=nullptr)el(a,myevent);
									break;
								case 2:
									el=a->OnMiddleMausDown; if(el!=nullptr)el(a,myevent);
									break;
								}
					}
				}
				break;
				case SDL_KEYDOWN:{
					myevent.key=event.key.keysym.sym;
					myevent.key_unicode=event.key.keysym.unicode;
					lastunicode=myevent.key_unicode;
					el=Focus->OnKeyDown; if (el!=nullptr)el(Focus,myevent);
				}
				break;
				case SDL_KEYUP:{
					myevent.key=event.key.keysym.sym;
					myevent.key_unicode=lastunicode;
					el=Focus->OnKeyUp; if (el!=nullptr)el(Focus,myevent);
				} 
				break;
				case SDL_MOUSEMOTION:{
					if(x!=LastSeenMausX || y!=LastSeenMausY){										//if maus has changed position since last time
														//set last position to this position
						if(a!=LastArea){															//if we have changed areas ("transitioning!")
							el=LastArea->OnMausLeeb; if(el!=nullptr)el(LastArea,myevent);				//if the area under the last position has onleave event, call it
							el=a->OnMausEnter; if (el!=nullptr) el(a,myevent);							//if area under new position has onenter event, call it
						} else {
							//el=a->OnMausMoob;  if (el!=nullptr) el(a,myevent);							//maus moob is what you get when no area transition but charcell transition
						}
						el=a->OnMausMoob;  if (el!=nullptr) el(a,myevent);																			//at the moment we begin drag even if the first drag changed areas
						if(mausdown[0]){															//if this maus button is down
							//if(a!=DragFocus[0])myevent.DragOutOfBoundsP=true; else myevent.DragOutOfBoundsP=false;
							if(dragging[0]){//if we are currently dragging
								if(a!=DragFocus[0])myevent.DragOutOfBoundsP=true; else myevent.DragOutOfBoundsP=false;
								myevent.DragBeginScreenX=dragfromx[0];
								myevent.DragBeginScreenY=dragfromy[0];
								myevent.IsInitialDragEventP=false;
								//std::cout << " A " << " " ;////
								el=DragFocus[0]->OnDrag; if(el!=nullptr)el(DragFocus[0],myevent);		//if the drag focus area has a dragevent, call it
								el=DragFocus[0]->OnLeftDrag; if(el!=nullptr)el(DragFocus[0],myevent);	//if drag focus area has specific maus button drag event, call it
							} else {																//maus is down but not already dragging	 so BEGIN DRAG												
								myevent.IsInitialDragEventP=true;
								DragFocus[0]=LastArea;
								dragging [0]=true;
								dragfromx[0]=LastSeenMausX;
								dragfromy[0]=LastSeenMausY;
								myevent.DragBeginScreenX=dragfromx[0];
								myevent.DragBeginScreenY=dragfromy[0];
								//std::cout << myevent.CurrentScreenX << " " ;////
								el=LastArea->OnDragBegin; if(el!=nullptr)el(LastArea,myevent);		//generic on drag begin, maus button agnostic
								el=LastArea->OnLeftDragBegin; if(el!=nullptr)el(LastArea,myevent);	//specific maus button drag begin event
								el=DragFocus[0]->OnDrag; if(el!=nullptr)el(DragFocus[0],myevent);		//if the drag focus area has a dragevent, call it
								el=DragFocus[0]->OnLeftDrag; if(el!=nullptr)el(DragFocus[0],myevent);	//if drag focus area has specific maus button drag event, call it
							}
						}
						if(mausdown[2]){															//if this maus button is down
							
							if(dragging[2]){														//if we are currently dragging
								if(a!=DragFocus[2])myevent.DragOutOfBoundsP=true; else myevent.DragOutOfBoundsP=false;
								myevent.IsInitialDragEventP=false;
								myevent.DragBeginScreenX=dragfromx[2];
								myevent.DragBeginScreenY=dragfromy[2];
								el=DragFocus[2]->OnDrag; if(el!=nullptr)el(DragFocus[2],myevent);		//if the drag focus area has a dragevent, call it
								el=DragFocus[2]->OnRightDrag; if(el!=nullptr)el(DragFocus[2],myevent);	//if drag focus area has specific maus button drag event, call it
							} else {																//maus is down but not already dragging	 so BEGIN DRAG	
								myevent.IsInitialDragEventP=true;
								DragFocus[2]=LastArea;
								dragging [2]=true;
								dragfromx[2]=LastSeenMausX;
								dragfromy[2]=LastSeenMausY;
								myevent.DragBeginScreenX=dragfromx[2];
								myevent.DragBeginScreenY=dragfromy[2];
								el=LastArea->OnDragBegin; if(el!=nullptr)el(LastArea,myevent);		//generic on drag begin, maus button agnostic
								el=LastArea->OnRightDragBegin; if(el!=nullptr)el(LastArea,myevent);	//specific maus button drag begin event
								el=DragFocus[2]->OnDrag; if(el!=nullptr)el(DragFocus[2],myevent);		//if the drag focus area has a dragevent, call it
								el=DragFocus[2]->OnRightDrag; if(el!=nullptr)el(DragFocus[2],myevent);	//if drag focus area has specific maus button drag event, call it
							}
						}
						if(mausdown[1]){															//if this maus button is down
						
							if(dragging[1]){														//if we are currently dragging
								if(a!=DragFocus[1])myevent.DragOutOfBoundsP=true; else myevent.DragOutOfBoundsP=false;
								myevent.IsInitialDragEventP=false;
								myevent.DragBeginScreenX=dragfromx[1];
								myevent.DragBeginScreenY=dragfromy[1];
								el=DragFocus[1]->OnDrag; if(el!=nullptr)el(DragFocus[1],myevent);		//if the drag focus area has a dragevent, call it
								el=DragFocus[1]->OnMiddleDrag; if(el!=nullptr)el(DragFocus[1],myevent);	//if drag focus area has specific maus button drag event, call it
							} else {																//maus is down but not already dragging	 so BEGIN DRAG												
								myevent.IsInitialDragEventP=true;
								DragFocus[1]=LastArea;
								dragging [1]=true;
								dragfromx[1]=LastSeenMausX;
								dragfromy[1]=LastSeenMausY;
								myevent.DragBeginScreenX=dragfromx[1];
								myevent.DragBeginScreenY=dragfromy[1];
								el=LastArea->OnDragBegin; if(el!=nullptr)el(LastArea,myevent);		//generic on drag begin, maus button agnostic
								el=LastArea->OnMiddleDragBegin; if(el!=nullptr)el(LastArea,myevent);	//specific maus button drag begin event
								el=DragFocus[1]->OnDrag; if(el!=nullptr)el(DragFocus[1],myevent);		//if the drag focus area has a dragevent, call it
								el=DragFocus[1]->OnMiddleDrag; if(el!=nullptr)el(DragFocus[1],myevent);	//if drag focus area has specific maus button drag event, call it
							}
						}

						LastSeenMausX=x; LastSeenMausY=y;											//we set this later than before, here, so we have access to it for drag
						//if button is down
							//if we are not dragging
								//begin dragging, set variables
								//control under maus gets maus drag begin
							//if we are dragging
								//continue dragging
								//drag target area gets still dragging event

					}
				}
				break; 
			}//end switch event type

			LastArea=a;
		}
	}	

}

void PangoEngine::DrawPixelRect(int _x, int _y, int _w, int _h,Uint32 col){
	SDL_Rect r={_x,_y,_w,_h};
	SDL_FillRect(screen,&r,col);
	SDL_UpdateRect(screen,_x,_y,_w,_h);
}
int PangoEngine::SetKeyRepeat(int a,int b){
	return SDL_EnableKeyRepeat(a,b);
}
void PangoEngine::Pause(int a){
	SDL_Delay(a);
}

Uint32 PangoEngine::MapRGB(Uint8 r,Uint8 g,Uint8 b){
	return SDL_MapRGB(screen->format,r,g,b);
}

void PangoEngine::GetRGB(Uint32 xx,Uint8* r,Uint8* g,Uint8* b){
	SDL_GetRGB(xx,screen->format,r,g,b);
}

//adds random noise to painted pixels at the actual screen resolution
Uint32 PangoEngine::fluctuate(Uint32 x){
	Uint8 r,g,b;
	const int high=10;
	const int low=0;
	SDL_GetRGB(x,screen->format,&r,&g,&b);
	Uint8 fluc=(std::rand()%((high-low)+1))+low;
	int rr,gg,bb;
	if(std::rand()%100>49){
		rr=r-fluc;
		gg=g-fluc;
		bb=b-fluc;
	} else {
		rr=r+fluc;
		gg=g+fluc;
		bb=b+fluc;
	}
	if(rr>255)rr=255; else if(rr<0)rr=0;
	if(gg>255)gg=255; else if(gg<0)gg=0;
	if(bb>255)bb=255; else if(bb<0)bb=0;

	Uint32 retval=SDL_MapRGB(screen->format,rr,gg,bb);
	return retval;

}

void PangoEngine::DrawScaledPixel(int _x,int _y,Uint32 _colour,bool rectupdate){
	Uint32 *bufp;	
	_x*=fontzoomx;_y*=fontzoomy;

	bufp = (Uint32 *)screen->pixels + _y*screen->pitch/4 + _x;

	for(Uint8 repeety=0;repeety<fontzoomy;repeety++){
		for (Uint8 repeetx=0;repeetx<fontzoomx;repeetx++){
			*bufp++=_colour;
		}
		bufp+=(screen->pitch/4)-(fontzoomx);
	}

	

	

	if(rectupdate)SDL_UpdateRect(screen, _x, _y, fontzoomx, fontzoomy);
}

//draw a 16x16 sprite to the screen. co-ords are in "emulator screen space" i.e. 640x360
//does not do clipping. it will probably crash if you draw a sprite off screen
void PangoEngine::DrawSprite16x16(Sprite16x16* s,int x, int y,bool drawtransparentp, bool alpha50p){
	LOCK_OR_RET(screen);
	
	

		if(x<0 || y<0 || x>624 || y>344){
				std::cerr << "PangoConsole: Attempt to draw a 16sprite out of bounds." << std::endl
					<< "x " << x << "y " << y << std::endl;
		}
	
		
		Uint32 *bufp;
	
		x*=fontzoomx;y*=fontzoomy;
		
		bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
		for(int yy=0;yy<16;yy++){ //old ver was <8
			for(Uint8 repeety=0;repeety<fontzoomy;repeety++){
				for(Uint8  xx=0;xx<16;xx++){
						for (Uint8 repeetx=0;repeetx<fontzoomx;repeetx++){
							if( !s->transparency[xx][yy] ){
								*bufp++=s->colours[xx][yy];
							} else {
								if(!drawtransparentp)*bufp++ = colour_bg;
								else bufp++;
							}
							
						} //repeetx
						
				}//across
				
			bufp+=(screen->pitch/4)-(16*fontzoomx); //was -8 on the end
		} //repeety
		
		}//line
		
		
		SDL_UpdateRect(screen, x, y, 16*fontzoomx, 16*fontzoomy);
		
		
		//_x++;
	
	
	UNLOCK_IF_NEEDED(screen);
}

void PangoEngine::SetAppTitle(char* title){
	SDL_WM_SetCaption(title,title);
}
