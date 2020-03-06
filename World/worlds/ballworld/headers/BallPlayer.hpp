//
// Created by rodo on 2020-02-20.
//

#pragma once

#include "Ball.hpp"
#include "BallPlayerFeatures.hpp"

namespace Pix {

	class BallPlayer : public Ball {

	protected:

		const BallPlayerFeatures *FEATURES;
		float fAcceleration = 0;
		float fCalcDirection = 0;
//		float fSteerAngle = 0;

	public:

		float fSteerAngle = 0;

		BallPlayer(World *world, ObjectProperties_t& objectMeta, BallPlayerFeatures_t features, ObjectLocation_t location);

		virtual void process(World *world, float fTime);

		void steer(float angle, float fElapsedTime);

		void accelerate(float percentage, float fElapsedTime);

		void brake(float percentage, float fElapsedTime);

		void jump();

		float speedPercent();
	};

	// speed percent r/max speed
	inline float BallPlayer::speedPercent() { return FEATURES->speedPercent(speed()); }

	/**
	 * A more convincing simulation of a 4-wheel car
	 */

	class BallPlayer4wheels : public BallPlayer {
	public:
		BallPlayer4wheels(World *world, ObjectProperties_t objectMeta, BallPlayerFeatures_t features = {}, ObjectLocation_t location = {});

		void process(World *world, float fTime);
	};

	inline BallPlayer4wheels::BallPlayer4wheels(World *world, ObjectProperties_t objectMeta, BallPlayerFeatures_t features,
												ObjectLocation_t location)
			: BallPlayer(world, objectMeta, features, location) {}


	class GameObject : public Ball {
	public:
		GameObject(World *world, ObjectProperties_t features, ObjectLocation_t loc)
				: Ball(world->CONFIG, features, loc, 0) {}
	};
}
