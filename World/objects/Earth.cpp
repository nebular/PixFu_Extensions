//
//  Circuit Class
//
//  The circuit class manages players racing across a circuit.
//  This class takes care of rendering the circuit, the players
//  and running the race until it is over.
//
//  Created by rodo on 28/12/2019.
//  Copyright © 2019 rodo. All rights reserved.
//

#include "Earth.hpp"
#include "Ball.hpp"
#include "PixFu.hpp"
#include <vector>
#define  DBG_NOEDGESCOLLISIONS
// 0.25 energy loss on crash
#define KCRASH_EFFICIENCY 0.75

#include "World.hpp"

namespace rgl {

// muti-vector iteration


// get current microseconds epoch
	long pnow();

	CircuitAssets *CircuitAssets::__currentInstance = nullptr;

	CircuitAssets::CircuitAssets(std::string name) {

		__currentInstance = this;

		// ground texture
		sprGround = Drawable::fromFile(name + ".png");

		iMapWidth = sprGround->width;
		iMapHeight = sprGround->height;

		// height map
		sprHeight = Drawable::fromFile(name + ".heights.png");

		// populate ground texture with height (optimization for opengl shader)
		for (int y = 0; y < iMapHeight; y++)
			for (int x = 0; x < iMapWidth; x++) {
				Pixel ground = sprGround->getPixel(x, y);
				Pixel height = sprHeight->getPixel(x, y);
				ground.a = height.r;
				sprGround->setPixel(x, y, ground);
			}

		// horizon parallax bitmap
		sprHoriz = Drawable::fromFile(name + ".horiz.png");

		// sky texture
		sprSky = Drawable::fromFile(name + ".clouds.png");

		// drawable sprite for car trails, etc.
		sprCars = new Drawable(iMapWidth, iMapHeight);
		sprCars->clear(Colors::BLANK);

	}

	CircuitAssets::~CircuitAssets() {
		if (sprSky != nullptr) delete sprSky;
		if (sprGround != nullptr) delete sprGround;
		if (sprCars != nullptr) delete sprCars;
		if (sprHeight != nullptr) delete sprHeight;
		if (sprHoriz != nullptr) delete sprHoriz;
		sprSky = sprGround = sprCars = sprHoriz = sprHoriz = nullptr;
		if (__currentInstance == this) __currentInstance = nullptr;
	}

	Circuit::Circuit() {

	}

	Circuit::~Circuit() {
		vObjects.clear();
		vPlayers.clear();
	}

	void Circuit::init() {


	}


	void Circuit::processStaticCollision(Ball *ball, Ball *target) {

		glm::vec3 displacement = ball->calculateOverlapDisplacement(target);

		// Displace Current Ball away from collision
		ball->position.x -= displacement.x;
		ball->position.z -= displacement.y;

		// Displace Target Ball away from collision
		target->position.x += displacement.x;
		target->position.z += displacement.y;

		//	std::cerr << "Static Colision, dist " << fDistance << " olap " << fOverlap << " newdist " << ball->distance(target) << std::endl;

	}

	void Circuit::processDynamicCollision(Ball *b1, Ball *b2, float fElapsedTime) {

		//	auto grados = [](float rads) { return std::to_string(rads*180/PI); };

		// Distance between balls
		float fDistance = sqrtf((b1->xWorld() - b2->xWorld())
								* (b1->xWorld() - b2->xWorld()) + (b1->yWorld() - b2->yWorld()) * (b1->yWorld() - b2->yWorld()));

		// Normal
		float nx = (b2->xWorld() - b1->xWorld()) / fDistance;
		float ny = (b2->yWorld() - b1->yWorld()) / fDistance;

		glm::vec3 b1vel = b1->speedVector();
		glm::vec3 b2vel = b2->speedVector();

		float b1mass = b1->mass(), b2mass = b2->mass();

		// Wikipedia Version - Maths is smarter but same
		float kx = (b1vel.x - b2vel.x);
		float ky = (b1vel.y - b2vel.y);
		float p = 2.0 * (nx * kx + ny * ky) / (b1mass + b2mass) * KCRASH_EFFICIENCY;

		b1vel.x = b1vel.x - p * b2mass * nx;
		b1vel.y = b1vel.y - p * b2mass * ny;

		b2vel.x = b2vel.x + p * b1mass * nx;
		b2vel.y = b2vel.y + p * b1mass * ny;

		float b2speed = sqrt(b2vel.x * b2vel.x + b2vel.y * b2vel.y);
		float b1speed = sqrt(b1vel.x * b1vel.x + b1vel.y * b1vel.y);

		float b1Angle = atan2(b1vel.y, b1vel.x);
		float b2Angle = atan2(b2vel.y, b2vel.x);

		b1->onCollision(b2, fElapsedTime, b1speed, b1Angle);
		b2->onCollision(b1, fElapsedTime, b2speed, b2Angle);

	}

