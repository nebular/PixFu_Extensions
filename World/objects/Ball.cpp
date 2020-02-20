//
//  Ball.cpp
//  javid
//
//  Created by rodo on 11/01/2020.
//  Copyright © 2020 rodo. All rights reserved.
//	Copyright ©

#include "Ball.hpp"
#include "Utils.hpp"
#include <cmath>

namespace rgl {

	constexpr int KSPEED = 1000;
	// global scale for ball radius
	float Ball::stfBaseScale = 1.0;
	float Ball::stfHeightScale = 1.0;

	constexpr std::string Ball::TAG;

	Ball::Ball(int uid, float radi, float mass, int worldWidth, int worldHeight) {
		TAG = "BALL" + std::to_string(uid);

		nId = uid;
		fRadius = radi;
		fMass = mass;
		bFlying = false;
		iWorldWidth = worldWidth;
		iWorldHeight = worldHeight;
		setRadiusMultiplier(1.0);
	}

	Ball *Ball::makeCollisionBall(float radi, float nmass) {
		Ball *obstacle = new Ball(FAKE_BALL_ID, radi, nmass, iWorldWidth, iWorldHeight);
		obstacle->fAngle = -fAngle;
		obstacle->position = {
				position.x - cosf(fAngle) * (radi + radius() - 2),
				position.y,
				position.z - sinf(fAngle) * (radi + radius() - 2)
		};
		return obstacle;
	}

	void Ball::disable(bool disabled) {
		bDisabled = disabled;
		if (DBG) LogV(TAG, SF(" disabled %d", disabled));
	}

	glm::vec3 Ball::calculateOverlapDisplacement(Ball *target, bool outer) {

		// Distance between ball centers
		float fDistance = distance(target);

		// Calculate displacement required
		float fOverlap = 0.5f * (fDistance - (outer ? outerRadius() : radius()) - target->radius());

		return {
				fOverlap * (fDistance != 0 ? (position.x - target->position.x) / fDistance : 1),
				fOverlap * (fDistance != 0 ? (position.y - target->position.y) / fDistance : 1),
				fOverlap * (fDistance != 0 ? (position.z - target->position.z) / fDistance : 1)
		};

	}

	bool Ball::isPointInBall(glm::vec3 point) {
		// we are using multiplications because is faster than calling Math.pow
		float distance = ((point.x - position.x) * (point.x - position.x) +
						  (point.y - position.y) * (point.y - position.y) +
						  (point.z - position.z) * (point.z - position.z));
		return fabs(distance) < radius() * radius();
	}

	bool Ball::intersects(rgl::Ball *point, bool outer) {
		// we are using multiplications because is faster than calling Math.pow

		glm::vec3 *p = &point->position;

		float outerRadi = outer ? outerRadius() : 0;

		float distance = ((p->x - position.x) * (p->x - position.x) +
						  (p->y - position.y) * (p->y - position.y) +
						  (p->z - position.z) * (p->z - position.z)),
				sumRadius = (outer ? outerRadi : radius()) + point->radius();

		return (outer && outerRadi == 0) ? false : (fabs(distance) < sumRadius * sumRadius);

	}

	Overlaps_t Ball::overlaps(rgl::Ball *point) {
		// we are using multiplications because is faster than calling Math.pow
		if (intersects(point, false)) return OVERLAPS;
		if (outerRadius() > 0 && intersects(point, true)) return OVERLAPS_OUTER;
		return NO_OVERLAPS;

	}

	// whether a ball overlaps this one
	Overlaps_t Ball::overlaps2D(Ball *b2) {


		// 2d overlap
		float vect = fabs((position.x - b2->position.x) * (position.x - b2->position.x) +
						  (position.z - b2->position.z) * (position.z - b2->position.z));

		bool overlaps2d = vect <= (radius() + b2->radius()) * (radius() + b2->radius());

		// does not overlap. Lets check outer radius for collision prediction
		return fOuterRadius > 0
			   && vect <= (outerRadius() + b2->radius()) * (outerRadius() + b2->radius())
			   ? OVERLAPS_OUTER : NO_OVERLAPS;
	}

// subclasses MUST call this
	void Ball::onCollision(Ball *other, float fElapsedTime, float newSpeed, float newAngle) {
		fAngle = newAngle;
		fSpeed = newSpeed;
	}

