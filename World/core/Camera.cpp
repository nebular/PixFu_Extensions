//
//  Camera.cpp
//  PixEngine
//
//  Created by rodo on 19/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Camera.hpp"
#include "World.hpp"
#include "Player.hpp"
#include "Utils.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/fast_square_root.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "err_ovl_no_viable_member_function_in_call"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
// glm warnngs in idea

namespace rgl {

	constexpr glm::vec3 Camera::DEF_UPVECTOR;
	constexpr glm::vec3 Camera::DEF_FRONTVECTOR;

	constexpr float Camera::STEP;
	constexpr float Camera::DEF_YAW;
	constexpr float Camera::DEF_PITCH;
	constexpr float Camera::DEF_HEIGHT;
	constexpr float Camera::DEF_MOUSE_SENS;
	constexpr float Camera::DEF_ZOOM;

// Constructor with vectors

	Camera::Camera(
			glm::vec3 initialPosition,
			float initialYaw,
			float initialPitch,
			glm::vec3 upVector,
			bool smooth,
			float smoothLerp,
			float distanceLerp
	) :
			mFrontVector(DEF_FRONTVECTOR),
			mMouseSensitivity(DEF_MOUSE_SENS),
			mMouseZoom(DEF_ZOOM),
			UPVECTOR(upVector),
			bSmooth(smooth),
			SMOOTHLERP(smoothLerp),
			DISTANCELERP(distanceLerp) {
		mPosition = mInterpolatedPosition = initialPosition;
		fYaw = initialYaw;
		fRoll = 0;
		fPitch = initialPitch;
		updateCameraVectors();
	}

	/**
	 * Processes input received from any keyboard-like input system. Accepts input parameter
	 * in the form of camera defined ENUM (to abstract it from windowing systems)
	 * @param direction Direction enum
	 * @param speed speed
	 * @param fElapsedTime time
	 */

	void Camera::inputMovement(CameraMovement_t direction, float speed, float fElapsedTime) {

		GLfloat velocity = speed * fElapsedTime;
		switch (direction) {
			case CM_FORWARD:
				mPosition += mFrontVector * velocity;
				break;
			case CM_BACKWARD:
				mPosition -= mFrontVector * velocity;
				break;
			case CM_LEFT:
				mPosition -= mRightVector * velocity;
				break;
			case CM_RIGHT:
				mPosition += mRightVector * velocity;
				break;
			case CM_UP:
				mPosition += mUpVector * velocity;
				break;
			case CM_DOWN:
				mPosition -= mUpVector * velocity;
				break;
		}
	}

	/**
	 * Updates the camera. called by the engine.
	 * @param fElapsedTime frame time
	 */

	void Camera::update(float fElapsedTime) {

		fPlayerDistanceFar += (fTargetDistance - fPlayerDistanceFar) * DISTANCELERP * fElapsedTime;

		if (bTargetMode) {

			float lerp = 15; // 0.1;
			float diff =fabs(fTargetAngle - fYaw);

			if (bSmooth && diff < 15*M_PI / 8) {
				
				if (diff > M_PI / 32) lerp=2;

				fYaw += (fTargetAngle - fYaw) * lerp * fElapsedTime;

			} else {
				setYaw(fTargetAngle);
			}

			setPitch(fPlayerPitch);
			updateCameraVectors();

			if (bSmooth) {
				mInterpolatedPosition.x += (mTargetPosition.x - mInterpolatedPosition.x) * lerp * fElapsedTime;
				mInterpolatedPosition.y = mTargetPosition.y;

//				mInterpolatedPosition.y += (mTargetPosition.y - mInterpolatedPosition.y) * lerp * fElapsedTime;
				mInterpolatedPosition.z += (mTargetPosition.z - mInterpolatedPosition.z) * lerp * fElapsedTime;
				mPosition = mInterpolatedPosition;
			} else {
				mPosition = mTargetPosition;
			}

			inputMovement(CM_BACKWARD, fPlayerDistanceFar, 0.08);
			inputMovement(CM_UP, fPlayerDistanceUp, 0.08);
		}

	}

// Processes input received from a keyboard-like input system. Expects the pressed status of
// each direction, plus a mode to set the arrwos meaning: move camera, adjust position,
// adjust pitch/yaw

