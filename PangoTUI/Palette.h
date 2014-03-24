#pragma once
#include "PangoTUI.h"



namespace PangoTUI {

	enum Ecrayola { Almond, AntiqueBrass, Apricot, Aquamarine, Asparagus, AtomicTangerine, BananaMania, Beaver, Bittersweet, Black,
	BlizzardBlue, Blue, BlueBell, BlueGray, BlueGreen, BlueViolet, Blush, BrickRed, Brown, BurntOrange, BurntSienna, CadetBlue,
	Canary, CaribbeanGreen, CarnationPink, Cerise, Cerulean, Chestnut, Copper, Cornflower, CottonCandy, Dandelion, Denim, DesertSand,
	Eggplant, ElectricLime, Fern, ForestGreen, Fuchsia, FuzzyWuzzy, Gold, Goldenrod, GrannySmithApple, Gray, Green, GreenBlue,
	GreenYellow, HotMagenta, Inchworm, Indigo, JazzberryJam, JungleGreen, LaserLemon, Lavender, LemonYellow, MacaroniandCheese, 
	Magenta, MagicMint, Mahogany, Maize, Manatee, MangoTango, Maroon, Mauvelous, Melon, MidnightBlue, MountainMeadow, Mulberry,
	NavyBlue, NeonCarrot, OliveGreen, Orange, OrangeRed, OrangeYellow, Orchid, OuterSpace, OutrageousOrange, PacificBlue, Peach, 
	Periwinkle, PiggyPink, PineGreen, PinkFlamingo, PinkSherbert, Plum, PurpleHeart, PurpleMountainsMajesty, PurplePizzazz,
	RadicalRed, RawSienna, RawUmber, RazzleDazzleRose, Razzmatazz, Red, RedOrange, RedViolet, RobinsEggBlue, RoyalPurple, Salmon,
	Scarlet, ScreaminGreen, SeaGreen, Sepia, Shadow, Shamrock, ShockingPink, Silver, SkyBlue, SpringGreen, Sunglow, SunsetOrange,
	Tan, TealBlue, Thistle, TickleMePink, Timberwolf, TropicalRainForest, Tumbleweed, TurquoiseBlue, UnmellowYellow, VioletPurple, 
	VioletBlue, VioletRed, VividTangerine, VividViolet, White, WildBlueYonder, WildStrawberry, WildWatermelon, Wisteria, Yellow,
	YellowGreen, YellowOrange, DEFAULTCOLOUR };


class PANGOTUI_API Palette
{
public:

	static const unsigned char palCrayola[133][3];

	Palette(int numcol,unsigned char* rgbdata,SDL_Surface* screen);
	~Palette(void);

	

	int numberofcolours;

	struct rgbtriplet {
		Uint8 r,g,b;
	};

	rgbtriplet* rgbvals;
	Uint32*		u32vals;


};

}