	void Ball::onFutureCollision(Ball *other) {}

	void Ball::commitSimulation() {

		// Time displacement - we knew the velocity of the ball, so we can estimate the distance it should have covered
		// however due to collisions it could not do the full distance, so we look at the actual distance to the collision
		// point and calculate how much time that journey would have taken using the speed of the object. Therefore
		// we can now work out how much time remains in that timestep.

		if (fSpeed != 0 || true) {

			float fIntendedSpeed = fSpeed;
			float fActualDistance = sqrtf(
					(position.x - origPos.x) * (position.x - origPos.x) + (position.z - origPos.z) * (position.z - origPos.z));
			float fActualTime = fActualDistance / fIntendedSpeed;

			// After static resolution, there may be some time still left for this epoch,
			// so allow simulation to continue

			fSimTimeRemaining = fSimTimeRemaining - fabs(fActualTime);
			if (fSimTimeRemaining < 0) fSimTimeRemaining = 0;

		}

	}

	Ball *Ball::process(float fTime, Drawable *sprHeights) {

		if (fTime == NOTIME) {

			// Flag NOTIME is used from outside so the simulation loop does not
			// need to access ball private properties to call this function

			fTime = fSimTimeRemaining;
			origPos = position;                                // Store original position this epochoverla
		}

		// fAcceleration += fOverAcceleration *  fTime;
		bool wasPositive = fSpeed >= 0;
		fSpeed += fAcceleration * fTime - TERRAINFRICTION * fTime * (1 - fmin(fSpeed / 0.2, 1));
//	if (fSpeed < 0 && !bReverse) fSpeed = 0;
		if (fSpeed < 0 && wasPositive & !bReverse) fSpeed = 0;

		// Speed vector
		float vx = cosf(fAngle) * fSpeed,
				vy = sinf(fAngle) * fSpeed;

		// Update position
		position.x += vx * fTime * KSPEED;
		position.z += vy * fTime * KSPEED;

		// Stop ball when velocity is neglible
		if (fabs(fSpeed) < STABLE && !bForward && !bReverse && fAcceleration == 0)
			fSpeed = fAcceleration = fOverAcceleration = 0;

		// process heights from heightmap
		if (sprHeights != nullptr)
			return processHeights(fTime, sprHeights);

		return nullptr;
	}


