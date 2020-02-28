//
//  Ball.cpp
//  javid
//
//  Created by rodo on 11/01/2020.
//  Copyright © 2020 rodo. All rights reserved.
//	Copyright ©

#include "Ball.hpp"
#include "Utils.hpp"
#include "World.hpp"
#include "WorldMeta.hpp"
#include "Config.hpp"
#include <cmath>
#include "gtx/fast_square_root.hpp"

namespace rgl {

	constexpr int KSPEED = 1000;
	// global scale for ball radius
	float Ball::stfBaseScale = 1.0;
	float Ball::stfHeightScale = 1.0;
	int Ball::instanceCounter = 0;

	Ball::Ball(const WorldConfig_t &planetConfig, std::string className, glm::vec3 position, float radi, float mass, bool isStatic,
			   bool isPlayer)
			: WorldObject(planetConfig, std::move(className)),
			  ID(instanceCounter++),
			  RADIUS(radi),
			  MASS(mass),
			  ISPLAYER(isPlayer),
			  ISSTATIC(isStatic),
			  mPosition(position),
			  bFlying(false) {
		TAG = "BALL" + std::to_string(ID);
		setRadiusMultiplier(1.0);
	}

	Ball::Ball(const WorldConfig_t &planetConfig, float radi, float mass, glm::vec3 position, glm::vec3 speed)
			: WorldObject(planetConfig, "FAKE"),
			  ID(FAKE_BALL_ID),
			  RADIUS(radi),
			  MASS(mass),
			  ISSTATIC(false),
			  ISPLAYER(false),
			  bFlying(false),
			  mPosition(position),
			  mSpeed(speed) {
		TAG = "BALL" + std::to_string(ID);
		setRadiusMultiplier(1.0);
	}

	Ball *Ball::makeCollisionBall(float radi, glm::vec3 position) {
		return new Ball(PLANET, radi, MASS * 0.8f, position, {-mSpeed.x, 0, -mSpeed.z});
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
				fOverlap * (fDistance != 0 ? (mPosition.x - target->mPosition.x) / fDistance : 1),
				fOverlap * (fDistance != 0 ? (mPosition.y - target->mPosition.y) / fDistance : 1),
				fOverlap * (fDistance != 0 ? (mPosition.z - target->mPosition.z) / fDistance : 1)
		};
	}

	bool Ball::isPointInBall(glm::vec3 point) {
		// we are using multiplications because is faster than calling Math.pow
		float distance = ((point.x - mPosition.x) * (point.x - mPosition.x) +
						  (point.y - mPosition.y) * (point.y - mPosition.y) +
						  (point.z - mPosition.z) * (point.z - mPosition.z));
		return fabs(distance) < radius() * radius();
	}

	bool Ball::intersects(rgl::Ball *point, bool outer) {
		// we are using multiplications because is faster than calling Math.pow

		glm::vec3 *p = &point->mPosition;

		float outerRadi = outer ? outerRadius() : 0;

		float distance = ((p->x - mPosition.x) * (p->x - mPosition.x) +
						  (p->y - mPosition.y) * (p->y - mPosition.y) +
						  (p->z - mPosition.z) * (p->z - mPosition.z)),
				sumRadius = (outer ? outerRadi : radius()) + point->radius();

		return (outer && outerRadi == 0) ? false : (fabs(distance) < sumRadius * sumRadius);

	}

	Overlaps_t Ball::overlaps(rgl::Ball *point) {
		// we are using multiplications because is faster than calling Math.pow
		if (intersects(point, false)) return OVERLAPS;
		if (outerRadius() > 0 && intersects(point, true)) return OVERLAPS_OUTER;
		return NO_OVERLAPS;
	}


