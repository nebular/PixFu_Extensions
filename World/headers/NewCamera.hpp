//
//  NewCamera.hpp
//  PixEngine
//
//  Created by rodo on 19/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "Keyboard.hpp"

#include <vector>
#include <cmath>

namespace rgl {

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	CM_FORWARD,
	CM_BACKWARD,
	CM_LEFT,
	CM_RIGHT,
	CM_UP,
	CM_DOWN
};


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class NewCamera
{
	static constexpr glm::vec3 DEF_UPVECTOR    = glm::vec3(0.0f, 1.0f,  0.0f);
	static constexpr glm::vec3 DEF_FRONTVECTOR = glm::vec3(0.0f, 0.0f, -1.0f);

	static constexpr float STEP = 0.0001 * 5;
	static constexpr float VSTEP = 0.1;

	// Default camera values
	static constexpr float DEF_YAW         =  0;
	static constexpr float DEF_PITCH       =  0; // M_PI;
	static constexpr float DEF_HEIGHT      =  0.2f;
	static constexpr float DEF_SPEED       =  2.5f;
	static constexpr float DEF_MOUSE_SENS  =  0.1f;
	static constexpr float DEF_ZOOM        =  45.0f;


	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	
	// Euler Angles
	float Yaw;
	float Pitch;
	float Roll;
	
	// Camera options
	float MouseSensitivity;
	float Zoom;
	
public:
	
	// Constructor with vectors
	NewCamera(
			  glm::vec3 position = glm::vec3(0.0f, -DEF_HEIGHT, 0.0f),
			  float yaw 	= DEF_YAW,
			  float pitch 	= DEF_PITCH,
			  glm::vec3 up 	= DEF_UPVECTOR
			  );
	
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 getViewMatrix();
	
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void inputMovement(Camera_Movement direction, float speed = STEP, float fElapsedTime = 1);
	
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void inputMouse(float xoffset, float yoffset, bool constrainPitch = true);
	
	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void inputMouseWheel(float yoffset);
	
	void move (float fElapsedTime);
	
	// getters
	glm::vec3 &getPosition();
	
	void setX(float xpos);
	void setZ(float ypos);
	void setHeight(float height);
	
	float getPitch();
	float getYaw();
	float getRoll();
	
	void setPitch(float ang);
	void setRoll(float ang);
	void setYaw(float ang);
	
	void stepYaw(float rads);
	void stepPitch(float rads);
	void stepRoll(float rads);
	
private:
	
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();
	
};


// main getters / setters

inline glm::vec3 &NewCamera::getPosition() 	{ return Position; }
inline float NewCamera::getPitch() 			{ return Pitch; }
inline float NewCamera::getYaw() 			{ return Yaw; }
inline float NewCamera::getRoll() 			{ return Roll; }

inline void NewCamera::setPitch(float p) 	{ Pitch = p;}
inline void NewCamera::setRoll(float r) 	{ Roll = r;}
inline void NewCamera::setYaw(float y) 		{ Yaw = y;}

inline void NewCamera::stepPitch(float rads) { Pitch += rads; }
inline void NewCamera::stepRoll(float rads) { Roll += rads; }
inline void NewCamera::stepYaw(float rads) 	{ Yaw += rads; }

inline void NewCamera::setHeight(float h)	{ Position.y = h;}

inline void NewCamera::setX(float xpos) 	{ Position.x = xpos;}
inline void NewCamera::setZ(float zpos) 	{ Position.z = zpos;}


}
