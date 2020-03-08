//
//  BallObject.hpp
//  PixFu
//
//  Created by rodo on 06/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "WorldMeta.hpp"
#include "Ball.hpp"
#include "Splines.hpp"

namespace Pix {

	class BallObject : public Ball {
		
		inline const static std::string TAG = "BallObject";
		
		Spline_t *pSpline = nullptr;
		sPoint2D mLastPoint;
		float fPosition = 0;

		void driveSpline(float fElapsedTime);

	public:
		BallObject (BallWorld *world, ObjectProperties_t& meta, ObjectLocation_t location);
		void process(World *world, float fTime);

	};

}
