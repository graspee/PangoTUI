#include "stdafx.h"
#include "Surface.h"

using namespace PangoTUI;


PangoEngine* Surface::pe;



Surface::Surface(){
}


Surface::Surface(char* filename)
{
	SDL_Surface* pre=IMG_Load(filename);
	if(pre==NULL){
		std::cerr << "PangoTUI::Surface::Surface: Error loading image." << std::endl;
		surface=NULL;
		return;
	}
	//SDL_Surface* pre=SDL_LoadBMP(filename); //this was old, pure sdl, only .bmp version
	surface = SDL_DisplayFormat(pre);
	SDL_FreeSurface(pre);
}
Surface::Surface(char* filename,bool alpha)
{
	SDL_Surface* pre=IMG_Load(filename);
	if(pre==NULL){
		std::cerr << "PangoTUI::Surface::Surface: Error loading image." << std::endl;
		surface=NULL;
		return;
	}
	//SDL_Surface* pre=SDL_LoadBMP(filename); //this was old, pure sdl, only .bmp version
	surface = SDL_DisplayFormatAlpha(pre);
	SDL_FreeSurface(pre);
}
Surface::Surface(SDL_Surface* x) : surface(x)
{
}

//destructor
Surface::~Surface(void)
{
	//is it really ok to do this?
	//the only time destructor gets called is when you have a Surface* and do delete on it, or when you have
	//a non-pointer Surface that goes out of scope, right?
	std::cerr << "Surface:~Surface: destructor called" << std::endl;
	SDL_FreeSurface(surface);
}

Surface* Surface::Copy(void){
	SDL_Surface* temp=SDL_ConvertSurface(surface,surface->format,surface->flags);
	Surface* retval=new Surface(temp);
	return retval;
}

void Surface::Display(int x, int y){
	SDL_BlitSurface(surface,NULL,pe->screen,NULL);
	SDL_UpdateRect(pe->screen,0,0,0,0);

}
void Surface::DisplayCentered(void){
	SDL_Rect s;
	s.x=(pe->xres-this->surface->w)/2;
	s.y=(pe->yres-this->surface->h)/2;
	s.w=this->surface->w;
	s.h=this->surface->h;
	
	SDL_BlitSurface(surface,NULL,pe->screen,&s);
	SDL_UpdateRect(pe->screen,0,0,0,0);

}
//Destructive dither of the image
Surface* 
	Surface::SDL_GFX_FILTER_Dither_ErrorDiffusion(
		int algorithm [],
		Uint8 threshold, 
		std::function<Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumFn) {
	
	LOCK_OR_RET_NULL(surface);
    
	Uint32* pixpoint=(Uint32*)surface->pixels;
	
	auto round = [] (float x) -> float { return (x>=0)?(int)(x+0.5):(int)(x-0.5); };
	auto clamp = [] (float x) -> Uint8 { if(x<0)return 0; if(x>255)return 255; return x; };


	
	Uint32 black=SDL_MapRGB(surface->format,0,0,0);
	Uint32 white=SDL_MapRGB(surface->format,255,255,255);

	float onedivx=(float)(1.0f/(float)(algorithm[0]));

	int perline=surface->pitch/4;

	for(int y=0;y<surface->h;y++){
		for(int x=0;x<surface->w;x++){
			Uint8 oldr,oldg,oldb;
			SDL_GetRGB(*pixpoint,surface->format,&oldr,&oldg,&oldb);
			//find closest colour, (black or white) and set it for x,y
			
			Uint8 oldpixel=lumFn(oldr, oldg, oldb);
			
			*pixpoint=(oldpixel>threshold)?white:black;
			//set quant error as three numbers, for rg and b
			float kevic=(oldpixel>threshold)?255.0f:0.0f;
			float quant_error_r=oldr-kevic;
			float quant_error_g=oldg-kevic;
			float quant_error_b=oldb-kevic;
			//spread the error out
			//loop through algorithm
			int i=1;
			while(algorithm[i]!=255){
					
				if(x+algorithm[i] >=0 && x+algorithm[i] < surface->w
				&& y+algorithm[i+1] >=0 && y+algorithm[i+1] < surface->h){
					Uint8 r,g,b;
					int loc=algorithm[i] + ( algorithm[i+1] * perline );
					SDL_GetRGB(*(pixpoint+loc),surface->format,&r,&g,&b);
					float f=(float)(((float)(algorithm[i+2]))*onedivx);
					int temp_r = clamp((r + round(f * quant_error_r)));
					int temp_g = clamp((g + round(f * quant_error_g)));
					int temp_b = clamp((b + round(f * quant_error_b)));
					*(pixpoint+loc)=SDL_MapRGB(surface->format,temp_r,temp_g,temp_b);
				}
				i+=3;
			}
			
			pixpoint++;
		}
	}
	
	UNLOCK_IF_NEEDED(surface);
	return this;
}