	void Camera::inputKey(CameraKeyControlMode_t mode, bool up, bool down, bool left, bool right, float fElapsedTime) {

		mCameraMode = mode;

		switch (mode) {
			case MOVE:
				if (up) inputMovement(CM_FORWARD, VSTEP, fElapsedTime);
				if (down) inputMovement(CM_BACKWARD, VSTEP, fElapsedTime);
				if (left) stepYaw(-STEP * 5);
				if (right) stepYaw(STEP * 5);
				break;
			case ADJUST_ANGLES:
				if (left) stepYaw(-STEP * 5);
				if (right) stepYaw(STEP * 5);
				if (up) stepPitch(-STEP * 2);
				if (down) stepPitch(STEP * 2);
				break;
			case ADJUST_POSITION:
				if (up) inputMovement(CM_UP, VSTEP, fElapsedTime);
				if (down) inputMovement(CM_DOWN, VSTEP, fElapsedTime);
				if (left) inputMovement(CM_LEFT, VSTEP, fElapsedTime);
				if (right) inputMovement(CM_RIGHT, VSTEP, fElapsedTime);
				break;
			case ADJUST_PLAYER_ANGLES:
				if (up) fPlayerPitch -= VSTEP / 100;
				if (down) fPlayerPitch += VSTEP / 100;
//				LogV("cam", SF("fPlayerPitch %f", fPlayerPitch));
				break;
			case ADJUST_PLAYER_POSITION:
				if (left) fPlayerDistanceFar -= VSTEP / 100;
				if (right) fPlayerDistanceFar += VSTEP / 100;
				if (up) fPlayerDistanceUp -= VSTEP / 1000;
				if (down) fPlayerDistanceUp += VSTEP / 1000;
				if (fPlayerDistanceUp < 0) fPlayerDistanceUp = 0;
//				LogV("cam", SF("fPlayerDistanceFar %f fPlayerDistanceUp  %f", fPlayerDistanceFar, fPlayerDistanceUp));

				break;

		}

	};

// Processes input received from a mouse input system. Expects the offset value in both
// the x and y direction.

	void Camera::inputMouse(float xoffset, float yoffset, bool constrainPitch) {

		xoffset *= mMouseSensitivity;
		yoffset *= mMouseSensitivity;

		fYaw += xoffset;
		fPitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch) {
			if (fPitch > M_PI)
				fPitch = static_cast<float>(M_PI);
			if (fPitch < -M_PI)
				fPitch = static_cast<float>(-M_PI);
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

// Processes input received from a mouse scroll-wheel event. Only requires input on the
// vertical wheel-axis

	void Camera::inputMouseWheel(float yoffset) {
		if (mMouseZoom >= 1.0f && mMouseZoom <= 45.0f)
			mMouseZoom -= yoffset;
		if (mMouseZoom <= 1.0f)
			mMouseZoom = 1.0f;
		if (mMouseZoom >= 45.0f)
			mMouseZoom = 45.0f;
	}

	void Camera::updateCameraVectors() {
		// Calculate the new Front vector
		glm::vec3 front = {
				cosf(fYaw) * cosf(fPitch),
				sinf(fPitch),
				sinf(fYaw) * cosf(fPitch)
		};

		// Re-calculate the Front, Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down
		// which results in slower movement.

		// todo this fancy fastnormalize is supposedly much less accurate but is it enough?
		mFrontVector = glm::fastNormalize(front);
		mRightVector = glm::fastNormalize(glm::cross(mFrontVector, UPVECTOR));
		mUpVector = glm::fastNormalize(glm::cross(mRightVector, mFrontVector));

	}


}

#pragma clang diagnostic pop
