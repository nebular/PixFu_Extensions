//
// Created by rodo on 2020-02-20.
//

#pragma once

#include "Ball.hpp"
#include "Meta.hpp"
#include "PlayerFeatures.hpp"

namespace Pix {

	typedef struct sWorldMeta {
		std::string name;
		int xMas, yMax;
	} WorldMeta_t;

	class BallPlayer : public Ball {

	protected:
		
		const PlayerFeatures *FEATURES;
		float fAcceleration = 0;
		float fCalcDirection = 0;
//		float fSteerAngle = 0;

	public:

		float fSteerAngle = 0;

		BallPlayer(World *world, ObjectFeatures_t objectMeta, PlayerFeatures_t features = {});

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
		BallPlayer4wheels(World *world, ObjectFeatures_t objectMeta, PlayerFeatures_t features = {});
		void process(World *world, float fTime);
	};

	inline BallPlayer4wheels::BallPlayer4wheels(World *world, ObjectFeatures_t objectMeta, PlayerFeatures_t features)
	: BallPlayer (world, objectMeta, features) {}


	class GameObject : public Ball {
	public:
		GameObject(World *world, ObjectFeatures_t features)
		: Ball(world->CONFIG, features) {}
	};
}
