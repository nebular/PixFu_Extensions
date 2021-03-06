//
//  WorldObject.cpp
//  PixEngine
//
//  Created by rodo on 08/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "WorldObject.hpp"
#include "World.hpp"
#include "Fu.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

namespace Pix {

	// Segment (A, B)
	// Point P
	// return : Point Q, the cloesest point on [AB] from P

	glm::vec3 ClosestPoint(glm::vec3 A, glm::vec3 B, glm::vec3 P) {

		glm::vec3 AB		= B - A;
		auto ab2			= glm::dot(AB,AB);
		glm::vec3 AP		= P - A;
		auto ap_dot_ab 		= glm::dot(AP,AB);
		float t				= ap_dot_ab / ab2;

		// the projection parameter on the line
		// clamp parameter to segment extremities
		if (t < 0.0F) t = 0.0F;
		else if (t > 1.0F) t = 1.0F;

		// calculate the closest point
		glm::vec3 Q = A + AB * t;
		return Q;

	}

	bool PointInSphere(glm::vec3 P, float r, glm::vec3 Q) {

		glm::vec3 PQ = Q - P;
		auto pq2 = glm::dot(PQ,PQ);
		float r2  = r * r;
		return !(pq2 > r2);

	}

	// https://gamedev.stackexchange.com/questions/21552/picking-objects-with-mouse-ray

	bool WorldObject::checkRayCollision(glm::vec3& origin, glm::vec3& direction) {
		glm::vec3 center = pos();
		glm::vec3 closest = ClosestPoint(origin, origin+2000.0F*direction, center);
		return PointInSphere(center, radius(), closest);
	}

	void WorldObject::process(World *world, float fElapsedTime) {

		// process intrinsic animation
		if (CONFIG.animation.enabled) {

			// apply rotation
			rot() += CONFIG.animation.deltaRotation * fElapsedTime;

			// apply scale pulse
			if (CONFIG.animation.scalePulse > 0)
				fRadiusAnimator = sinf(Fu::METRONOME) * CONFIG.animation.scalePulse;

		}

		if (world->CONFIG.debugMode == DEBUG_COLLISIONS)
			world->canvas()->drawCircle(static_cast<int32_t>(pos().x), static_cast<int32_t>(pos().z), static_cast<int32_t>(radius()),
										Pix::Colors::RED);

	}
}

#pragma clang diagnostic pop