	long Circuit::processCollisions(World *engine, float fElapsedTime) {

		long crono = pnow();

		vFakeBalls.clear();
		vCollidingPairs.clear();

		// Break up the frame elapsed time into smaller deltas for each simulation update
		float fSimElapsedTime = fElapsedTime / (float) Ball::SIMULATIONUPDATES;

		// Main simulation loop
		for (int i = 0; i < Ball::SIMULATIONUPDATES; i++) {

			// Set all balls time to maximum for this epoch

			world->iterate([fSimElapsedTime](Ball *ball) {
				ball->fSimTimeRemaining = fSimElapsedTime;
			}, vObjects);

			// Erode simulation time on a per objec tbasis, depending upon what happens
			// to it during its journey through this epoch

			for (int j = 0; j < Ball::MAXSIMULATIONSTEPS; j++) {

				// Update Ball Positions
				foreach([this](Ball *ball) {
					if (!ball->bDisabled) {
						if (ball->fSimTimeRemaining > 0.0f) {
							Ball *obstacle;
							Player *player = dynamic_cast<Player *>(ball);
							obstacle = player
									   ? player->process(NOTIME, pAssets->sprHeight)
									   : ball->process(NOTIME, pAssets->sprHeight);

#ifndef DBG_NOHEIGHTMAPCOLLISIONS

							// these are collisions against height map

							if (obstacle != nullptr) {
								vFakeBalls.push_back(obstacle);
								// Add collision to vector of collisions for dynamic resolution
								vCollidingPairs.push_back({ball, obstacle});
								if (DBG) std::cerr << "- Ball collided with wall" << std::endl;
							}
#endif
						}
					}
				}, vPlayers, vObjects);


#ifdef DBG_NOCARCOLLISIONS
				// this would disable car collisions (with other cars)
				return pnow()-crono;
#endif
				// Work out static collisions with walls and displace balls so no overlaps
				foreach([this, fSimElapsedTime](Ball *ball) {

#ifndef DBG_NOEDGESCOLLISIONS

					if (!ball->bDisabled)
						for (auto &edge : this->pGameCircuit->vecLines) {

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
							if (ball->height() < HEIGHT_EDGE_FLYOVER && fDistance <= (ball->radius() + edge.radius)) {

								// Collision has occurred - treat collision point as a ball that cannot move. To make this
								// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
								// so it behaves like a solid object when the momentum calculations are performed

								Ball *fakeball = ball->makeCollisionBall((edge.radius + 2.0f), ball->mass() * .8f);
								fakeball->position = {fClosestPointX, ball->height(), fClosestPointY};

								//						fakeball->fSpeed=ball->fSpeed;

								fakeball->fSpeed = 0;
								//						fakeball->fMass = ball->fMass; // 900000;
								fakeball->fAngle = 0;

								// TODO: smartly calculating fHeight and Radius here will allow
								// us to jump over walls if so desired. Idea is to make edges
								// jumpable unless their height in the heightmap is 1

								fakeball->fHeight = ball->fHeight;

								// Add collision to vector of collisions for dynamic resolution
								vCollidingPairs.push_back({ball, fakeball});

								// Calculate displacement required
								float fOverlap = 1.0f * (fDistance - ball->radius() - fakeball->radius());

								// Displace Current Ball away from collision
								ball->position.x -= fOverlap * (ball->position.x - fakeball->position.x) / fDistance;
								ball->position.z -= fOverlap * (ball->position.z - fakeball->position.z) / fDistance;

								if (DBG)
									std::cerr << "overlap " << fOverlap << " and after displacement " << ball->overlaps(fakeball)
											  << std::endl;

							}
						}
#endif
					// Against other balls
					foreach([this, ball](Ball *target) {
						if (!ball->bDisabled && !target->bDisabled && ball->id() != target->id()) {
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
					}, vPlayers, vObjects);

					ball->commitSimulation();

				}, vPlayers, vObjects);

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
		return pnow() - crono;
	}

	ObjectPlacement_t *Circuit::getUniqueObject(int oid) {

		auto k = std::find_if(
				pGameCircuit->vecObjects.begin(),
				pGameCircuit->vecObjects.end(),
				[oid](ObjectPlacement_t &o) { return o.oid == oid; }
		);

		return k != pGameCircuit->vecObjects.end() ? &(*k) : nullptr;
	}

	float Circuit::getTerrainHeight(glm::vec2 pos) {
		return pAssets->sprHeight->sample(fmod(pos.x / pAssets->sprHeight->width, 1.0f),
										  fmod(pos.y / pAssets->sprHeight->height, 1.0f)).r / (float) 255;
	}
}
