//
// Created by rodo on 2020-02-20.
//

#pragma once

#include "Ball.hpp"
#include "Meta.hpp"
#include "PlayerFeatures.hpp"

namespace rgl {

	typedef struct sWorldMeta {
		std::string name;
		int xMas, yMax;
	} WorldMeta_t;

	typedef struct sObjectFeatures {
		std::string className;
		sObjectConfig config;
		bool isStatic = true;
	} ObjectFeatures_t;

	class Player : public Ball {

		const PlayerFeatures *FEATURES;

		const int MAX_STEER_MOMENTUM = 20 * 10;

		float fSteerAngle = 0;
		float fSteerMomentum = 0;
		
	public: 
		Player(PlayerFeatures_t features);
		Ball *process(World *world, float fTime);
		void steer(float angle, float fElapsedTime);
		void accelerate(float percentage, float fElapsedTime);
		void brake(float percentage, float fElapsedTime);
		void jump();
		
		float getSpeedPercent();
	};

	// speed percent r/max speed
	inline float Player::getSpeedPercent() { return fmax(fabs(fSpeed) / FEATURES->maxSpeed(), 1); }

	class GameObject : public Ball {
	public:
		GameObject(ObjectFeatures_t features)
				: Ball(features.className, features.config.position,
					   features.config.radius, features.config.mass, features.isStatic) {}
	};
}