//returns itself, having destructively resized by created new, discarding old and switching pointers
Surface* Surface::SDL_GFX_FILTER_ResizeBilinear(int newW, int newH, bool returncopy){
  
	typedef struct tRGBA {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
	} RGBA;
		
	Uint32 mean1, mean2;
    Uint16 destinationGap;
    SDL_Surface *destination;
    int x_increment, y_increment;
    int x, y, accum_col, accum_row;
    Uint32 wx, wy, rwx, rwy;
    Uint32 *col_increment, *row_increment;
    RGBA *sourcePtr, *destinationPtr;
    RGBA *cO, *cH, *cV, *cD;
    
    if(newW <= 0) newW = 1;
    if(newH <= 0) newH = 1;

    x_increment = (((surface->w - 1) << 16) / newW);
    y_increment = (((surface->h - 1) << 16) / newH);

    col_increment = (Uint32 *) malloc(sizeof(*col_increment) * (newW + 1));
    row_increment = (Uint32 *) malloc(sizeof(*row_increment) * (newH + 1));

    for(x = 0, accum_col = 0; x <= newW; x++) {
        col_increment[x] = accum_col;
        accum_col &= 0xFFFF;
        accum_col += x_increment;
    }
    for(y = 0, accum_row = 0; y <= newH; y++) {
        row_increment[y] = accum_row;
        accum_row &= 0xFFFF;
        accum_row += y_increment;
    }
	
    if((destination = SDL_CreateRGBSurface(surface->flags, newW, newH, 
                                           surface->format->BitsPerPixel,
                                           surface->format->Rmask,
                                           surface->format->Gmask,
                                           surface->format->Bmask,
                                          surface->format->Amask)) == NULL) {
        return NULL;
    }

    LOCK_OR_RET_NULL(surface);
	LOCK_OR_RET_NULL(destination);

    sourcePtr = (RGBA *) surface->pixels;
    destinationPtr = (RGBA *) destination->pixels;
    destinationGap = destination->pitch - (destination->w * 4);

    for(y = 0; y < newH; y++) {
        cO = cH = sourcePtr;
        cH++;
        cV = cD = (RGBA *) ((Uint8 *) sourcePtr + surface->pitch);
        cD++;
        wy = row_increment[y] % 65536;
        rwy = 65536 - wy;
        for(x = 0; x < newW; x++) {
            wx = col_increment[x] % 65536;
            rwx = 65536 - wx;

            mean1 = ((cO->r * rwx + cH->r * wx) >> 16);
            mean2 = ((cV->r * rwx + cD->r * wx) >> 16);
            destinationPtr->r = (mean1 * rwy + mean2 * wy) >> 16;

            mean1 = ((cO->g * rwx + cH->g * wx) >> 16);
            mean2 = ((cV->g * rwx + cD->g * wx) >> 16);
            destinationPtr->g = (mean1 * rwy + mean2 * wy) >> 16;

            mean1 = ((cO->b * rwx + cH->b * wx) >> 16);
            mean2 = ((cV->b * rwx + cD->b * wx) >> 16);
            destinationPtr->b = (mean1 * rwy + mean2 * wy) >> 16;

            mean1 = ((cO->a * rwx + cH->a * wx) >> 16);
            mean2 = ((cV->a * rwx + cD->a * wx) >> 16);
            destinationPtr->a = (mean1 * rwy + mean2 * wy) >> 16;

            destinationPtr++;

            wx = col_increment[x + 1] >> 16;
            cO += wx;
            cH += wx;
            cV += wx;
            cD += wx;
        }
        sourcePtr = (RGBA *) ((Uint8 *) sourcePtr + (row_increment[y + 1] >> 16) * surface->pitch);
        destinationPtr = (RGBA *) ((Uint8 *) destinationPtr + destinationGap);
    }

    UNLOCK_IF_NEEDED(surface);
	UNLOCK_IF_NEEDED(destination);

    free(col_increment);
    free(row_increment);

	if(returncopy){
		Surface* temp=new Surface(destination);
		return temp;
	} else {
		//std::cerr << "time for lunch" ;
		SDL_FreeSurface(surface);
		surface=destination;
		return this;
	}

}


