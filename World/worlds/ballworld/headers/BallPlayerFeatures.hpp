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

		/** Max Speed in km/h */
		const float MAXSPEED = 160;
		/** TODO max speed backwards */
		const float MAXSPEEDBACK = 50;
		/** Maximum Steer Angle */
		const float MAXSTEERANGLE = (float) M_PI / 4;

		const float SCALE = 1.0;                    // player scale
		const float MASS = 1000;                    // player mass in kg

	} BallPlayerFeatures_t;


	class BallPlayerFeatures {

		const ObjectProperties_t OBJECT;
		const BallPlayerFeatures_t FEATURES;

		//	const Weapon_t *pActiveWeapon = nullptr;	// player active weapon
		//	const Weapon_t *pPassiveWeapon = nullptr;	// other player's weapon side effects

	public:

		BallPlayerFeatures(ObjectProperties_t objectMeta, BallPlayerFeatures_t features);

//		void setActiveWeapon(const Weapon_t *weapon, Ball *player);
//		void setPassiveWeapon(const Weapon_t *weapon, Ball *player, bool enable = true);

		/** Used: The wheel distance as percent of raduis */
		float wheelBase() const;

		/** Used: returns speed percent r/ maximum speed*/
		float speedPercent(float speed) const;

		/** Used: return max speed */
		float maxSpeed() const;

		float maxSpeedBack() const;

		float maxSteerAngle() const;

		float mass() const;

		float scale() const;
	};


// PLAYER FEATURES

	inline BallPlayerFeatures::BallPlayerFeatures(ObjectProperties_t objectMeta, BallPlayerFeatures_t features) :
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

	inline float
	BallPlayerFeatures::wheelBase() const { return OBJECT.radius * 1.7F; }

	inline float
	BallPlayerFeatures::maxSpeed() const { return FEATURES.MAXSPEED; } // * (pActiveWeapon!=nullptr?pActiveWeapon->maxSpeedMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->maxSpeedMultiplier: 1.0); }
	inline float BallPlayerFeatures::speedPercent(float speed) const { return (float) fmin(fabs(speed) / FEATURES.MAXSPEED, 1); }

	inline float
	BallPlayerFeatures::maxSpeedBack() const { return FEATURES.MAXSPEEDBACK; } // * (pActiveWeapon!=nullptr?pActiveWeapon->maxSpeedMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->maxSpeedMultiplier: 1.0); }

	inline float
	BallPlayerFeatures::maxSteerAngle() const { return FEATURES.MAXSTEERANGLE; } // * (pActiveWeapon!=nullptr?pActiveWeapon->maxSpeedMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->maxSpeedMultiplier: 1.0); }

	inline float
	BallPlayerFeatures::mass() const { return FEATURES.MASS; } //  * (pActiveWeapon!=nullptr?pActiveWeapon->massMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->massMultiplier: 1.0); }

	inline float
	BallPlayerFeatures::scale() const { return FEATURES.SCALE; } // * (pActiveWeapon!=nullptr?pActiveWeapon->scaleMultiplier : pPassiveWeapon != nullptr ? pPassiveWeapon->scaleMultiplier: 1.0); }

}
