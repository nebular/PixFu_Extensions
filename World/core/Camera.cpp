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
			   glm::vec3 position,
			   float yaw,
			   float pitch,
			   glm::vec3 up
			   ) :
mFrontVector(DEF_FRONTVECTOR),
mMouseSensitivity(DEF_MOUSE_SENS),
mMouseZoom(DEF_ZOOM),
UPVECTOR(up) {
	mPosition = interPosition = position;
	fYaw = yaw;
	fRoll = 0;
	fPitch = pitch;
	updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(mPosition, mPosition + mFrontVector, mUpVector);
}

// Processes input received from any keyboard-like input system. Accepts input parameter
// in the form of camera defined ENUM (to abstract it from windowing systems)

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

void Camera::follow(Player *player, float fElapsedTime) {
	targetPosition = player->pos();
	targetAngle = -M_PI/2 - player->angle();
}

void Camera::setTargetMode(bool enable) { targetMode = enable; }

void Camera::update(float fElapsedTime) {

	if (targetMode) {
		float lerp  = 15; // 0.1;

		if (smooth) {
			fYaw += (targetAngle - fYaw) * lerp * fElapsedTime;
		} else {
			setYaw(targetAngle);
		}

		setPitch(fPlayerPitch);
		updateCameraVectors();
		
		if (smooth) {
			interPosition.x += (targetPosition.x - interPosition.x) * lerp * fElapsedTime;
			interPosition.y += (targetPosition.y - interPosition.y) * lerp * fElapsedTime;
			interPosition.z += (targetPosition.z - interPosition.z) * lerp * fElapsedTime;
			mPosition = interPosition;
		} else {
			mPosition = targetPosition;
		}

		inputMovement(CM_BACKWARD, fPlayerDistanceFar, 0.08);
		inputMovement(CM_UP, fPlayerDistanceUp, 0.08);
	}

}

// Processes input received from a keyboard-like input system. Expects the pressed status of
// each direction, plus a mode to set the arrwos meaning: move camera, adjust position,
// adjust pitch/yaw

void Camera::inputKey(World *world, CameraKeyControlMode_t mode, bool up, bool down, bool left, bool right, float fElapsedTime) {
	
	mCameraMode = mode;
	
	switch (mode) {
		case MOVE:
//			mPosition.y = world->getHeight(mPosition) + 0.01;
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
			if (up) fPlayerPitch -= VSTEP/100;
			if (down) fPlayerPitch += VSTEP/100;
//			LogV("cam", SF("fPlayerPitch %f", fPlayerPitch));
			break;
		case ADJUST_PLAYER_POSITION:
			if (left) fPlayerDistanceFar -= VSTEP/100;
			if (right) fPlayerDistanceFar += VSTEP/100;
			if (up) fPlayerDistanceUp -= VSTEP/1000;
			if (down) fPlayerDistanceUp += VSTEP/1000;
			if (fPlayerDistanceUp<0) fPlayerDistanceUp = 0;
//			LogV("cam", SF("fPlayerDistanceFar %f fPlayerDistanceUp  %f", fPlayerDistanceFar, fPlayerDistanceUp));

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