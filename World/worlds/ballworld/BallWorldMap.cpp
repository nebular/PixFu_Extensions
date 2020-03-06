//
//  GameCircuit
//
//  Saves and loads a circuit definition (.cir)
//
//  Created by rodo on 11/01/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include <fstream>
#include <ios>
#include <iostream>
#include <filesystem> // C++17

#include "BallWorldMap.hpp"
#include "Fu.hpp"
#include "Config.hpp"
#include "World.hpp"

namespace Pix {

	std::string BallWorldMap_t::getPath(const std::string& filename) {
		return FuPlatform::getPath(std::string(PATH_LEVELS) + "/" + NAME + "/" + filename);
	}

	// current circuit loader / saver
	bool BallWorldMap_t::loadV3(const std::string& circuitName, int scaleFactor) {
		std::ifstream file(getPath(circuitName), std::ios::in | std::ios::binary);
		if (!file.is_open()) return false;

		int version;
		file.read((char *) &version, sizeof(int));

		// now we have version info and can detect a wrong file !
		if (version != 3) return false;

		for (int i = 0; i < LEVEL_MAXPLAYERS + 1; i++) {
			sSpline spline = sSpline();
			spline.Load(&file, scaleFactor);
			vecSplines.push_back(spline);
		}

		long l;
		file.read((char *) &l, sizeof(long));

		vecLines.clear();

		float f = static_cast<float>(1 / (1.0 + scaleFactor));
		for (int x = 0; x < l; x++) {
			LineSegment_t segment;
			file.read((char *) &segment, sizeof(LineSegment_t));
			vecLines.push_back({segment.sx * f, segment.sy * f, segment.ex * f, segment.ey * f, segment.radius});
		}

		file.read((char *) &fTrackWidth, sizeof(float));
		file.read((char *) &l, sizeof(long));

		for (long x = 0; x < l; x++) {

			ObjectPlacement_t object;
			file.read((char *) &object, sizeof(ObjectPlacement_t));

			if (scaleFactor != 0) {
				object.x *= f;
				object.y *= f;
			}

			vecObjects.push_back(object);
		}

		if (DBG)
			std::cerr << "Read circuit, len = " << vecSplines[0].fTotalSplineLength << ", edges " << vecLines.size() << ", objects "
					  << vecObjects.size() << std::endl;
		return true;
	}

	bool BallWorldMap_t::saveV3(std::string sFilename, int scaleFactor) {

		std::ofstream file(getPath(sFilename), std::ios::out | std::ios::binary);

		if (!file.is_open()) return false;

		// version
		int v = 3;
		file.write((char *) &v, sizeof(int));

		// TODO V4 write number of splines as well !
		// pahs

		for (sSpline s:vecSplines)
			s.Save(&file, scaleFactor);

		// edges
		unsigned long l = vecLines.size();
		file.write((char *) &l, sizeof(long));

		for (long x = 0; x < l; x++) {
			LineSegment_t segment = vecLines.at(x);

			if (scaleFactor != 0) {
				float f = (1 + scaleFactor);
				segment = {segment.sx * f, segment.sy * f, segment.ex * f, segment.ey * f, segment.radius};
			}
			file.write((char *) &segment, sizeof(LineSegment_t));
		}

		file.write((char *) &fTrackWidth, sizeof(float));

		l = vecObjects.size();
		file.write((char *) &l, sizeof(long));

		for (long x = 0; x < l; x++) {
			ObjectPlacement_t object = vecObjects.at(x);

			if (scaleFactor != 0) {
				float f = (1 + scaleFactor);
				object.x *= f;
				object.y *= f;
			}
			file.write((char *) &object, sizeof(ObjectPlacement_t));
		}

		if (DBG) std::cerr << "saved " << sFilename << " s " << scaleFactor;
		return true;
	}

