//
//  NewCamera.cpp
//  PixEngine
//
//  Created by rodo on 19/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "NewCamera.hpp"

namespace rgl {

constexpr glm::vec3 NewCamera::DEF_UPVECTOR;
constexpr glm::vec3 NewCamera::DEF_FRONTVECTOR;
constexpr float NewCamera::STEP;
constexpr float NewCamera::VSTEP;
constexpr float NewCamera::DEF_YAW;
constexpr float NewCamera::DEF_PITCH;
constexpr float NewCamera::DEF_HEIGHT;
constexpr float NewCamera::DEF_SPEED;
constexpr float NewCamera::DEF_MOUSE_SENS;
constexpr float NewCamera::DEF_ZOOM;

// Constructor with vectors
NewCamera::NewCamera(
					 glm::vec3 position,
					 float yaw,
					 float pitch,
					 glm::vec3 up
					 ) :
Front(DEF_FRONTVECTOR),
MouseSensitivity(DEF_MOUSE_SENS),
Zoom(DEF_ZOOM) {
		Position = position;
	WorldUp = up;
	Yaw = yaw;
	Roll = 0;
	Pitch = pitch;
	updateCameraVectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 NewCamera::getViewMatrix() {
	return glm::lookAt(Position, Position + Front, Up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void NewCamera::inputMovement(Camera_Movement direction, float speed, float fElapsedTime ) {
	float velocity = speed * fElapsedTime;
	switch (direction) {
		case CM_FORWARD: 	Position += Front * velocity; break;
		case CM_BACKWARD: 	Position -= Front * velocity; break;
		case CM_LEFT: 		Position -= Right * velocity; break;
		case CM_RIGHT: 		Position += Right * velocity; break;
		case CM_UP: 		Position += Up * velocity; break;
		case CM_DOWN:	 	Position -= Up * velocity; break;
	}
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void NewCamera::inputMouse(float xoffset, float yoffset, bool constrainPitch) {
	
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;
	
	Yaw   += xoffset;
	Pitch += yoffset;
	
	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch) {
		if (Pitch > M_PI)
			Pitch = M_PI;
		if (Pitch < -M_PI)
			Pitch = -M_PI;
	}
	
	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void NewCamera::inputMouseWheel(float yoffset) {
	if (Zoom >= 1.0f && Zoom <= 45.0f)
		Zoom -= yoffset;
	if (Zoom <= 1.0f)
		Zoom = 1.0f;
	if (Zoom >= 45.0f)
		Zoom = 45.0f;
}

void NewCamera::updateCameraVectors() {
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cosf(Yaw) * cosf(Pitch); // ojo
	front.y = sinf(Pitch);
	front.z = sinf(Yaw) * cosf(Pitch); // ojo
	
	Front = glm::normalize(front);
	
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up    = glm::normalize(glm::cross(Right, Front));
}

void NewCamera::move(float fElapsedTime) {
	
	if (Keyboard::isHeld(Keys::ALT)) {
		
		if (Keyboard::isHeld(Keys::UP)) 	stepPitch(-STEP*2);
		if (Keyboard::isHeld(Keys::DOWN)) 	stepPitch(STEP*2);
		if (Keyboard::isHeld(Keys::LEFT)) 	stepYaw(STEP*5);
		if (Keyboard::isHeld(Keys::RIGHT)) 	stepYaw(-STEP*5);
		updateCameraVectors();
		
	} else if (Keyboard::isHeld(Keys::COMMAND)) {

		if (Keyboard::isHeld(Keys::UP)) 	Position.y-=VSTEP * fElapsedTime;
		if (Keyboard::isHeld(Keys::DOWN)) 	Position.y+=VSTEP * fElapsedTime;

	} else {
		// walk mode
		if (Keyboard::isHeld(Keys::UP)) 	inputMovement(CM_FORWARD, VSTEP, fElapsedTime);
		if (Keyboard::isHeld(Keys::DOWN)) 	inputMovement(CM_BACKWARD, VSTEP, fElapsedTime);
		if (Keyboard::isHeld(Keys::LEFT)) 	inputMovement(CM_LEFT, VSTEP, fElapsedTime);
		if (Keyboard::isHeld(Keys::RIGHT)) 	inputMovement(CM_RIGHT, VSTEP, fElapsedTime);
	}
};

}
