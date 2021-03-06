#include "BallPlayer.hpp"
#include "Utils.hpp"
#include "glm/gtx/rotate_vector.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "err_ovl_no_viable_member_function_in_call"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

namespace Pix {

	BallPlayer::BallPlayer(World *world, ObjectProperties_t &objectMeta, BallPlayerFeatures_t features, ObjectLocation_t location, std::shared_ptr<SpotLight> light)
			: Ball(world->CONFIG, objectMeta, location),
			  FEATURES(new BallPlayerFeatures(objectMeta, features)),
			  mFlashLight(light) {
	}

	/**
	 * The base class accepts a steering angle and acceleration and does simple rotations
	 * on the speed vector. The movement is ok for balls but not very realistic for cars.
	 */

	void BallPlayer::process(World *world, float fElapsedTime) {

		//	auto toDeg = [] (float rad) { return (int)(rad*180/M_PI); };

		if (fElapsedTime == NOTIME) {
			fElapsedTime = fSimTimeRemaining;
			origPos = mPosition;                                // Store original position this epochoverla
		}

		// SIMPLE BALL rotates the ball speed vector according to tthe steering wheel
		// this is the simplest simulatino

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

		if (mFlashLight != nullptr) {
			mFlashLight->position = mPosition / 1000.0f;
			glm::vec3 direction = glm::normalize(mSpeed);
			direction = glm::rotate(direction, (float)(-30*M_PI/180), glm::vec3 {0,1,0});
			mFlashLight->direction = direction;
		}

		// process the ball parameters
		Ball::process(world, fElapsedTime);

	}

	/**
	 * This derived class implements a more elaborate model to simulate cars with
	 * 4 wheels and calculating their positions according to the axis distance
	 */

	void BallPlayer4wheels::process(class World *world, float fElapsedTime) {

		if (fElapsedTime == NOTIME) {
			// this belongs to the base class but we are avoiding its call, so we have
			// to take care of it.
			fElapsedTime = fSimTimeRemaining;
			origPos = mPosition;                                // Store original position this epochoverla
		}

		// process intrinsic animation
		WorldObject::process(world, fElapsedTime); // NOLINT(bugprone-parent-virtual-call)

		const bool debug = world->CONFIG.debugMode == DEBUG_COLLISIONS;
		Canvas2D *canvas = debug ? world->canvas(mPosition) : nullptr;

		// following is a simulation based on that website that models back and front axis
		// so steering is applied to the front wheels
		// http://engineeringdotnet.blogspot.com/2010/04/simple-2d-car-physics-in-games.html

		const float modSpeed = speed();
		const float steerAngle = fSteerAngle;

		// when speed = 0 we dont know the car heading so use last one
//		fCalcDirection = modSpeed > STABLE ? atan2(mSpeed.z, mSpeed.x) : fCalcDirection;
		fCalcDirection = modSpeed > 0 ? atan2(mSpeed.z, mSpeed.x) : fCalcDirection;

		// the current car direction, calculated from the velocity vector
		float ang = fCalcDirection;

		if (debug) {

			const float TAIL = 40.0F;

			glm::vec2 r = glm::rotate(glm::vec2(TAIL, 0), ang - steerAngle);
			canvas->drawLine(static_cast<int32_t>(mPosition.x), static_cast<int32_t>(mPosition.z), static_cast<int32_t>(mPosition.x + r.x),
							 static_cast<int32_t>(mPosition.z + r.y),
							 Pix::Colors::RED);
			r = glm::rotate(glm::vec2(TAIL, 0), ang);
			canvas->drawLine(static_cast<int32_t>(mPosition.x), static_cast<int32_t>(mPosition.z), static_cast<int32_t>(mPosition.x + r.x),
							 static_cast<int32_t>(mPosition.z + r.y),
							 Pix::Colors::GREEN);
		}





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

		if (debug) {
			canvas->fillCircle(static_cast<int32_t>(frontWheel.x), static_cast<int32_t>(frontWheel.z), 2, Pix::Colors::RED);
			canvas->fillCircle(static_cast<int32_t>(backWheel.x), static_cast<int32_t>(backWheel.z), 2, Pix::Colors::GREEN);
		}

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
		if (debug) {
			canvas->fillCircle(static_cast<int32_t>(frontWheel.x), static_cast<int32_t>(frontWheel.z), 2, Pix::Colors::BLACK);
			canvas->fillCircle(static_cast<int32_t>(backWheel.x), static_cast<int32_t>(backWheel.z), 2, Pix::Colors::GREY);
		}
		/*
		The new car position can be calculated by averaging the two new wheel positions.
		*/

		mPosition = (frontWheel + backWheel) / 2.0F;

		if (debug) {
			canvas->fillCircle(static_cast<int32_t>(mPosition.x), static_cast<int32_t>(mPosition.z), 2, Pix::Colors::BLUE);
		}

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
		const float accAmount = fAcceleration * (1.0F - FEATURES->speedPercent(modSpeed));
		mAcceleration = accAmount * head;

		if (accAmount != 0) {
			// add acceleration
			mSpeed.x += mAcceleration.x * fElapsedTime;    // Update Velocity
			mSpeed.z += mAcceleration.z * fElapsedTime;
		}
		
		if (mFlashLight != nullptr) {
			mFlashLight->position = mPosition/1000.0f;
			glm::vec3 lookdir =  mSpeed;
			lookdir.y = 0;
			lookdir = glm::rotate(lookdir, mRotation.x, {1,0,0});
			lookdir = glm::rotate(lookdir, mRotation.z, {0,0,1});
			mFlashLight->direction = glm::normalize(lookdir);
		}

	}

	void BallPlayer::steer(float perc, float fElapsedTime) {

		//	fSteerAngle = perc * M_PI / 512; // * (- log2(fabs(speedPercent()) + 0.000001));
		fSteerAngle = static_cast<float>(
				perc
				* FEATURES->maxSteerAngle()
				* (1 - 0.85 * speedPercent())
		); // * (- log2(fabs(speedPercent()) + 0.000001));

	}

	void BallPlayer::accelerate(float percentage, float fElapsedTime) {

		// MAXSPEED == speed -> 0;
		// MINSPEED -> KACCEL

		float KACCEL = 100.0F;

		bReverse = false;
		bForward = true;

		fAcceleration = KACCEL * percentage;
	}

	void BallPlayer::brake(float percentage, float fElapsedTime) {

		float KDECCEL = 200.0F * (1 - speedPercent());
		fAcceleration = -KDECCEL * percentage;

	}

	void BallPlayer::jump() {

	}

}

#pragma clang diagnostic pop