Surface* Surface::IntelligentResize(int w, int h){
	if(w==0 && h==0)return this;

	int target_w,target_h;

	bool resizing_needed=false;
	bool flopper=true;

	//now resize it
	//if w and h are both 0, user wants it in the size it is on disk
	
		resizing_needed=true;
		if(w!=0 && h!=0){
			target_w=w;
			target_h=h;
		} else {
			float ratio=(float)surface->w/(float)surface->h;
			if(w==0){
				target_h=h;
				target_w= (int) ((float)h * ratio);
			} else {
				target_w=w;
				target_h= (int) ( (float)w / ratio );
			}
		}
		//actual resizing heer:
		//compare w and h so far/2 to target-w and target-h. if either is lower, go with what we have already
		//else divide by 2 and check again
		int tempw=surface->w;
		int temph=surface->h;
		

		while( tempw/2 > target_w && temph/2 > target_h ){
			tempw /= 2;
			temph /= 2;
			SDL_GFX_FILTER_ResizeBilinear(tempw,temph);		
		}

		//final resize to the actual size
		SDL_GFX_FILTER_ResizeBilinear(target_w,target_h);
	
	return this;
}

Surface* Surface::FitToScreen(bool magnify){
	//if magnify is true then we are enlarging the picture to fit the screen if it's too small

	//if picture is smaller than screen in both dimensions, and we aren't magnifying, then nothing to do
	if(!magnify && surface->w < pe->xres && surface->h<pe->yres)return this;

	float ratio=(float)surface->w/(float)surface->h;
	

	int tryh,tryw;

	tryh=pe->yres;
	tryw= (int) ((float)tryh * ratio);
	
	if(tryw<= pe->xres && tryh<=pe->yres)return IntelligentResize(tryw,tryh);

	tryw= pe->xres;
	tryh= (int) ( (float)tryw / ratio );
	
	if(tryw<= pe->xres && tryh<=pe->yres)return IntelligentResize(tryw,tryh);

	std::cerr << "Surface::FitToScreen: Unable to fit to screen" << std::endl;
	return NULL;
}
Surface* Surface::FitToArbRect(int _w, int _h, bool magnify){
	//if magnify is true then we are enlarging the picture to fit the screen if it's too small

	//if picture is smaller than screen in both dimensions, and we aren't magnifying, then nothing to do
	if(!magnify && surface->w < _w && surface->h<_h)return this;

	float ratio=(float)surface->w/(float)surface->h;
	

	int tryh,tryw;

	tryh=_h;
	tryw= (int) ((float)tryh * ratio);
	
	if(tryw<= _w && tryh<=_h)return IntelligentResize(tryw,tryh);

	tryw= _w;
	tryh= (int) ( (float)tryw / ratio );
	
	if(tryw<= _w && tryh<=_h)return IntelligentResize(tryw,tryh);

	std::cerr << "Surface::FitToScreen: Unable to fit to screen" << std::endl;
	return NULL;
}
Surface* Surface::SDL_GFX_FILTER_ConvMatrix(float kernel[],float kernel_factor, float divisor,float offset){
	//assume the kernel is 3x3 for now cos i want to finish this and play skyrim
	//plus we will use only one edge dealing-with method: crop (ignore pixels that go off edge)
	static int deltax[9]={-1,0,1,-1,0,1,-1,0,1};
	static int deltay[9]={-1,-1,-1,0,0,0,1,1,1};

	for(Uint8 f=0;f<9;f++)kernel[f]*=kernel_factor;


	//first things first: create a copy of the graphic
	SDL_Surface* copypic=SDL_ConvertSurface(surface,surface->format,surface->flags);

	//lock if needed
	LOCK_OR_RET_NULL(surface);
	LOCK_OR_RET_NULL(copypic);

	//now loop through the original pix...

	Uint32* pp=(Uint32*)surface->pixels;
	Uint32* qq=(Uint32*)copypic->pixels;

	auto clamp= [] (float x) -> Uint8 {if(x<0)return 0;if(x>255)return 255;return x;};

	for(int y=0;y<surface->h;y++){
		for(int x=0;x<surface->w;x++){
			float totalr=0,totalg=0,totalb=0;
			for(Uint8 p=0;p<9;p++){
				Uint8 r,g,b;
				int dx=x+deltax[p];
				int dy=y+deltay[p];	
				if(dx>=0 && dy>=0 && dx<surface->w && dy<surface->h){
					int offs=dx + ((surface->pitch/4)*dy);
					SDL_GetRGB(*(pp+offs),surface->format,&r,&g,&b);
					totalr+=( kernel[p] * (float)r );
					totalg+=( kernel[p] * (float)g );
					totalb+=( kernel[p] * (float)b );
				}
			}
			*qq++=SDL_MapRGB(surface->format,clamp(totalr/divisor+offset),clamp(totalg/divisor+offset),clamp(totalb/divisor+offset));

		}
	}
	//unlock if needed
	UNLOCK_IF_NEEDED(surface);
	UNLOCK_IF_NEEDED(copypic);

	SDL_FreeSurface(surface);
	surface=copypic;
	
	return this;
}

