//
//  ArenaLevel.hpp
//  PixEngine
//
//  Created by rodo on 22/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "Splines.hpp"
#include "LineSegment.hpp"
#include "Config.hpp"
#include "Canvas2D.hpp"

namespace Pix {

	// objects located in map
	typedef struct sObjectPlacement {
		int oid;            // object OID
		float x, y;            // map position
	} ObjectPlacement_t;

	/**
	 * A World Map stores splines, edges and objects in a file,
	 * and provides methods to load and save them.
	 */

	typedef struct sBallWorldMap {

		const std::string NAME;

		std::vector<sSpline> vecSplines;
		std::vector<LineSegment_t> vecLines;
		std::vector<ObjectPlacement_t> vecObjects;

		float fTrackWidth = 20.0f;
		float fModelScale = 0.1f;

		bool loadV3(const std::string &filename, int scaleFactor = 0);

		bool saveV3(std::string filename, int scaleFactor = 0);

		bool loadV4(std::string filename, int scaleFactor = 0);

		bool saveV4(std::string filename, int scaleFactor = 0);

	private:

		bool bEmpty = true;

		std::string getPath(const std::string &filzwename);

	public:

		inline sBallWorldMap(std::string filename, int scaleFactor = 0) : NAME(filename) {
			bEmpty = !loadV3(filename + ".dat", scaleFactor);
		}

		inline bool isEmpty() { return bEmpty; }

		void drawSelf(Canvas2D *canvas);

	} BallWorldMap_t;

}

