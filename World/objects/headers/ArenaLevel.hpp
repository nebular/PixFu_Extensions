//
//  ArenaLevel.hpp
//  PixEngine
//
//  Created by rodo on 22/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include "Splines.h"
#include "Config.hpp"

namespace rgl {

typedef struct sObjectPlacement {
	float x, y;
	float radiusDeprected;
	int oid;
} ObjectPlacement_t;

typedef struct sArenaLevel {

	const std::string NAME;

	sSpline sPaths[LEVEL_MAXPLAYERS+1];
	std::vector <sLineSegment> vecLines;
	std::vector <ObjectPlacement_t> vecObjects;
	
	float fTrackWidth = 20.0f;
	
	bool LoadV2(std::string sFilename, int scaleFactor = 0);
	bool LoadV3(std::string sFilename, int scaleFactor = 0);
	bool SaveV3(std::string sFilename, int scaleFactor = 0);
	bool LoadV4(std::string sFilename, int scaleFactor = 0);
	bool SaveV4(std::string sFilename, int scaleFactor = 0);

	std::string getPath(std::string filename);
	
	sArenaLevel(std::string filename, int scaleFactor = 0):NAME(filename) {

		if (!LoadV3(filename+".dat", scaleFactor))
			throw std::runtime_error("Error loading level "+filename);
	}
	
	

} ArenaLevel_t;

}