//draw the surface as characters of solid colour at the offset, screen-based co-ords
void Surface::DisplayAsChars(int _x,int _y){
	Uint32* p=(Uint32*)surface->pixels;
	for(int y=0;y<surface->h;y++){
		for(int x=0;x<surface->w;x++){
			pe->desktop->DrawChar(219,*p,*p,_x+x,_y+y,true,true,false);
			*p++;
		}
	}
	pe->SDLUpdateRect(_x,_y,_x+surface->w-1,_y+surface->h-1);//this shuold fucking work
//	SDL_UpdateRect(pe->screen,0,0,0,0);
//	pe->SDLUpdateRect(0,0,79,44);
}
void Surface::DisplayCenteredAsChars(void){
	std::cout << "display cent as chars: w h = " << surface->w << " " << surface->h ;
	DisplayAsChars((pe->desktop->TOTALWIDTH()-surface->w)/2,(pe->desktop->TOTALHEIGHT()-surface->h)/2);
}

void Surface::DitherToMainWindow(void){
	
	
	//resize main image
	//WORKING SDL_GFX_FILTER_ResizeBilinear(pe->xres,pe->yres,false);
	FitToScreen(true);
	

	float edgedetect[]={	0, 1,0,
							1,-4,1,
							0, 1,0};
	float identity[]={0,0,0,0,1,0,0,0,0};
	float emboss[]={-2,-1,0,-1,1,1,0,1,2};
	float graspee2[]={-1,-1,-1,-1,4,-1,-1,-1,-1};
	float graspee[]={-1,-1,0,0,4,0,0,0,0};
	float sharpen[]={0,-2,0,-2,11,-2,0,-2,0};


	//edge detect/sharpen/something it
	//SDL_GFX_FILTER_ConvMatrix(graspee,1,1,0);


	//dither it
	SDL_GFX_FILTER_Dither_ErrorDiffusion(algorithm::JarvisJudiceNinke,127,algorithm::lumLumaCorrectionBT601);
	pe->CLS_bypixel();
	DisplayCentered();
	
}