	// future circuit loader / saver, slight modification to support arbitrary number of players (>8)
	bool BallWorldMap_t::loadV4(std::string sFilename, int scaleFactor) {
		long l;
		int version;

		std::ifstream file(getPath(sFilename), std::ios::in | std::ios::binary);
		if (!file.is_open()) return false;

		file.read((char *) &version, sizeof(int));

		// now we have version info and can detect a wrong file !
		if (version != 4) return false;

		// read number of saved splines (difference with V3)
		file.read((char *) &l, sizeof(long));

		for (int i = 0; i < l; i++) {
			sSpline spline;
			spline.Load(&file, scaleFactor);
			vecSplines.push_back(spline);
		}
		file.read((char *) &l, sizeof(long));

		vecLines.clear();

		float f = 1 / (1.0 + scaleFactor);
		for (int x = 0; x < l; x++) {
			LineSegment_t segment;
			file.read((char *) &segment, sizeof(LineSegment_t));
			vecLines.push_back({segment.sx * f, segment.sy * f, segment.ex * f, segment.ey * f, segment.radius});
		}

		file.read((char *) &fTrackWidth, sizeof(float));
		file.read((char *) &l, sizeof(long));

		for (long x = 0; x < l; x++) {

			ObjectPlacement_t object;
			file.read((char *) &object, sizeof(ObjectPlacement_t));

			if (scaleFactor != 0) {
				object.x *= f;
				object.y *= f;
			}

			vecObjects.push_back(object);
		}

		if (DBG)
			std::cerr << "Read circuit, len = " << vecSplines[0].fTotalSplineLength << ", edges " << vecLines.size() << ", objects "
					  << vecObjects.size() << std::endl;
		return true;
	}

	bool BallWorldMap_t::saveV4(std::string sFilename, int scaleFactor) {
		std::ofstream file(getPath(sFilename), std::ios::out | std::ios::binary);
		if (!file.is_open()) return false;

		// version
		int v = 4;
		long l;
		file.write((char *) &v, sizeof(int));

		// v4 write number of splines. Difference with SaveV3 !
		l = vecSplines.size();
		file.write((char *) &l, sizeof(long));

		for (sSpline s:vecSplines)
			s.Save(&file, scaleFactor);

		// edges
		l = vecLines.size();
		file.write((char *) &l, sizeof(long));

		for (long x = 0; x < l; x++) {
			LineSegment_t segment = vecLines.at(x);

			if (scaleFactor != 0) {
				float f = (1 + scaleFactor);
				segment = {segment.sx * f, segment.sy * f, segment.ex * f, segment.ey * f, segment.radius};
			}
			file.write((char *) &segment, sizeof(LineSegment_t));
		}

		file.write((char *) &fTrackWidth, sizeof(float));

		l = vecObjects.size();
		file.write((char *) &l, sizeof(long));

		for (long x = 0; x < l; x++) {
			ObjectPlacement_t object = vecObjects.at(x);

			if (scaleFactor != 0) {
				float f = (1 + scaleFactor);
				object.x *= f;
				object.y *= f;
			}
			file.write((char *) &object, sizeof(ObjectPlacement_t));
		}

		if (DBG) std::cerr << "saved " << sFilename << " s " << scaleFactor;
		return true;
	};

	void BallWorldMap_t::drawSelf(Canvas2D *canvas) {

		float scale = 1;

		if (vecSplines.size() > 0) vecSplines[0].DrawSelf(canvas, 0, 0, Pix::Colors::YELLOW, scale);
		if (vecSplines.size() > 1) vecSplines[1].DrawSelf(canvas, 0, 0, Pix::Colors::CYAN, scale);
		if (vecSplines.size() > 2) vecSplines[2].DrawSelf(canvas, 0, 0, Pix::Colors::PINK, scale);
		if (vecSplines.size() > 3) vecSplines[3].DrawSelf(canvas, 0, 0, Pix::Colors::MAGENTA, scale);
		if (vecSplines.size() > 4) vecSplines[4].DrawSelf(canvas, 0, 0, Pix::Colors::BLUE, scale);
		if (vecSplines.size() > 5) vecSplines[5].DrawSelf(canvas, 0, 0, Pix::Colors::GREEN, scale);

		//		vecSplines[0].DrawPoints(canvas, ox, oy, 0, Pix::Colors::GREEN, scale, fmod(fMetronome,1.0), true);

		// draw the edges
		for (LineSegment_t &segment:vecLines) {
			canvas->drawLine(segment.sx * scale, segment.sy * scale, segment.ex * scale, segment.ey * scale, Pix::Colors::RED);
		}

	}

};


