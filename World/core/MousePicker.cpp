//
//  MousePicker.cpp
//  PixFu
//
//  http://antongerdelan.net/opengl/raycasting.html
//
//  Created by rodo on 07/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "MousePicker.hpp"
#include "glm/gtx/fast_square_root.hpp"

namespace Pix {

	MousePicker::MousePicker(World *world, glm::mat4 &projection) {
		invertedProjectionMatrix = glm::inverse(projection);
		pWorld = world;
	}

	glm::vec3 *MousePicker::getCurrentTerrainPoint() {
		return terrainPointValid ? &currentTerrainPoint : nullptr;
	}

	glm::vec3& MousePicker::getCurrentRay() {
		return currentRay;
	}

	void MousePicker::update() {
		currentRay = calculateMouseRay(Mouse::xNdc(), Mouse::yNdc());
		if (intersectionInRange(0, RAY_RANGE, currentRay)) {
			terrainPointValid=binarySearch(currentTerrainPoint, 0, 0, RAY_RANGE, currentRay);
		} else {
			terrainPointValid = false;
		}
		if (Mouse::isPressed(0)) {
			pWorld->select(currentRay);
		}
	}

	glm::vec3 MousePicker::calculateMouseRay(float mouseXndc, float mouseYndc) {
		glm::vec4 clipCoords = { mouseXndc, mouseYndc, -1.0f, 1.0f };
		glm::vec4 eyeCoords = toEyeCoords(clipCoords);
		glm::vec3 worldRay = toWorldCoords(eyeCoords);
		return worldRay;
	}

	glm::vec3 MousePicker::toWorldCoords(glm::vec4& eyeCoords) {
		glm::mat4& invertedView = pWorld->camera()->getInvViewMatrix(); // glm::inverse(viewMatrix);
		glm::vec4 rayWorld = invertedView * eyeCoords;
		glm::vec3 mouseRay = {rayWorld.x, rayWorld.y, rayWorld.z};
		return glm::fastNormalize(mouseRay);
	}

	glm::vec4 MousePicker::toEyeCoords(glm::vec4& clipCoords) {
		glm::vec4 eyeCoords = invertedProjectionMatrix * clipCoords;
		return {eyeCoords.x, eyeCoords.y, 1.0F, 0.0F};
//		return {eyeCoords.x, eyeCoords.y, -1.0F, 0.0F};
	}

	//**********************************************************

	glm::vec3 MousePicker::getPointOnRay(glm::vec3& ray, float distance) {
		glm::vec3 camPos = pWorld->camera()->getPosition();
		return { camPos.x + ray.x * distance, camPos.y + ray.y * distance, camPos.z + ray.z * distance };
	}

	bool MousePicker::binarySearch(glm::vec3& result, int count, float start, float finish, glm::vec3 ray) {

		float half = start + ((finish - start) / 2.0F);
		if (count >= RECURSION_COUNT) {
			result = getPointOnRay(ray, half);
			return pWorld->hasTerrain(result);
		}
	
		if (intersectionInRange(start, half, ray)) {
			return binarySearch(result, count + 1, start, half, ray);
		} else {
			return binarySearch(result, count + 1, half, finish, ray);
		}
	}

	bool MousePicker::intersectionInRange(float start, float finish, glm::vec3& ray) {
		glm::vec3 startPoint = getPointOnRay(ray, start);
		glm::vec3 endPoint = getPointOnRay(ray, finish);
		if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
			return true;
		} else {
			return false;
		}
	}

	bool MousePicker::isUnderGround(glm::vec3& testPoint) {
		float height = 0;
		height = pWorld->getHeight(testPoint);
		if (testPoint.y < height) {
			return true;
		} else {
			return false;
		}
	}
};

