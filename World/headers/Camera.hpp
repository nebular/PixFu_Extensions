//
//  World.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef World_hpp
#define World_hpp

#include "glm.hpp"
#include "ext.hpp"
#include "Keyboard.hpp"

namespace rgl {
	class Camera {

		glm::vec3 position = {0, 0, -0.02};
		float pitch = 2.5;
		float yaw = 0.0;
		float roll = 0;

		float t = 0;

	public:
		void moveWhat();

		void moveRight();

		void moveLeft();

		void moveUp();

		void move();

		void moveDown();

		glm::vec3 &getPosition();

		float getPitch();

		float getYaw();

		float getRoll();

	};


	inline void Camera::move() {

		t += 0.0001;

		float s = 0.0001;

		bool isShift = Keyboard::isHeld(Keys::SPACE);

		// eje x: left right
		if (Keyboard::isHeld(Keys::LEFT))
			position.x -= s;

		if (Keyboard::isHeld(Keys::RIGHT))
			position.x += s;

		if (Keyboard::isHeld(Keys::UP))
			if (isShift) position.z -= s; else position.y -= s;

		if (Keyboard::isHeld(Keys::DOWN))
			if (isShift) position.z += s; else position.y += s;

		if (Keyboard::isHeld(Keys::P))
			pitch += s * (isShift ? -1 : 1);

		if (Keyboard::isHeld(Keys::R))
			roll += s * 10 * (isShift ? -1 : 1);

		if (Keyboard::isHeld(Keys::Y))
			yaw += s * (isShift ? -1 : 1);



		//	pitch = t; // (random()%3140)/1000.0;
		//	yaw = (random()%3140)/1000.0;
		//	roll = (random()%3140)/1000.0;
	}


	inline void Camera::moveWhat() { position.z -= 0.2f; }

	inline void Camera::moveRight() { position.x += 0.2f; }

	inline void Camera::moveLeft() { position.x -= 0.2f; }

	inline void Camera::moveUp() { position.y += 0.2f; }

	inline void Camera::moveDown() { position.y -= 0.2f; }

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


#endif /* World_hpp */
