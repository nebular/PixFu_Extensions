//
//  BallCollisions.cpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "BallWorld.hpp"
#include "Ball.hpp"
#include "Player.hpp"
#include "Splines.h"

// 0.25 energy loss on crash
#define KCRASH_EFFICIENCY 0.75

namespace rgl {

	std::string BallWorld::TAG="BallWorld";

	// heigh over which
	constexpr int HEIGHT_EDGE_FLYOVER=1000;

	BallWorld::BallWorld(WorldConfig_t config, Perspective_t perspective)
	: World(config, perspective) {}

	void BallWorld::processStaticCollision(Ball *ball, Ball *target) {
		
		glm::vec3 displacement = ball->calculateOverlapDisplacement(target);
		
		// Displace Current Ball away from collision
		ball->position.x -= displacement.x;
		ball->position.y -= displacement.y;
		ball->position.z -= displacement.z;
		
		// Displace Target Ball away from collision
		target->position.x += displacement.x;
		target->position.y += displacement.y;
		target->position.z += displacement.z;
			
	}

	void BallWorld::processDynamicCollision(Ball *b1, Ball *b2, float fElapsedTime) {
		
		//	auto grados = [](float rads) { return std::to_string(rads*180/PI); };
		glm::vec3 pos1 = b1->position, pos2 = b2->position;
		
		// Distance between balls TODO
		float fDistance = sqrtf(  (pos1.x - pos2.x) * (pos1.x - pos2.x)
								+ (pos1.z - pos2.z) * (pos1.z - pos2.z) );
		
		// Normal
		float nx = (pos2.x - pos1.x) / fDistance;
		float nz = (pos2.z - pos1.z) / fDistance;

		// this "-" is because the map is rotated, I would like to avoid this but I
		// dont know where to put this fix
	//	glm::vec3 b1vel = -b1->speedVector();
	//	glm::vec3 b2vel = -b2->speedVector();
		
		glm::vec3 b1vel = -b1->speedVector();
		glm::vec3 b2vel = -b2->speedVector();

		
		float b1mass = b1->mass(), b2mass = b2->mass();
		
		// Wikipedia Version - Maths is smarter but same
		float kx = b1vel.x - b2vel.x;
		float kz = b1vel.z - b2vel.z;
		float p = 2.0 * (nx * kx + nz * kz) / (b1mass + b2mass) ;
		
		b1vel.x = b1vel.x - p * b2mass * nx;
		b1vel.z = b1vel.z - p * b2mass * nz;
		
		b2vel.x = b2vel.x + p * b1mass * nx;
		b2vel.z = b2vel.z + p * b1mass * nz;
		
		float b2speed = sqrt(b2vel.x * b2vel.x + b2vel.z * b2vel.z)* KCRASH_EFFICIENCY;
		float b1speed = sqrt(b1vel.x * b1vel.x + b1vel.z * b1vel.z)* KCRASH_EFFICIENCY;
		
		float b1Angle = atan2(b1vel.z, b1vel.x);
		float b2Angle = atan2(b2vel.z, b2vel.x);

		b1->onCollision(b2, fElapsedTime, b1speed, b1Angle);
		b2->onCollision(b1, fElapsedTime, b2speed, b2Angle);
		
	}

