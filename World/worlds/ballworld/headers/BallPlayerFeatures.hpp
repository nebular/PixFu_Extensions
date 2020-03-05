//
//  PlayerFeatures.hpp
//  PixEngine
//
//  Created by rodo on 21/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "World.hpp"

namespace Pix {

	// TODO most are not used, review and remove
	typedef struct sBallPlayerFeatures {

		const float MAXSPEED = 160;                // max speed in m/h, 0.15 = 150 km/h and kind of makes sense for the map
		const float MAXSPEEDBACK = 50;            // max speed backwards

		const float FRICTION = 0.001;                // speed lose on no gas. change this name, it is not friction.
		const float ACCELERATION = 0.003;            // max car acceleration
		const float HANDLING = 18;//12;				// max turn angle (value is not an angle, but related to it), higher = sharper
		const float TRACTION = 2;                    // turn speed and back to center on no turn input
		const float TURNPERFORMANCE = 0.01; //0.03;	// speed lose on turns. Beware has to be in the 0.0xx order
		const float FALL_LIMIT = 0.3;                // (heightmap collisions) height difference to consider a fall rather than just climb down
		const float CLIMB_LIMIT = 0.3; // 0.2;		// (heightmap collisions) height difference to consider a wreck against a wall rather than just climb up
		const float CLIMB_EASY = 0.06;                // (heightmap collisions) height difference to consider a wreck against a wall rather than just climb up
		const float SCRATCHING = 0.96;                // speed lose on height change (1 = none)
		const float SCALE = 1.0;                    // player scale
		const float MASS = 1000;                    // player mass in kg

	} BallPlayerFeatures_t;


	class BallPlayerFeatures {

		const ObjectMeta_t OBJECT;
		const BallPlayerFeatures_t FEATURES;
		//	const Weapon_t *pActiveWeapon = nullptr;	// player active weapon
		//	const Weapon_t *pPassiveWeapon = nullptr;	// other player's weapon side effects

	public:

		BallPlayerFeatures(ObjectMeta_t objectMeta, BallPlayerFeatures_t features);

//		void setActiveWeapon(const Weapon_t *weapon, Ball *player);
//		void setPassiveWeapon(const Weapon_t *weapon, Ball *player, bool enable = true);



		float wheelBase() const;

		float speedPercent(float speed) const;

		float maxSpeed() const;

		float maxSpeedBack() const;

		float maxAcceleration() const;

		float friction() const;

		float handling() const;

		float traction(float multiplier = 1.0) const;

		float turnPerformance(float multiplier = 1.0) const;

		float fallLimit() const;

		float climbLimit() const;

		float climbEasy() const;

		float scratching() const;

		float mass() const;

		float scale() const;
	};


// PLAYER FEATURES

	inline BallPlayerFeatures::BallPlayerFeatures(ObjectMeta_t objectMeta, BallPlayerFeatures_t features) :
			OBJECT(std::move(objectMeta)),
			FEATURES(std::move(features)) {}

/*
inline void PlayerFeatures::setActiveWeapon(const Weapon_t *weapon, Ball *player)  {
	pActiveWeapon = weapon;
	player->setMassMultiplier(weapon == nullptr || weapon->massMultiplier == 0 ? 1.0 : weapon->massMultiplier);
	player->setRadiusMultiplier(weapon == nullptr || weapon->scaleMultiplier == 0 ? 1.0 : weapon->scaleMultiplier);
}

inline void PlayerFeatures::setPassiveWeapon(const Weapon_t *weapon, Ball *player, bool enable) {
	if (!enable && weapon != nullptr) {
		if (weapon != pPassiveWeapon) {
			// disable passive weapon, but there is other later passive weapon active that replaced
			// this one
			return;
		} else {
			weapon = nullptr;
		}
	}
	pPassiveWeapon = weapon;
	player->setMassMultiplier(weapon == nullptr || weapon->massMultiplier == 0 ? 1.0 : weapon->massMultiplier);
	player->setRadiusMultiplier(weapon == nullptr || weapon->scaleMultiplier == 0 ? 1.0 : weapon->scaleMultiplier);
}
*/
	inline float BallPlayerFeatures::wheelBase() const {
		return OBJECT.PROPERTIES.radius * 1.7;
	} // * (pActiveWeapon!=nullptr?pActiveWeapon->maxSpeedMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->maxSpeedMultiplier: 1.0); }

	inline float
	BallPlayerFeatures::maxSpeed() const { return FEATURES.MAXSPEED; } // * (pActiveWeapon!=nullptr?pActiveWeapon->maxSpeedMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->maxSpeedMultiplier: 1.0); }

	inline float BallPlayerFeatures::speedPercent(float speed) const {
		return fmin(fabs(speed) / FEATURES.MAXSPEED, 1);
	}

	inline float
	BallPlayerFeatures::maxSpeedBack() const { return FEATURES.MAXSPEEDBACK; } // * (pActiveWeapon!=nullptr?pActiveWeapon->maxSpeedMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->maxSpeedMultiplier: 1.0); }

	inline float BallPlayerFeatures::maxAcceleration() const { return FEATURES.ACCELERATION; } // todo
	inline float BallPlayerFeatures::friction() const { return FEATURES.FRICTION; } // todo
	inline float BallPlayerFeatures::handling() const { return FEATURES.HANDLING; } // todo
	inline float BallPlayerFeatures::traction(float multiplier) const { return FEATURES.TRACTION * multiplier; }

	inline float BallPlayerFeatures::turnPerformance(float multiplier) const { return FEATURES.TURNPERFORMANCE * multiplier; }

	inline float BallPlayerFeatures::fallLimit() const { return FEATURES.FALL_LIMIT; }

	inline float BallPlayerFeatures::climbLimit() const { return FEATURES.CLIMB_LIMIT; }

	inline float BallPlayerFeatures::climbEasy() const { return FEATURES.CLIMB_EASY; }

	inline float BallPlayerFeatures::scratching() const { return FEATURES.SCRATCHING; }

	inline float
	BallPlayerFeatures::mass() const { return FEATURES.MASS; } //  * (pActiveWeapon!=nullptr?pActiveWeapon->massMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->massMultiplier: 1.0); }
	inline float
	BallPlayerFeatures::scale() const { return FEATURES.SCALE; } // * (pActiveWeapon!=nullptr?pActiveWeapon->scaleMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->scaleMultiplier: 1.0); }
}