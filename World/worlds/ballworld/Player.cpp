#include "Player.hpp"
#include "Utils.hpp"
#include "glm/gtx/rotate_vector.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "err_ovl_no_viable_member_function_in_call"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

#define NO_DEBUG_CARPHYSICS


namespace Pix {

	Player::Player(World *world, PlayerFeatures_t features)
			: Ball(world, features.className, features.config.position,
				   features.config.radius,
				   features.config.mass,
				   false),
			  FEATURES(new PlayerFeatures(features)) {
	}

	// process ball iteration
	// we override the Ball update method as we will simulate a car more accurately
	// using axis distance and wheel positions.

	void Player::process(World *world, float fElapsedTime) {

		// turn off simple simulation (below) and use car simulation
		constexpr bool SIMPLE_BALL = false;

		//	auto toDeg = [] (float rad) { return (int)(rad*180/M_PI); };

#ifdef DEBUG_CARPHYSICS
		Canvas2D *canvas = world->canvas(mPosition);
#endif

		if (fElapsedTime == NOTIME) {
			fElapsedTime = fSimTimeRemaining;
			origPos = mPosition;                                // Store original position this epochoverla
		}

		// SIMPLE BALL rotates the ball speed vector according to tthe steering wheel
		// this is the simplest simulatino

		if (SIMPLE_BALL) {
			// rotate the speed vector ccording to the steering wheel
			mSpeed = glm::rotate(mSpeed, fSteerAngle, {0, 1, 0});
			fCalcDirection = atan2(mSpeed.z, mSpeed.x);

			// visually rotate player & set camera (camera follow will use this rotation - player absolute rotation)
			mRotation.y = -fCalcDirection;

			// add pedal acceleration to the car heading
			glm::vec3 head = {
					cosf(fCalcDirection),
					0,
					sinf(fCalcDirection)
			};

			const float fSpeed = speed();
			const float accAmount = fAcceleration * (1 - FEATURES->speedPercent(fSpeed));

			mAcceleration = accAmount * head;

			// process the ball parameters
			Ball::process(world, fElapsedTime);
		}

		// following is a simulation based on that website that models back and front axis
		// so steering is applied to the front wheels
		// http://engineeringdotnet.blogspot.com/2010/04/simple-2d-car-physics-in-games.html

		const float modSpeed = speed();
		const float steerAngle = fSteerAngle;

		// when speed = 0 we dont know the car heading so use last one
		fCalcDirection = modSpeed > STABLE ? atan2(mSpeed.z, mSpeed.x) : fCalcDirection;

		// the current car direction, calculated from the velocity vector
		float ang = fCalcDirection;

		// the direction vector of the back wheels
		glm::vec3 headingBack = glm::vec3(cosf(ang), 0, sinf(ang));

		// the direction vector of the front wheels, that is affected by the steering
		glm::vec3 headingFront = glm::vec3(cosf(ang + steerAngle), 0, sinf(ang + steerAngle));

		/**
		 We can use geometry calculations to find the actual positions of the wheels
		 (which are at the centre of each axle). The gap between the axles is given by
		 the wheelBase, so each wheel is half that distance from the car centre, in the
		 direction that the car is facing.
		 */

		const glm::vec3 offset = FEATURES->wheelBase() / 2 * headingBack;
		glm::vec3 frontWheel = mPosition + offset;
		glm::vec3 backWheel = mPosition - offset;

#ifdef DEBUG_CARPHYSICS
		canvas->blank();
		canvas->fillCircle(static_cast<int32_t>(frontWheel.x), static_cast<int32_t>(frontWheel.z), 2, Pix::Colors::RED);
		canvas->fillCircle(static_cast<int32_t>(backWheel.x), static_cast<int32_t>(backWheel.z), 2, Pix::Colors::GREEN);
#endif

		/**
		 Each wheel should move forward by a certain amount in the direction it is pointing.
		 The distance it needs to move depends on the car speed, and the time between frames
		 The rear wheel is easy, it moves in the same direction the car is heading.
		 For the front wheel, we have to add the steer angle to the car heading
		 */

		//	backWheel += carSpeed * dt * new Vector2(cos(carHeading) , sin(carHeading));
		//	frontWheel += carSpeed * dt * new Vector2(cos(carHeading+steerAngle) , sin(carHeading+steerAngle));

		backWheel += modSpeed * fElapsedTime * headingBack;
		frontWheel += modSpeed * fElapsedTime * headingFront;

#ifdef DEBUG_CARPHYSICS
		canvas->fillCircle(static_cast<int32_t>(frontWheel.x), static_cast<int32_t>(frontWheel.z), 2, Pix::Colors::BLACK);
		canvas->fillCircle(static_cast<int32_t>(backWheel.x), static_cast<int32_t>(backWheel.z), 2, Pix::Colors::GREY);
#endif
		/*
		The new car position can be calculated by averaging the two new wheel positions.
		*/

		mPosition = (frontWheel + backWheel) / 2.0f;

#ifdef DEBUG_CARPHYSICS
		canvas->fillCircle(static_cast<int32_t>(mPosition.x), static_cast<int32_t>(mPosition.z), 2, Pix::Colors::BLUE);
#endif

		/*
		 The new car heading can be found by calculating the angle of the line between the
		 two new wheel positions:
		 */

		const float newAngle = atan2(frontWheel.z - backWheel.z, frontWheel.x - backWheel.x);

		// rotation.y is player's rotation around y, and is used by the camera when following
		// this player

		mRotation.y = -newAngle;

		// Rotate velocity vector
		mSpeed = glm::rotate(mSpeed, newAngle - fCalcDirection, {0, 1, 0});

		//////////// END CAR HANDLING. Begin speed / acceleration

		// Now, acceleration, we have the modulus, and we set the new heading
		const glm::vec3 head = {cosf(newAngle), 0, sinf(newAngle)};
		const float accAmount = fAcceleration * (1.0f - FEATURES->speedPercent(modSpeed));
		mAcceleration = accAmount * head;

		if (accAmount != 0) {
			// add acceleration
			mSpeed.x += mAcceleration.x * fElapsedTime;    // Update Velocity
			mSpeed.z += mAcceleration.z * fElapsedTime;
		}
	}

	void Player::steer(float perc, float fElapsedTime) {
		//	fSteerAngle = perc * M_PI / 512; // * (- log2(fabs(speedPercent()) + 0.000001));
		fSteerAngle = static_cast<float>(perc * M_PI / 4 * (1 - 0.85 * speedPercent())); // * (- log2(fabs(speedPercent()) + 0.000001));
	}

	void Player::accelerate(float percentage, float fElapsedTime) {

		// MAXSPEED == speed -> 0;
		// MINSPEED -> KACCEL

		float KACCEL = 100;

		bReverse = false;
		bForward = true;

		fAcceleration = KACCEL * percentage;
	}

	void Player::brake(float percentage, float fElapsedTime) {

		float KDECCEL = 200 * (1 - speedPercent());
		fAcceleration = -KDECCEL * percentage;

	}

	void Player::jump() {

	}

}

#pragma clang diagnostic pop
