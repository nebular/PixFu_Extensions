//
//  GameCircuit
//
//  Saves and loads a circuit definition (.cir)
//
//  Created by rodo on 11/01/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ArenaLevel.hpp"
#include "PixFu.hpp"
#include "Config.hpp"
#include "fstream"

namespace rgl {

std::string ArenaLevel_t::getPath(std::string filename) {
	return PixFuPlatform::getPath(std::string(PATH_LEVELS)+"/"+NAME+"/"+filename);
}

// current circuit loader / saver
bool ArenaLevel_t::LoadV3(std::string circuitName, int scaleFactor)
{
	std::ifstream file(getPath(circuitName), std::ios::in | std::ios::binary);
	if (!file.is_open()) return false;
	
	int version;
	file.read((char*)&version, sizeof(int));

	// now we have version info and can detect a wrong file !
	if (version != 3) return false;
	
	for (int i=0; i<LEVEL_MAXPLAYERS+1; i++)
		sPaths[i].Load(&file, scaleFactor);
	
	long l;
	file.read((char*)&l, sizeof(long));
	
	vecLines.clear();
	
	float f = 1/(1.0+scaleFactor);
	for (int x = 0; x < l; x++)
	{
		sLineSegment segment;
		file.read( (char*)&segment, sizeof(sLineSegment));
		vecLines.push_back({segment.sx*f, segment.sy*f, segment.ex*f, segment.ey*f, segment.radius});
	}
	
	file.read((char*)&fTrackWidth, sizeof(float));
	file.read((char*)&l, sizeof(long));
	
	for (long x = 0; x < l; x++)
	{
		
		ObjectPlacement_t object;
		file.read( (char*)&object, sizeof(ObjectPlacement_t));
		
		if (scaleFactor != 0) {
			object.x*=f;
			object.y*=f;
		}
		
		vecObjects.push_back(object);
	}
	
	if (DBG) std::cerr << "Read circuit, len = "<<sPaths[0].fTotalSplineLength<<", edges "<<vecLines.size()<<", objects "<<vecObjects.size() << std::endl;
	return true;
}

bool ArenaLevel_t::SaveV3(std::string sFilename, int scaleFactor)
{
	std::ofstream file(getPath(sFilename), std::ios::out | std::ios::binary);
	if (!file.is_open()) return false;
	
	// version
	int v=3;
	file.write((char*)&v, sizeof(int));
	
	// TODO V4 write number of splines as well !
	// pahs
	
	for (sSpline s:sPaths)
		s.Save(&file, scaleFactor);
	
	// edges
	long l=vecLines.size();
	file.write((char*)&l, sizeof(long));
	
	for (long x = 0; x < l; x++)
	{
		sLineSegment segment = vecLines.at(x);
		
		if (scaleFactor != 0) {
			float f = (1+scaleFactor);
			segment={segment.sx*f, segment.sy*f, segment.ex*f, segment.ey*f, segment.radius};
		}
		file.write( (char*)&segment, sizeof(sLineSegment));
	}
	
	file.write((char*)&fTrackWidth, sizeof(float));
	
	l=vecObjects.size();
	file.write((char*)&l, sizeof(long));
	
	for (long x = 0; x < l; x++)
	{
		ObjectPlacement_t object = vecObjects.at(x);
		
		if (scaleFactor != 0) {
			float f = (1+scaleFactor);
			object.x*=f;
			object.y*=f;
		}
		file.write( (char*)&object, sizeof(ObjectPlacement_t));
	}
	
	if (DBG) std::cerr << "saved "<<sFilename<<" s " << scaleFactor;
	return true;
}

// future circuit loader / saver, slight modification to support arbitrary number of players (>8)
bool ArenaLevel_t::LoadV4(std::string sFilename, int scaleFactor)
{
	long l;
	int version;

	std::ifstream file(getPath(sFilename), std::ios::in | std::ios::binary);
	if (!file.is_open()) return false;

	file.read((char*)&version, sizeof(int));

	// now we have version info and can detect a wrong file !
	if (version != 4) return false;

	// read number of saved splines (difference with V3)
	file.read((char*)&l, sizeof(long));

	for (int i=0; i<fmin(l, LEVEL_MAXPLAYERS+1); i++)
		sPaths[i].Load(&file, scaleFactor);
	
	file.read((char*)&l, sizeof(long));
	
	vecLines.clear();
	
	float f = 1/(1.0+scaleFactor);
	for (int x = 0; x < l; x++)
	{
		sLineSegment segment;
		file.read( (char*)&segment, sizeof(sLineSegment));
		vecLines.push_back({segment.sx*f, segment.sy*f, segment.ex*f, segment.ey*f, segment.radius});
	}
	
	file.read((char*)&fTrackWidth, sizeof(float));
	file.read((char*)&l, sizeof(long));
	
	for (long x = 0; x < l; x++)
	{
		
		ObjectPlacement_t object;
		file.read( (char*)&object, sizeof(ObjectPlacement_t));
		
		if (scaleFactor != 0) {
			object.x*=f;
			object.y*=f;
		}
		
		vecObjects.push_back(object);
	}
	
	if (DBG) std::cerr << "Read circuit, len = "<<sPaths[0].fTotalSplineLength<<", edges "<<vecLines.size()<<", objects "<<vecObjects.size() << std::endl;
	return true;
}

bool ArenaLevel_t::SaveV4(std::string sFilename, int scaleFactor)
{
	std::ofstream file(getPath(sFilename), std::ios::out | std::ios::binary);
	if (!file.is_open()) return false;
	
	// version
	int v=4;
	long l;
	file.write((char*)&v, sizeof(int));
	
	// v4 write number of splines. Difference with SaveV3 !
	l = LEVEL_MAXPLAYERS + 1;
	file.write((char*)&l, sizeof(long));

	for (sSpline s:sPaths)
		s.Save(&file, scaleFactor);
	
	// edges
	l=vecLines.size();
	file.write((char*)&l, sizeof(long));
	
	for (long x = 0; x < l; x++)
	{
		sLineSegment segment = vecLines.at(x);
		
		if (scaleFactor != 0) {
			float f = (1+scaleFactor);
			segment={segment.sx*f, segment.sy*f, segment.ex*f, segment.ey*f, segment.radius};
		}
		file.write( (char*)&segment, sizeof(sLineSegment));
	}
	
	file.write((char*)&fTrackWidth, sizeof(float));
	
	l=vecObjects.size();
	file.write((char*)&l, sizeof(long));
	
	for (long x = 0; x < l; x++)
	{
		ObjectPlacement_t object = vecObjects.at(x);
		
		if (scaleFactor != 0) {
			float f = (1+scaleFactor);
			object.x*=f;
			object.y*=f;
		}
		file.write( (char*)&object, sizeof(ObjectPlacement_t));
	}
	
	if (DBG) std::cerr << "saved "<<sFilename<<" s " << scaleFactor;
	return true;
};

};