void Surface::ASCIIArtOnMainWindow(void){
	//create a copy, scaled to character size and display it
	//WORKING Surface* col=SDL_GFX_FILTER_ResizeBilinear(pe->desktop->TOTALWIDTH(),pe->desktop->TOTALHEIGHT(),true);
	Surface* col=Copy(); //this copy is strictly speaking, unnecessary, but we'd have to go through all the layers of resizing routines
	//getting each one to pass on the fact that you wanted to return a copy. kind of tedious and could lead to bugs. later.
	col->FitToArbRect(pe->desktop->TOTALWIDTH(),pe->desktop->TOTALHEIGHT());
	//blit it supersized
	col->DisplayCenteredAsChars();
	
	int scaled_w=col->surface->w;
	int scaled_h=col->surface->h;

	int offsetx=(pe->desktop->TOTALWIDTH()-col->surface->w)/2;
	int offsety=(pe->desktop->TOTALHEIGHT()-col->surface->h)/2;

	delete col;

	pe->WaitForMausClik();
	
	//resize main image
	//WORKING SDL_GFX_FILTER_ResizeBilinear(pe->xres,pe->yres,false);
	FitToScreen(true);
	

	float edgedetect[]={	0, 1,0,
							1,-4,1,
							0, 1,0};
	float identity[]={0,0,0,0,1,0,0,0,0};
	float emboss[]={-2,-1,0,-1,1,1,0,1,2};
	float graspee2[]={-1,-1,-1,-1,4,-1,-1,-1,-1};
	float graspee[]={-1,-1,0,0,4,0,0,0,0};
	float sharpen[]={0,-2,0,-2,11,-2,0,-2,0};


	//edge detect/sharpen/something it
	//SDL_GFX_FILTER_ConvMatrix(graspee,1,1,0);


	//dither it
	SDL_GFX_FILTER_Dither_ErrorDiffusion(algorithm::JarvisJudiceNinke,127,algorithm::lumLumaCorrectionBT601);
	pe->CLS_bypixel();
	DisplayCentered();
	pe->WaitForMausClik();

	pe->CLS_bypixel();

	//now the ascii-erization shit
	LOCK_OR_RET(surface);

	//count the pixels in each square
	// Uint32 tempcol1=SDL_MapRGB(surface->format,255,255,255);
	 //Uint32 tempcol2=SDL_MapRGB(surface->format,0,0,0);
	 //Uint32 tempcol3=SDL_MapRGB(surface->format,255,0,0);
	//
	Uint8 red,green,blue;
	int pixacount;

	//pixels per char, w and h = surface->w (number of pixels wide the picture is) divided by pe->desktop-totalwidth (total w of 
	//desktop in characters)
	
	//WORKED float pixelsperchar_w = (float)surface->w / (float)pe->desktop->TOTALWIDTH();
	//WORKED float pixelsperchar_h = (float)surface->h / (float)pe->desktop->TOTALHEIGHT();
	float pixelsperchar_w = (float)surface->w / (float)scaled_w;
	float pixelsperchar_h = (float)surface->h / (float)scaled_h;

	float percentpiece=100.0f / (pixelsperchar_h * pixelsperchar_w);
	

	////loop through each piece of the image that will be handled by one character
	//WORKING for(int cy=0; cy < pe->desktop->TOTALHEIGHT(); cy++){//eg 0 to 44
		//WORKING for(int cx=0;cx < pe->desktop->TOTALWIDTH(); cx++){//eg 0 to 79
	for(int cy=0; cy < scaled_h; cy++){//eg 0 to 44
		for(int cx=0;cx < scaled_w; cx++){//eg 0 to 79
	
			pixacount=0;
			//Uint32* pixpointer = (Uint32 *)surface->pixels + (cy*pe->font->sizey)*(surface->pitch/4) + (cx*pe->font->sizex);
			
			Uint32* pixpointer = (Uint32 *)surface->pixels + (int)(
				(float)cy*(pixelsperchar_h)*((float)surface->pitch/4) + ((float)cx*pixelsperchar_w));

		// for(Uint8 sy=0;sy<pe->font->sizey;sy++){
		//		 for(Uint8 sx=0;sx<pe->font->sizex;sx++){
		for(Uint8 sy=0;sy<(int)pixelsperchar_h;sy++){
			 for(Uint8 sx=0;sx<(int)pixelsperchar_w;sx++){
				SDL_GetRGB(*pixpointer++,surface->format,&red,&green,&blue);
					if(red>0 || green>0 || blue>0)pixacount++;
				}
				//pixpointer+=(surface->pitch/4)-(pe->font->sizex*pe->fontzoomx); 
				pixpointer+=(surface->pitch/4)-((int)pixelsperchar_w); 
			}
			//tempcol3=SDL_MapRGB(screen->format,pixacount*4,pixacount*4,pixacount*4);

			 float percent = (float)pixacount * percentpiece;
			
			 int char2use= (int)( (255.00f / 100.00f) * percent ); 
			
	//		 // if(char2use<1 || char2use >255){std::cout << char2use << " " ;}
	//		 // char2use=' ';
	//		 //if(percent>5)char2use='.';
	//		 //if(percent>10)char2use=',';
	//		 //if(percent>20)char2use='!';
	//		 //if(percent>50)char2use='#';

			 CharCell* cc=pe->desktop->chargrid+((cy*pe->desktop->TOTALWIDTH())+cx);
				
			 bool match=false;

			 for(int slow=0;slow<256;slow++){
				 if(pe->fontramp2[slow]>=percent){
					// slow=255-slow;
					
					 pe->desktop->DrawCharEx(pe->fontramp[slow],pe->palette->u32vals[White],pe->palette->u32vals[Black],cx+offsetx,cy+offsety,
						true,true,false,
						true,false,false); //true f f
					match=true;
					break;
				 }
			 }
			if(!match)std::cerr << "percent " << percent << " ";
			// DrawChar(cx,cy,fontramp[char2use],cc->bg,tempcol2,true);

			 
			 

		// std::cerr << pixacount << std::endl;
		}
	}
	
	 SDL_UpdateRect(pe->screen,0,0,0,0);

	UNLOCK_IF_NEEDED(surface);

	pe->WaitForMausClik();

}


void Surface::PartBlit(int sx, int sy, int w, int h, int dx, int dy){
	surface->flags|=SDL_SRCALPHA;
	SDL_Rect srect,drect;
	srect.x=sx; srect.y=sy; srect.w=w; srect.h=h;
	drect.x=dx; drect.y=dy; drect.w=w; drect.h=h;
	SDL_BlitSurface(surface,&srect,pe->screen,&drect);
	
}
