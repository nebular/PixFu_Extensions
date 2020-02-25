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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "glm.hpp"
#include "matrix.hpp"
#include "Keyboard.hpp"

#include <vector>
#include <cmath>

namespace rgl {

// Defines several possible options for camera movement. Used as abstraction to stay away from
// window-system specific input methods

typedef enum CameraMovement {
	CM_FORWARD,
	CM_BACKWARD,
	CM_LEFT,
	CM_RIGHT,
	CM_UP,
	CM_DOWN
} CameraMovement_t;

typedef enum sCameraKeyControlMode {
	MOVE, ADJUST_POSITION, ADJUST_ANGLES, ADJUST_PLAYER_POSITION, ADJUST_PLAYER_ANGLES
} CameraKeyControlMode_t;

// An abstract camera class that processes input and calculates the corresponding Euler Angles,
// Vectors and Matrices for use in OpenGL

class World;
class Player;
class Camera {
	
	static constexpr glm::vec3 DEF_UPVECTOR = glm::vec3(0.0f, 1.0f, 0.0f);
	static constexpr glm::vec3 DEF_FRONTVECTOR = glm::vec3(0.0f, 0.0f, -1.0f);
	
	static constexpr float STEP = 0.0001f * 15.0f, VSTEP = 0.05;
	
	// Default camera values
	static constexpr float DEF_YAW = 0;
	static constexpr float DEF_PITCH = 0; // M_PI;
	static constexpr float DEF_HEIGHT = 0.2f;
	static constexpr float DEF_MOUSE_SENS = 0.1f;
	static constexpr float DEF_ZOOM = 45.0f;
	
	const glm::vec3 UPVECTOR;
	
	// Camera Attributes
	glm::vec3 mPosition;
	glm::vec3 mFrontVector;
	glm::vec3 mUpVector;
	glm::vec3 mRightVector;
	
	// Euler Angles
	float fYaw;
	float fPitch;
	float fRoll;
	
	// target mode
	bool smooth = true;
	bool targetMode = false;
	float targetDistance;
	float targetAngle;
	glm::vec3 targetPosition, interPosition;
	
	// Camera options
	float mMouseSensitivity;
	float mMouseZoom;
	
	int mCameraMode;

	// camera player focus
	float fPlayerDistanceUp = 0.1;
	float fPlayerDistanceFar = 0.3; // 0.05;
	float fPlayerPitch = -0.05;
	
public:
	
	// Constructor with vectors
	Camera(
		   glm::vec3 position = glm::vec3(0.0f, DEF_HEIGHT, 0.0f),
		   float yaw = DEF_YAW,
		   float pitch = DEF_PITCH,
		   glm::vec3 up = DEF_UPVECTOR
		   );
	
	void update(float fElapsedTime);
	
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 getViewMatrix();
	
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void inputMouse(float xoffset, float yoffset, bool constrainPitch = true);
	
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void inputMouseWheel(float yoffset);
	
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void inputMovement(CameraMovement_t direction, float speed = STEP, float fElapsedTime = 1);
	
	// Processes input received from a keyboard-like input system. Expects the pressed status of
	// each direction, plus a mode to set the arrwos meaning: move camera, adjust position,
	// adjust pitch/yaw

	void inputKey(World *world, CameraKeyControlMode_t mode, bool up, bool down, bool left, bool right, float fElapsedTime);
	
	void stepYaw(float deltaRads);
	
	void stepPitch(float deltaRads);
	
	void stepRoll(float deltaRads);
	
	// getters & getters
	
	glm::vec3 &getPosition();
	
	void setX(float xpos);
	
	void setZ(float ypos);
	
	void setHeight(float height);
	
	float getPitch();
	
	float getYaw();
	
	float getRoll();
	
	void setPitch(float rads);
	
	void setRoll(float rads);
	
	void setYaw(float rads);
	
	void follow(Player *player, float fElapsedTime);
	
	void setTargetMode(bool enable);
	
private:
	
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();
	
};


// main getters / setters

inline glm::vec3 &Camera::getPosition() { return mPosition; }

inline float Camera::getPitch() { return fPitch; }

inline float Camera::getYaw() { return fYaw; }

inline float Camera::getRoll() { return fRoll; }

inline void Camera::setPitch(float rads) { fPitch = rads; }

inline void Camera::setRoll(float rads) { fRoll = rads; }

inline void Camera::setYaw(float rads) { fYaw = rads; }

inline void Camera::setHeight(float h) { mPosition.y = h; }

inline void Camera::setX(float xpos) { mPosition.x = xpos; }

inline void Camera::setZ(float zpos) { mPosition.z = zpos; }

inline void Camera::stepPitch(float deltaRads) { fPitch += deltaRads; updateCameraVectors(); }

inline void Camera::stepRoll(float deltaRads) { fRoll += deltaRads; updateCameraVectors(); }

inline void Camera::stepYaw(float deltaRads) { fYaw += deltaRads; updateCameraVectors(); }

class Light {
	
	glm::vec3 mPosition;
	glm::vec3 mColour;
	
public:
	
	Light(glm::vec3 position, glm::vec3 colour);
	
	glm::vec3 &position();
	
	void setPosition(glm::vec3 position);
	
	glm::vec3 getColour();
	
	void setColour(glm::vec3 colour);
	
	
};

inline Light::Light(glm::vec3 position, glm::vec3 colour) {
	mPosition = position;
	mColour = colour;
}

inline glm::vec3 &Light::position() { return mPosition; }

inline void Light::setPosition(glm::vec3 position) { mPosition = position; }

inline glm::vec3 Light::getColour() { return mColour; }

inline void Light::setColour(glm::vec3 colour) { mColour = colour; }

}

#pragma clang diagnostic pop