// subclasses MUST call this
	void Ball::onCollision(Ball *otherBall, glm::vec3 newSpeedVector, float fElapsedTime) {
		LogV(TAG, SF("I crashed"));
		mSpeed = newSpeedVector;
	}

	void Ball::onFutureCollision(Ball *other) {}

	void Ball::commitSimulation() {

		// Time displacement - we knew the velocity of the ball, so we can estimate the distance it should have covered
		// however due to collisions it could not do the full distance, so we look at the actual distance to the collision
		// point and calculate how much time that journey would have taken using the speed of the object. Therefore
		// we can now work out how much time remains in that timestep.

		float fIntendedSpeed = speed();

		float fActualDistance = glm::fastSqrt(
				(mPosition.x - origPos.x) * (mPosition.x - origPos.x)
				+ (mPosition.z - origPos.z) * (mPosition.z - origPos.z));

		float fActualTime = fActualDistance / fIntendedSpeed;

		// After static resolution, there may be some time still left for this epoch,
		// so allow simulation to continue

		fSimTimeRemaining = fSimTimeRemaining - fabs(fActualTime);
		if (fSimTimeRemaining < 0) fSimTimeRemaining = 0;


	}

	Ball *Ball::process(World *world, float fTime) {

		if (fTime == NOTIME) {

			// Flag NOTIME is used from outside so the simulation loop does not
			// need to access ball private properties to call this function

			fTime = fSimTimeRemaining;
			origPos = mPosition;                                // Store original position this epochoverla
		}

		fMetronome += fTime;

		mAcceleration.z *= 0.8;
		mAcceleration.x *= 0.8;

		mSpeed.x += mAcceleration.x * fTime;    // Update Velocity
		mSpeed.z += mAcceleration.z * fTime;
		mPosition.x += mSpeed.x * fTime;            // Update position
		mPosition.z += mSpeed.z * fTime;


		// Stop ball when velocity is neglible
		if (fabs(mSpeed.x * mSpeed.x + mSpeed.z * mSpeed.z) < STABLE) {
			mSpeed.x = 0;
			mSpeed.z = 0;
		}

		// process heights from heightmap
		return processHeights(world, fTime);
	}


	Ball *Ball::processHeights(World *world, float fTime) {

		float collisionRadius = radius() * 1.2f; // TODO there are constants like this one here and there, unify them !

		glm::vec3 chk = {mPosition.x, 0, mPosition.z + collisionRadius};
		float cheight = world->getHeight(chk);
		chk.x = mPosition.x - collisionRadius;
		float heightl = world->getHeight(chk);
		chk.x = mPosition.x + collisionRadius;
		float heightr = world->getHeight(chk);
		fAngleTerrain = atan2((10 * (heightr - heightl)), (2 * collisionRadius));

//		if (fabs((rotation.x-fAngleTerrain))>M_PI/2) rotation.x = fAngleTerrain;
//		else rotation.x += (rotation.x - fAngleTerrain) * 0.5 * fTime;
// TODO
		//	std::cerr << "angle terr " << fAngleTerrain * 180 / 3.141592 << std::endl;
		//	std::cerr << "height  terr " << height << " ply " << fHeight << std::endl;

		if (mPosition.y > cheight) {

			// downhill
			if (mPosition.y - cheight < FEATURES_FALL_LIMIT) {
				// gong down
				fHeightTarget = cheight;
			} else {
				// fell from high
				// std::cerr << "GOING DOWN !!" << std::endl;
				fHeightTarget = cheight;
			}

		} else if (mPosition.y < cheight) {

#ifdef DBG_NOHEIGHTMAPCOLLISIONS

			// at the moment we are making a limited use of the height map with respect to collisions
			// because it is a little random as the heights are very irregular. So we only use it to
			// impose penalties on the speed depending on the gradient

			float delta = cheight - mPosition.y;
			if (delta < RIDEHEIGHT_SEAMLESS) {

				// we can ride seamlessly across this irregularity

				mPosition.y = cheight;
				fHeightTarget = cheight;        // accept new height
				fPenalty = 1;                    // seamlessly drive

			} else {

				// The terrain is pretty irregular, so impose a penalty on the car performance.
				// fPenalty is the percent of penalty to impose, 0  stops the car, 1 unaffected

				fPenalty = FEATURES_SCRATCHING_NEW +
						   (1 - fmin(delta, FEATURES_CLIMB_LIMIT) / FEATURES_CLIMB_LIMIT) * (1 - FEATURES_SCRATCHING_NEW);

				mSpeed *= fPenalty;    // this only affects human player as CPU uses acceleration to drive
				// but that's why we keep the calculated penalty so it can be
				// added in the CPU car drive routines where it makes sense

				mPosition.y = fHeightTarget = cheight;        // accept new height

				if (DBG) std::cerr << "Height Delta " << delta << " penalty " << fPenalty << std::endl;

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

				Ball *obstacle = makeCollisionBall(4.0,position); // mass*0.8
				float fDistance = distance(obstacle);

				// Calculate displacement required
				float fOverlap = 1.0f * (fDistance - radius() - obstacle->radius());

				// Displace Current Ball away from collision
				position.x -= fOverlap*(position.x - obstacle->position.x) / fDistance;
				position.z -= fOverlap*(position.z - obstacle->position.z) / fDistance;

				return obstacle;
			}
#endif
		}

		float ACCELERATION_EARTH = 9.8; // i found a 9.8 - ish value that makes sense so let´s keep it like this :)
//		float ACCELERATION_EARTH = 9.8 / 10 ; // i found a 9.8 - ish value that makes sense so let´s keep it like this :)

		if (fAccelerationZ > -ACCELERATION_EARTH || mPosition.y != fHeightTarget) {

			float vz = fAccelerationZ * fTime * KSPEED;
			float sz = vz * fTime;

			if (fAccelerationZ > 0) {

				// upwards
				mPosition.y += sz;
				// if (DBG) std::cerr << "fly up  "<<fHeight<<" vz "<<vz<<" az "<<fAccelerationZ <<std::endl;
				bFlying = true;

			} else {

				// falling
//					if (DBG) std::cerr << "fall down "<<position.y<<" vz "<<vz<<" az " << fAccelerationZ << std::endl;
				mPosition.y += sz;
				if (mPosition.y < fHeightTarget) {
					mPosition.y = fHeightTarget;
					bFlying = false;
				}
			}

			fAccelerationZ -= fTime;

			if (fAccelerationZ < -ACCELERATION_EARTH)
				fAccelerationZ = -ACCELERATION_EARTH;

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
