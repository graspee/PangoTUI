#include "stdafx.h"
#include "Bitmap.h"

using namespace PangoTUI;

Bitmap::Bitmap(void)
{
}

Bitmap::Bitmap(int _w,int _h) :
	w(_w), h(_h) {

	rgbdata=new unsigned char[w*h*3];

}

Bitmap::~Bitmap(void)
{
	delete [] rgbdata;
}

bool Bitmap::ExportArray(char* filename, Uint8 tripletsperline){
	if(tripletsperline<1)return false;

	
	int count=w*h;

	//declare a file for output and attempt to open it
	std::ofstream ofile;
	ofile.open(filename,std::ios::out);
	
	//write the start of the array declaration
	ofile << "const unsigned char mybitmap [" << count << "]" << " = { " << std::endl;
	
	int numperline=-1;

	
	unsigned char* source=rgbdata;
	Uint8 red,green,blue;
	//loop through the bitmap, outputting the pixels in the sdl surface as array numbers
	for(int upto=0;upto<count;upto++){
		red=*source++;
		green=*source++;
		blue=*source++;
	
		//write the file
		if( upto!=0 ) ofile << ", " ; 
		if( numperline++ == tripletsperline ) { 
			ofile << std::endl;
			numperline=0;
		}
		ofile << (int)red << "," << (int)green << "," << (int)blue ;

		
	}
	
	//write closing brace and close file
	ofile << " }" << std::endl;
	ofile.close();

	
	//success

	return true;
}