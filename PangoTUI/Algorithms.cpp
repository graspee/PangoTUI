#include "stdafx.h"
#include "Algorithms.h"
//#include "minwindef.h"


using namespace PangoTUI;



 int algorithm::FloydSteinberg[]= {16,   1,0,7,
		          -1,1,3, 0,1,5, 1,1,1, 255 };
	int algorithm::JarvisJudiceNinke[]={48, 1,0,7, 2,0,5,
		  -2,1,3, -1,1,5, 0,1,7, 1,1,6, 2,1,3,
		  -2,2,1, -1,2,3, 0,2,5, 1,2,3, 2,2,1,			255};	
	int algorithm::Stucki[]={42,            1,0,8, 2,0,4,
		  -2,1,2, -1,1,4, 0,1,8, 1,1,4, 2,1,2,
		  -2,2,1, -1,2,2, 0,2,4, 1,2,2, 2,2,1,			255};
	int algorithm::Atkinson[] = {8,         1,0,1, 2,0,1, 
				  -1,1,1, 0,1,1, 1,1,1, 
				          0,2,1,						255};
	int algorithm::Burkes[]={32,			 1,0,8, 2,0,4,
		  -2,1,2, -1,1,4, 0,1,8, 1,1,4, 2,1,2,			255};
	int algorithm::Sierra[]={32,			 1,0,5, 2,0,3,
		  -2,1,2, -1,1,4, 0,1,5, 1,1,4, 2,1,2,
			      -1,2,2, 0,2,3, 1,2,2,					255};
	int algorithm::Sierra2row[]={16,		 1,0,4, 2,0,3,
		  -2,1,1, -1,1,2, 0,1,3, 1,1,2, 2,1,1,			255};
	int algorithm::SierraLite[]={4,		 1,0,2,
				  -1,1,1, 0,1,1,						255};


	lumFn algorithm::lumAverage = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8
		{ return (r+g+b)/3; };
	lumFn algorithm::lumLumaCorrectionGimp  = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8
		{ return r*0.3 + g*0.59 + b*0.11; };
	lumFn algorithm::lumLumaCorrectionBT709 = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8
		{ return r*0.2126 + g*0.7152 + b*0.0722; };
	lumFn algorithm::lumLumaCorrectionBT601 = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8
		{ return r*0.299 + g*0.587 + b*0.114; };
	lumFn algorithm::lumDesaturate = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8 
		{ return ((max(max(r,g),b))+(min(min(r,g),b)))/2;  };
	lumFn algorithm::lumDecompositMax = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8 
		{ return max(r,(max(g,b))); };
	lumFn algorithm::lumDecompositMin = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8 
		{ return min(r,(min(g,b))); };
	lumFn algorithm::lumSingleChannelR = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8 
		{ return r; };
	lumFn algorithm::lumSingleChannelG = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8 
		{ return g; };
	lumFn algorithm::lumSingleChannelB = [] (Uint8 r, Uint8 g, Uint8 b) -> Uint8 
		{ return b; };

