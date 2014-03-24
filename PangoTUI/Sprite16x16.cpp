#include "stdafx.h"
#include "Sprite16x16.h"

using namespace PangoTUI;

Sprite16x16::Sprite16x16(void)
{
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			this->colours[i][j]=0;
			this->transparency[i][j]=true;
		}
	}
}

//load single sprite from a file
Sprite16x16::Sprite16x16(SDL_Surface* screen, const char* filename){
	std::ifstream ifile;
	ifile.open(filename,std::ios::in | std::ios::binary);

	char block[4];
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			ifile.read(block,4);
			colours[x][y] = SDL_MapRGB(screen->format,block[0],block[1],block[2]);
			transparency[x][y] = (block[3]==0)?false:true;

		}
	}
	ifile.close();
}

Sprite16x16::~Sprite16x16(void)
{
}

void Sprite16x16::SaveToFile(SDL_Surface* screen,const char* filename){
	std::ofstream ofile;
	ofile.open(filename,std::ios::out | std::ios::binary);

	//char block[4];
	Uint8 r,g,b;
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{	
			SDL_GetRGB(colours[x][y],screen->format,&r,&g,&b);
			ofile << r << g << b << transparency[x][y]?(int)1:(int)0 ;
			
		}
	}
	ofile.close();
}

