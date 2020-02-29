//
//  BallWorld.cpp
//  PixEngine
//
//  A world that manages Balls and their collisions. You will need to implement
//  virtual init() and process(), and call processCollisions(edges, fElapsedTime) to
//  perform collision calculations.
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "BallWorld.hpp"
#include "Ball.hpp"
#include "Player.hpp"
#include "Splines.h"
#include "gtx/fast_square_root.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace rgl {

	std::string BallWorld::TAG = "BallWorld";

	// heigh over which
	constexpr int HEIGHT_EDGE_FLYOVER = 1000;

	BallWorld::BallWorld(WorldConfig_t config, Perspective_t perspective)
			: World(config, perspective) {}

	void BallWorld::processStaticCollision(Ball *ball, Ball *target) {

		glm::vec3 displacement = ball->calculateOverlapDisplacement(target);

		// Displace Current Ball away from collision
		ball->mPosition.x -= displacement.x;
		ball->mPosition.y -= displacement.y;
		ball->mPosition.z -= displacement.z;

		// Displace Target Ball away from collision
		target->mPosition.x += displacement.x;
		target->mPosition.y += displacement.y;
		target->mPosition.z += displacement.z;

	}

	void BallWorld::processDynamicCollision(Ball *b1, Ball *b2, float fElapsedTime) {

		constexpr float fEfficiency = 0.6;

		//	auto grados = [](float rads) { return std::to_string(rads*180/PI); };
		glm::vec3 pos1 = b1->mPosition, pos2 = b2->mPosition;

		// Distance between balls TODO
		float fDistance = glm::fastSqrt((pos1.x - pos2.x) * (pos1.x - pos2.x)
								+ (pos1.z - pos2.z) * (pos1.z - pos2.z));

		// Normal
		float nx = (pos2.x - pos1.x) / fDistance;
		float nz = (pos2.z - pos1.z) / fDistance;

		// Tangent
		float tx = -nz;
		float tz = nx;

		// Dot Product Tangent
		float dpTan1 = b1->mSpeed.x * tx + b1->mSpeed.z * tz;
		float dpTan2 = b2->mSpeed.x * tx + b2->mSpeed.z * tz;

		// Dot Product Normal
		float dpNorm1 = b1->mSpeed.x * nx + b1->mSpeed.z * nz;
		float dpNorm2 = b2->mSpeed.x * nx + b2->mSpeed.z * nz;

		// Conservation of momentum in 1D
		float m1 = fEfficiency *
				   (dpNorm1 * (b1->mass() - b2->mass()) + 2.0f * b2->mass() * dpNorm2) /
				   (b1->mass() + b2->mass());

		float m2 = fEfficiency *
				   (dpNorm2 * (b2->mass() - b1->mass()) + 2.0f * b1->mass() * dpNorm1) /
				   (b1->mass() + b2->mass());

		// Update ball velocities
		glm::vec3 newSpeed1 = {tx * dpTan1 + nx * m1, b1->mSpeed.y, tz * dpTan1 + nz * m1};
		glm::vec3 newSpeed2 = {tx * dpTan2 + nx * m2, b2->mSpeed.y, tz * dpTan2 + nz * m2};

		b1->onCollision(b2, newSpeed1, fElapsedTime);
		b2->onCollision(b1, newSpeed2, fElapsedTime);

	}

	long BallWorld::processCollisions(const std::vector<sLineSegment> &edges, float fElapsedTime) {

		const long crono = nowns();

		vFakeBalls.clear();
		vCollidingPairs.clear();

		// Break up the frame elapsed time into smaller deltas for each simulation update
		const float fSimElapsedTime = fElapsedTime / (float) Ball::SIMULATIONUPDATES;

		// Main simulation loop
		for (int i = 0; i < Ball::SIMULATIONUPDATES; i++) {

//			long cronoIter=nowns();

			// Set all balls time to maximum for this epoch

			iterateObjects([fSimElapsedTime](WorldObject *ball) {
				static_cast<Ball *>(ball)->fSimTimeRemaining = fSimElapsedTime;
			});

			// Erode simulation time on a per objec tbasis, depending upon what happens
			// to it during its journey through this epoch

			for (int j = 0; j < Ball::MAXSIMULATIONSTEPS; j++) {

				// Update Ball Positions
				iterateObjects([this](WorldObject *w) {
					Ball *ball = (Ball *) w;

					if (!ball->ISSTATIC && !ball->bDisabled) {
						if (ball->fSimTimeRemaining > 0.0f) {

							// process ball physics
							ball->process(this);

							// process heightmap collisions & ball height
							Ball *obstacle = ball->processHeights(this);

							#ifndef DBG_NOHEIGHTMAPCOLLISIONS

							// these are collisions against height map

							if (obstacle != nullptr) {
								vFakeBalls.push_back(obstacle);
								// Add collision to vector of collisions for dynamic resolution
								vCollidingPairs.push_back({ball, obstacle});
								if (DBG) LogV(TAG, "- Ball collided with wall");
							}
							#endif
						}
					}
				});

#ifdef DBG_NOCARCOLLISIONS
				// this would disable car collisions (with other cars)
				return pnow()-crono;
#endif

				long times[4]={0};
				times[0]= nowns();

				// Work out static collisions with walls and displace balls so no overlaps

				iterateObjects([this, fSimElapsedTime, &edges, &times](WorldObject *b) {

					Ball *ball = (Ball *) b;
				
					long t2=nowns();
					
					if (!ball->ISSTATIC && !ball->bDisabled)
						for (auto &edge : edges) {

							// float fDeltaTime = ball->fSimTimeRemaining;
							// Against Edges

							// Check that line formed by velocity vector, intersects with line segment
							const float fLineX1 = edge.ex - edge.sx;
							const float fLineY1 = edge.ey - edge.sy;

							const float fLineX2 = ball->mPosition.x - edge.sx;
							const float fLineY2 = ball->mPosition.z - edge.sy;

							const float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;

							// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
							// how much of the segment is in the "shadow" of the object vector. The min and max clamp
							// this to lie between 0 and the line segment length, which is then normalised. We can
							// use this to calculate the closest point on the line segment

							const float t = (float)fmax(0, fmin(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;

							// Which we do here
							const float fClosestPointX = edge.sx + t * fLineX1;
							const float fClosestPointY = edge.sy + t * fLineY1;

							// And once we know the closest point, we can check if the ball has collided with the segment in the
							// same way we check if two balls have collided

							const float fDistance = glm::fastSqrt((ball->mPosition.x - fClosestPointX) * (ball->mPosition.x - fClosestPointX) +
														  (ball->mPosition.z - fClosestPointY) * (ball->mPosition.z - fClosestPointY));

							if (ball->mPosition.y < HEIGHT_EDGE_FLYOVER) {

								if (fDistance <= ball->radius() + edge.radius) {

									if (DBG)
										LogV(TAG, SF("collision, dist %f, bradius %f", fDistance, ball->radius()));

									// Collision has occurred - treat collision point as a ball that cannot move. To make this
									// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
									// so it behaves like a solid object when the momentum calculations are performed

									Ball *fakeball = ball->makeCollisionBall(
											edge.radius,
											{fClosestPointX, ball->mPosition.y, fClosestPointY});

									// TODO: smartly calculating fHeight and Radius here will allow
									// us to jump over walls if so desired. Idea is to make edges
									// jumpable unless their height in the heightmap is 1
									// Add collision to vector of collisions for dynamic resolution

									vCollidingPairs.push_back({ball, fakeball});

									// Calculate displacement required
									const float fOverlap = 1.00f * (fDistance - ball->radius() - fakeball->radius());

									// Displace Current Ball away from collision
									ball->mPosition.x -= fOverlap * (ball->mPosition.x - fakeball->mPosition.x) / fDistance;
									ball->mPosition.z -= fOverlap * (ball->mPosition.z - fakeball->mPosition.z) / fDistance;

									if (DBG)
										LogV(TAG, SF("overlap %f and after displacement %d", fOverlap, ball->overlaps(fakeball)));

								}
							}
						}

					times[1]=nowns()-t2;
					t2=nowns();

					// Against other balls
					iterateObjects([ball, this](WorldObject *targ) {

						Ball *target = (Ball *) targ;

						if ((!ball->ISSTATIC || !target->ISSTATIC)
							&& !ball->bDisabled && !target->bDisabled                            // disabled balls
							&& ball->ID != target->ID) {

							switch (ball->overlaps(target)) {
								case OVERLAPS:
									// Collision has occured
									vCollidingPairs.push_back({ball, target});
									processStaticCollision(ball, target);
									break;
								case OVERLAPS_OUTER:
									vFutureColliders.push_back({ball, target});
									break;
								default:
									break;
							}
						}
					});
					
					times[2]=nowns()-t2;

					ball->commitSimulation();
				});

				if (DBG) LogV(TAG, SF("iterate %ld, edges %ld, balls %ld, collid %d, future %d", nowns()-times[0], times[1], times[2], vCollidingPairs.size(), vFutureColliders.size()));

				// Now work out dynamic collisions
				for (auto c : vCollidingPairs)
					processDynamicCollision(c.first, c.second, fElapsedTime);

				for (auto c : vFutureColliders)
					c.first->onFutureCollision(c.second);

				vCollidingPairs.clear();
				vFutureColliders.clear();

				// Remove fake balls
				for (auto &b : vFakeBalls) delete b;
				vFakeBalls.clear();
			}
			
		}
		if (DBG) LogV(TAG, SF("total %ld", nowns()-crono));
		return nowns() - crono;
	}
}

#pragma clang diagnostic pop
