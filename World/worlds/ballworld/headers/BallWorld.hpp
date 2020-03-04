//
//  BallCollisions.hpp
//  All this learnt from onelonecoder.com masterclass on Ball physics
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "World.hpp"
#include "BallWorldMap.hpp"
#include "LineSegment.hpp"
#include <vector>

namespace Pix {

	class Ball;

	class BallWorld : public World {

		static std::string TAG;
		static constexpr float EFFICIENCY = 1.00;

	protected:

		BallWorldMap_t *pMap = nullptr;

		std::vector<Ball *> vFakeBalls;
		std::vector<std::pair<Ball *, Ball *>> vCollidingPairs;
		std::vector<std::pair<Ball *, Ball *>> vFutureColliders;

		/**
		 * Add Balls to the world
		 */

		WorldObject *add(ObjectMeta_t features, ObjectLocation_t location, bool setHeight) override;

		/**
		 * Processes ball updates and collisions.
		 */

		long processCollisions(const std::vector<LineSegment_t> &edges, float fElapsedTime);

		// process static collisions
		void processStaticCollision(Ball *ball, Ball *target);

		// process dynamic collisions
		void processDynamicCollision(Ball *b1, Ball *b2, float fElapsedTime);

	public:

		BallWorld(std::string levelName, WorldConfig_t config);

		virtual void tick(Pix::Fu *engine, float fElapsedTime) override;

		void load(std::string levelName);
	};
}

#pragma clang diagnostic pop
