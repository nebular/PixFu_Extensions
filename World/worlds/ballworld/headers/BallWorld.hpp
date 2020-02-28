//
//  BallCollisions.hpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "World.hpp"
#include "Splines.h"
#include <vector>

namespace rgl {

	class Ball;

	class BallWorld : public World {

		static std::string TAG;

		std::vector<Ball *> vFakeBalls;
		std::vector<std::pair<Ball *, Ball *>> vCollidingPairs;
		std::vector<std::pair<Ball *, Ball *>> vFutureColliders;

		// process static collisions
		void processStaticCollision(Ball *ball, Ball *target);

		// process dynamic collisions
		void processDynamicCollision(Ball *b1, Ball *b2, float fElapsedTime);

	public:
		BallWorld(WorldConfig_t config, Perspective_t perspective);

		/**
		 * Processes ball updates and collisions.
		 * This must be called from your overriden process() with the vector of world edges
		 */

		long processCollisions(const std::vector<sLineSegment> &edges, float fElapsedTime);

	};
}