	long BallWorld::processCollisions(const std::vector<sLineSegment> &edges, float fElapsedTime) {
		
		long crono = nowns();
		
		vFakeBalls.clear();
		vCollidingPairs.clear();
		
		// Break up the frame elapsed time into smaller deltas for each simulation update
		float fSimElapsedTime = fElapsedTime / (float) Ball::SIMULATIONUPDATES;
		
		// Main simulation loop
		for (int i = 0; i < Ball::SIMULATIONUPDATES; i++) {
			
			// Set all balls time to maximum for this epoch
			
			iterateObjects([fSimElapsedTime](WorldObject *ball) {
				static_cast<Ball *>(ball)->fSimTimeRemaining = fSimElapsedTime;
			});
			
			// Erode simulation time on a per objec tbasis, depending upon what happens
			// to it during its journey through this epoch
			
			for (int j = 0; j < Ball::MAXSIMULATIONSTEPS; j++) {
				
				// Update Ball Positions
				iterateObjects([this](WorldObject *w) {
					Ball *ball = (Ball*)w;
					if (!ball->bDisabled) {
						if (ball->fSimTimeRemaining > 0.0f) {
							Ball *obstacle;
							Player *player = dynamic_cast<Player *>(ball);
							obstacle = player
							? player->process(this, NOTIME)
							: ball->process(this, NOTIME);
							
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
				int ostias = 0;
				
				// Work out static collisions with walls and displace balls so no overlaps
				iterateObjects([this, fSimElapsedTime,&edges](WorldObject *b) {
					
					Ball *ball = (Ball*)b;
					
					if (!ball->isStatic() && !ball->bDisabled)
						for (auto &edge : edges) {
							
							// float fDeltaTime = ball->fSimTimeRemaining;
							// Against Edges
							
							// Check that line formed by velocity vector, intersects with line segment
							float fLineX1 = edge.ex - edge.sx;
							float fLineY1 = edge.ey - edge.sy;
							
							float fLineX2 = ball->position.x - edge.sx;
							float fLineY2 = ball->position.z - edge.sy;
							
							float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;
							
							// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
							// how much of the segment is in the "shadow" of the object vector. The min and max clamp
							// this to lie between 0 and the line segment length, which is then normalised. We can
							// use this to calculate the closest point on the line segment
							
							float t = fmax(0, fmin(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;
							
							// Which we do here
							float fClosestPointX = edge.sx + t * fLineX1;
							float fClosestPointY = edge.sy + t * fLineY1;
							
							// And once we know the closest point, we can check if the ball has collided with the segment in the
							// same way we check if two balls have collided
							
							float fDistance = sqrtf((ball->position.x - fClosestPointX) * (ball->position.x - fClosestPointX) +
													(ball->position.z - fClosestPointY) * (ball->position.z - fClosestPointY));

							if (ball->height() < HEIGHT_EDGE_FLYOVER) {

								if (fDistance <= ball->radius() + edge.radius) {
									
									if (DBG)
										LogV(TAG, SF("collision, dist %f, bradius %f", fDistance, ball->radius()));
									
									// Collision has occurred - treat collision point as a ball that cannot move. To make this
									// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
									// so it behaves like a solid object when the momentum calculations are performed
									
									Ball *fakeball = ball->makeCollisionBall((edge.radius + 2.0f), ball->mass()*1000); // ball->mass() * .8f);
									fakeball->position = {fClosestPointX, ball->position.y, fClosestPointY};
									
									//						fakeball->fSpeed=ball->fSpeed;
									fakeball->fSpeed = 0;
									//						fakeball->fMass = ball->fMass; // 900000;
									//fakeball->fAngle = 0;
									// fakeball->fAngle = -ball->fAngle;

									// TODO: smartly calculating fHeight and Radius here will allow
									// us to jump over walls if so desired. Idea is to make edges
									// jumpable unless their height in the heightmap is 1
									// Add collision to vector of collisions for dynamic resolution
									
									vCollidingPairs.push_back({ball, fakeball});
									
									// Calculate displacement required
									float fOverlap = 1.00f * (fDistance - ball->radius() - fakeball->radius());
									
									// Displace Current Ball away from collision
									ball->position.x -= fOverlap * (ball->position.x - fakeball->position.x) / fDistance;
									ball->position.z -= fOverlap * (ball->position.z - fakeball->position.z) / fDistance;
									
									if (DBG)
										LogV(TAG, SF("overlap %f and after displacement %d", fOverlap, ball->overlaps(fakeball)));
									
								}
								}
						}
					
					// Against other balls
					iterateObjects([this, ball](WorldObject *targ) {
						Ball *target = (Ball *)targ;

						if (!ball->bDisabled
							&& !target->bDisabled							// disabled balls
							&& ball->id() != target->id()					// same ball
							&& (!ball->isStatic() || !target->isStatic())		// 2 static objs wont collide (opt)
							) {
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
					
					ball->commitSimulation();
					
				});
				
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
		return nowns() - crono;
	}
}