	Ball *Ball::processHeights(float fTime, Drawable *sprHeights) {

		if (sprHeights != nullptr) {

			float collisionRadius = radius() * 1.2f; // TODO there are constants like this one here and there, unify them !
			float height = getTerrainHeight(sprHeights, {position.x, position.z + collisionRadius});
			float heightl = getTerrainHeight(sprHeights, {position.x - collisionRadius, position.z + collisionRadius});
			float heightr = getTerrainHeight(sprHeights, {position.x + collisionRadius, position.z + collisionRadius});

			fAngleTerrain = atan2((10 * (heightr - heightl)), (2 * collisionRadius));
			//	std::cerr << "angle terr " << fAngleTerrain * 180 / 3.141592 << std::endl;
			//	std::cerr << "height  terr " << height << " ply " << fHeight << std::endl;


			if (position.y > height) {

				// downhill
				if (position.y - height < FEATURES_FALL_LIMIT) {
					// gong down
					fHeightTarget = height;
				} else {
					// fell from high
					// std::cerr << "GOING DOWN !!" << std::endl;
					fHeightTarget = height;
				}

			} else if (position.y < height) {

#ifdef DBG_NOHEIGHTMAPCOLLISIONS

				// at the moment we are making a limited use of the height map with respect to collisions
				// because it is a little random as the heights are very irregular. So we only use it to
				// impose penalties on the speed depending on the gradient

				float delta = height - fHeight;
				if (delta < RIDEHEIGHT_SEAMLESS) {

					// we can ride seamlessly across this irregularity

					fHeight = fHeightTarget = height;		// accept new height
					fPenalty = 1;							// seamlessly drive

				} else {

					// The terrain is pretty irregular, so impose a penalty on the car performance.
					// fPenalty is the percent of penalty to impose, 0  stops the car, 1 unaffected

					fPenalty =  FEATURES_SCRATCHING_NEW + (1 - fmin(delta, FEATURES_CLIMB_LIMIT)/FEATURES_CLIMB_LIMIT) * (1-FEATURES_SCRATCHING_NEW);

					fSpeed*= fPenalty;	// this only affects human player as CPU uses acceleration to drive
										// but that's why we keep the calculated penalty so it can be
										// added in the CPU car drive routines where it makes sense

					fHeight = fHeightTarget = height;		// accept new height

					if (DBG) std::cerr << "Height Delta " << delta<< " penalty " << fPenalty << std::endl;

				}

#else
				// this is supposedly more elaborated as after a height threshold a fake ball is used
				// to simulate a collision, however I cant get it to work predictably

				//// climb
				if (height - position.y < FEATURES_CLIMB_LIMIT) {

					// we can climb this (ramp ...)
					//	std::cerr << height - fHeight << " clb h " << height << std::endl;

					position.y = fHeightTarget = height;
					//	std::cerr << "easy climb " << std::endl;
					//	if (height-fHeight > sFeatures.CLIMB_EASY)
					//		fSpeed*=sFeatures.SCRATCHING;

				} else {

					//	std::cerr << "ostion"<<std::endl;
					// a wall or something higher than the car
					//	std::cerr << "CRASH !!" << fSpeed << " fh " << fHeight <<std::endl;
					//	fSpeed=fmin(-abs(fSpeed),-.02);

					Ball *obstacle = makeCollisionBall(4.0, mass()); // mass*0.8
					float fDistance = distance(obstacle);

					// Calculate displacement required
					float fOverlap = 1.0f * (fDistance - radius() - obstacle->radius());

					// Displace Current Ball away from collision
					position.x -= fOverlap * cos(-fAngle); //  * (position.x - obstacle->position.x) / fDistance;
					position.z -= fOverlap * sin(-fAngle); //  * (position.z - obstacle->position.z) / fDistance;

					//	float height = sprHeights->Sample(fmod(position.x/sprHeights->width,1.0f), fmod(position.z/sprHeights->height, 1.0f)).r / (float)255;
					//	fHeight = fHeightTarget = height;
					//	fHeightTarget = height;
					//	std::cerr << "new height "<<fHeight<<std::endl;

					return obstacle;
				}
#endif
			}

			float ACCELERATION_EARTH = 9.8 / 10; // i found a 9.8 - ish value that makes sense so let´s keep it like this :)

			if (fAccelerationZ > -ACCELERATION_EARTH || position.y != fHeightTarget) {

				float vz = fAccelerationZ * fTime;
				float sz = vz * fTime * iWorldWidth;

				if (fAccelerationZ > 0) {

					// upwards
					position.y += sz;
					// if (DBG) std::cerr << "fly up  "<<fHeight<<" vz "<<vz<<" az "<<fAccelerationZ <<std::endl;
					bFlying = true;

				} else {

					// falling
					// if (DBG) std::cerr << "fall down "<<fHeight<<" vz "<<vz<<" az " << fAccelerationZ << std::endl;
					position.y += sz;
					if (position.y < fHeightTarget) {
						position.y = fHeightTarget;
						bFlying = false;
					}
				}

				fAccelerationZ -= fTime;

				if (fAccelerationZ < -ACCELERATION_EARTH)
					fAccelerationZ = -ACCELERATION_EARTH;

			}

		}

		return nullptr;

	}

	float LinearDelayer::tick(float fElapsedTime) {

		if (fabs(fCurrent - fTarget) > fIncrement * fElapsedTime) {

			if (fCurrent < fTarget)
				fCurrent += fIncrement * fElapsedTime;
			else if (fCurrent > fTarget)
				fCurrent -= fIncrement * fElapsedTime;

		} else fCurrent = fTarget;

		return fCurrent;
	}

}