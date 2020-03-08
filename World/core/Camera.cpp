//
//  Camera.hpp
//  PixFu World Extensoin
//
//  An abstract camera class that processes input and calculates the corresponding Euler Angles,
//	Vectors and Matrices for use in OpenGL
//
//  I learnt everything from https://learnopengl.com/Getting-started/Camera
//
//  Created by rodo on 19/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Camera.hpp"
#include "World.hpp"
#include "BallPlayer.hpp"
#include "Utils.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_ovl_ambiguous_call"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "err_ovl_no_viable_member_function_in_call"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
// glm warnings in idea TODO 

namespace Pix {

	/**
	 * Constructs the camera
	 * @param configuration The configuration object
	 */

	Camera::Camera(const CameraConfig_t &configuration) {
		setConfig(configuration, false);
	}

	/**
	 *
	 * Processes input received from any keyboard-like input system. Accepts input parameter
	 * in the form of camera defined ENUM (to abstract it from windowing systems)
	 *
	 * @param direction Direction enum
	 * @param speed speed increment
	 * @param fElapsedTime time frame time
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

		fPlayerDistanceFar += (fTargetDistance - fPlayerDistanceFar) * CONFIG->lerpConfig * fElapsedTime;

		if (bTargetMode) {

			float lerp = CONFIG->lerpAngle; // 0.1;
			float diff = fabs(fTargetYaw - fYaw);

			// when this is false means 99% we are in the discontinuity
			// from -PI to PI, so in that case we skip the lerp so it
			// doesnt do a full circle bac to the negaive or positive angle.

			if (bSmooth && diff < 15.0 * M_PI / 8.0) {

				// if there is a big delta, make lerp faster
				if (diff > M_PI / 16.0) lerp = CONFIG->lerpAngle / 5;

				fYaw += (fTargetYaw - fYaw) * lerp * fElapsedTime;

			} else {
				setYaw(fTargetYaw);
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

			constexpr float KMOVER = 0.08f;
			inputMovement(CM_BACKWARD, fPlayerDistanceFar, KMOVER);
			inputMovement(CM_UP, fPlayerDistanceUp, KMOVER);

		} else if (bAnimateConfigChange) {

			// animate a config change

			const float factor = CONFIG->lerpConfig * fElapsedTime;

			mPosition.x += (CONFIG->position.x - mPosition.x) * factor;
			mPosition.y += (CONFIG->position.y - mPosition.y) * factor;
			mPosition.z += (CONFIG->position.z - mPosition.z) * factor;
			fYaw += (CONFIG->yaw - fYaw) 					  * factor;
			fPitch += (CONFIG->pitch - fPitch) 				  * factor;
			fRoll += (CONFIG->roll - fRoll) 				  * factor;
			updateCameraVectors();
		}
		
		mCurrentViewMatrix = glm::lookAt(mPosition, mPosition + mFrontVector, mUpVector);
		mCurrentInvViewMatrix = glm::inverse(mCurrentViewMatrix);
	}

	/**
	 * Processes input received from a keyboard-like input system. Expects the pressed status of
	 * each direction, plus a mode to set the arrwos meaning: move camera, adjust position,
	 * adjust pitch/yaw
	 */


	void Camera::inputKey(CameraKeyControlMode_t mode, bool up, bool down, bool left, bool right, float percent, float fElapsedTime) {

		// if we are animating, keep animating unless any direction key is pressed
		if (bAnimateConfigChange)
			bAnimateConfigChange = !(up||down||left||right);

		mCameraMode = mode;

		constexpr float KSTEP = 100;

		switch (mode) {

			case MOVE:

				// move the camera using the calculated front and back vectors
				// yaw left/right

				if (up) inputMovement(CM_FORWARD, VSTEP * percent, fElapsedTime);
				if (down) inputMovement(CM_BACKWARD, VSTEP * percent, fElapsedTime);
				if (left) stepYaw(STEP * percent);        // understand why it is negated
				if (right) stepYaw(-STEP * percent);
				break;

			case ADJUST_ANGLES:

				// looks up / down (yaw)
				// left/right also yaw

				if (left) stepYaw(STEP * percent);        // understand why it is negated
				if (right) stepYaw(-STEP * percent);
				if (up) stepPitch(-STEP * percent);
				if (down) stepPitch(STEP * percent);
				break;

			case ADJUST_POSITION:

				// adjust camera position

				if (up) inputMovement(CM_UP, VSTEP * percent, fElapsedTime);
				if (down) inputMovement(CM_DOWN, VSTEP * percent, fElapsedTime);
				if (left) inputMovement(CM_LEFT, VSTEP * percent, fElapsedTime);
				if (right) inputMovement(CM_RIGHT, VSTEP * percent, fElapsedTime);
				break;

			case ADJUST_PLAYER_ANGLES:

				// UP/DOWN adjust target mode pitch

				if (up) fPlayerPitch -= VSTEP / KSTEP;
				if (down) fPlayerPitch += VSTEP / KSTEP;
				break;

			case ADJUST_PLAYER_POSITION:

				// UP/DOWN adjust vertical distance to target
				// LEFT/RIGHT adjust horizontal distance to target

				if (left) fPlayerDistanceFar -= VSTEP / KSTEP;
				if (right) fPlayerDistanceFar += VSTEP / KSTEP;
				if (up) fPlayerDistanceUp -= VSTEP / 1000;
				if (down) fPlayerDistanceUp += VSTEP / 1000;
				if (fPlayerDistanceUp < 0) fPlayerDistanceUp = 0;

				break;
		}
	};

	/**
	 * Processes input received from a mouse input system. Expects the offset value in both
	 * the x and y direction.
	 */

	void Camera::inputMouse(float xoffset, float yoffset, bool constrainPitch) {

		bAnimateConfigChange = false;
		
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

	/**
	 * Processes input received from a mouse scroll-wheel event. Only requires input on the
	 * vertical wheel-axis
	 */

	void Camera::inputMouseWheel(float yoffset) {

		bAnimateConfigChange = false;

		if (mMouseZoom >= 1.0F && mMouseZoom <= 45.0f)
			mMouseZoom -= yoffset;
		if (mMouseZoom <= 1.0F)
			mMouseZoom = 1.0F;
		if (mMouseZoom >= 45.0F)
			mMouseZoom = 45.0F;

	}

	void Camera::updateCameraVectors() {

		// Calculate the new Front vector
		const float cosPitch = cosf(fPitch);
		const glm::vec3 front = {
				cosf(fYaw) * cosPitch,
				sinf(fPitch),
				sinf(fYaw) * cosPitch
		};

		// Re-calculate the Front, Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down
		// which results in slower movement.

		// todo this fancy fastnormalize is supposedly much less accurate but is it enough?
		mFrontVector = glm::fastNormalize(front);
		mRightVector = glm::fastNormalize(glm::cross(mFrontVector, CONFIG->upVector));
		mUpVector = glm::fastNormalize(glm::cross(mRightVector, mFrontVector));

	}
}

#pragma clang diagnostic pop
