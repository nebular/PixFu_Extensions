//
//  World.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//
#pragma once

#include <cmath>
#include "glm.hpp"
#include "Keyboard.hpp"

namespace rgl {

	class Camera {

		static float constexpr DEFAULT_HEIGHT = 0.2;
		static float constexpr STEP = 0.0001 * 5;
		static float constexpr VSTEP = 1;

		glm::vec3 position = {0, -DEFAULT_HEIGHT, 0};

		float pitch = M_PI;
		float yaw = 0.0;
		float roll = 0;

	public:

		// absolute camera movement
		
		void lookForward(float step=STEP);
		void lookBackward(float step=STEP);
		void lookRight(float step=STEP);
		void lookLeft(float step=STEP);
		void lookUp(float step=STEP);
		void lookDown(float step=STEP);

		void moveForward(float step=STEP);
		void moveBackward(float step=STEP);

		void setX(float xpos);
		void setZ(float ypos);
		void setHeight(float height);

		// camera rotation
		
		void stepYaw(float rads);
		void stepPitch(float rads);
		void stepRoll(float rads);

		void setPitch(float ang);
		void setRoll(float ang);
		void setYaw(float ang);

		void move();

		glm::vec3 &getPosition();

		float getPitch();

		float getYaw();

		float getRoll();
		

	};

	inline void Camera::setX(float xpos) 	{ position.x = xpos;}
	inline void Camera::setZ(float zpos) 	{ position.z = zpos;}
	inline void Camera::setHeight(float h)	{ position.y = -h;}
	inline void Camera::setPitch(float p) 	{ pitch = p;}
	inline void Camera::setRoll(float r) 	{ roll = r;}
	inline void Camera::setYaw(float y) 	{ yaw = y;}
	inline void Camera::move() {

		bool isShift = Keyboard::isHeld(Keys::SPACE);

		if (Keyboard::isHeld(Keys::ALT)) {
			if (Keyboard::isHeld(Keys::UP)) 	stepPitch(-STEP*2);
			if (Keyboard::isHeld(Keys::DOWN)) 	stepPitch(STEP*2);
			if (Keyboard::isHeld(Keys::LEFT)) 	stepYaw(STEP*5);
			if (Keyboard::isHeld(Keys::RIGHT)) 	stepYaw(-STEP*5);
		} else if (Keyboard::isHeld(Keys::COMMAND)) {
			// walk mode
			if (Keyboard::isHeld(Keys::UP)) 	moveForward();
			if (Keyboard::isHeld(Keys::DOWN)) 	moveBackward();
			if (Keyboard::isHeld(Keys::LEFT)) 	stepYaw(STEP*5);
			if (Keyboard::isHeld(Keys::RIGHT)) 	stepYaw(-STEP*5);
		} else {

			// eje x: left right
			if (Keyboard::isHeld(Keys::LEFT))
				lookLeft();

			if (Keyboard::isHeld(Keys::RIGHT))
				lookRight();

			if (Keyboard::isHeld(Keys::UP)) {
				if (isShift) lookUp();
				else lookForward();
			}

			if (Keyboard::isHeld(Keys::DOWN)) {
				if (isShift) lookDown();
				else lookBackward();
			}
		}
	}
	inline void Camera::moveForward(float step) {
		
		position.x += step * sinf(yaw);
		position.z += step * cosf(yaw);
		
	}
	inline void Camera::moveBackward(float step) {
		moveForward(-step);
	}
	inline void Camera::lookForward(float step) { position.z += step; }
	inline void Camera::lookBackward(float step) { position.z -= step; }
	inline void Camera::lookRight(float step) { position.x += step; }
	inline void Camera::lookLeft(float step) { position.x -= step; }
	inline void Camera::lookUp(float step) { position.y -= step; }
	inline void Camera::lookDown(float step) { position.y += step; }
	inline void Camera::stepPitch(float rads) { pitch += rads; }
	inline void Camera::stepRoll(float rads) { roll += rads; }
	inline void Camera::stepYaw(float rads) { yaw += rads; }
	inline glm::vec3 &Camera::getPosition() { return position; }

	inline float Camera::getPitch() { return pitch; }
	inline float Camera::getYaw() { return yaw; }
	inline float Camera::getRoll() { return roll; }

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

	inline glm::vec3 &Light::position() {
		return mPosition;
	}

	inline void Light::setPosition(glm::vec3 position) {
		mPosition = position;
	}

	inline glm::vec3 Light::getColour() {
		return mColour;
	}

	inline void Light::setColour(glm::vec3 colour) {
		mColour = colour;
	}

}

