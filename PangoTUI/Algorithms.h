#pragma once 

#include "PangoTUI.h"
#include <functional>

namespace PangoTUI {

	typedef std::function<Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumFn;

	struct algorithm {
		static int FloydSteinberg[];
		static int JarvisJudiceNinke[];
		static int Stucki[];
		static int Atkinson[] ;
		static int Burkes[];
		static int Sierra[];
		static int Sierra2row[];
		static int SierraLite[];

		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumAverage;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumLumaCorrectionGimp;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumLumaCorrectionBT709;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumLumaCorrectionBT601;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumDesaturate;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumDecompositMax;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumDecompositMin;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumSingleChannelR;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumSingleChannelG;
		static std::function< Uint8 (Uint8 r, Uint8 g, Uint8 b)> lumSingleChannelB;
	};

	inline int rndz(int low,int high){
		return (std::rand()%((high-low)+1))+low;
  }

	
}