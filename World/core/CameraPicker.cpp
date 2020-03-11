//
//  MousePicker.cpp
//  PixFu Engine
//
//  A 3D mouse implementation
//
//  Tutorial and explanations in:
//  http://antongerdelan.net/opengl/raycasting.html
//
//  Created by rodo on 07/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "CameraPicker.hpp"
#include "glm/gtx/fast_square_root.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "err_ovl_ambiguous_call"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
namespace Pix {
	
	CameraPicker *CameraPicker::pInstance = nullptr;

	CameraPicker::CameraPicker(World *world) {
		pWorld = world;
	}

	glm::vec3 *CameraPicker::getCurrentTerrainPoint() {
		return terrainPointValid ? &currentTerrainPoint : nullptr;
	}

	glm::vec3& CameraPicker::getCurrentRay() {
		return currentRay;
	}

	bool CameraPicker::init (Fu *engine) {
		invertedProjectionMatrix = glm::inverse(pWorld->getProjectionMatrix());
		return true;
	}

	void CameraPicker::tick(Fu *engine, float fElapsedTime) {
		
		float mx = Mouse::xNdc();
		float my = Mouse::yNdc();

		// skip calculations if coordinates do not change
		if (mx != fLastMouseX || my != fLastMouseY) {

			fLastMouseY = my;
			fLastMouseX = mx;

			currentRay = calculateMouseRay(Mouse::xNdc(), Mouse::yNdc());
			if (intersectionInRange(0, RAY_RANGE, currentRay)) {
				terrainPointValid = binarySearch(currentTerrainPoint, 0, 0, RAY_RANGE, currentRay);
			} else {
				terrainPointValid = false;
			}
		}
	}

	glm::vec3 CameraPicker::calculateMouseRay(float mouseXndc, float mouseYndc) {
		glm::vec4 clipCoords = { mouseXndc, mouseYndc, -1.0F, 1.0F };
		glm::vec4 eyeCoords = toEyeCoords(clipCoords);
		glm::vec3 worldRay = toWorldCoords(eyeCoords);
		return worldRay;
	}

	glm::vec3 CameraPicker::toWorldCoords(glm::vec4& eyeCoords) {
		glm::mat4& invertedView = pWorld->camera()->getInvViewMatrix(); // glm::inverse(viewMatrix);
		glm::vec4 rayWorld = invertedView * eyeCoords;
		glm::vec3 mouseRay = {rayWorld.x, rayWorld.y, rayWorld.z};
		return glm::fastNormalize(mouseRay);
	}

	glm::vec4 CameraPicker::toEyeCoords(glm::vec4& clipCoords) {
		glm::vec4 eyeCoords = invertedProjectionMatrix * clipCoords;
		return {eyeCoords.x, eyeCoords.y, 1.0F, 0.0F};
//		return {eyeCoords.x, eyeCoords.y, -1.0F, 0.0F};
	}

	//**********************************************************

	glm::vec3 CameraPicker::getPointOnRay(glm::vec3& ray, float distance) {
		glm::vec3 camPos = pWorld->camera()->getPosition();
		return { camPos.x + ray.x * distance, camPos.y + ray.y * distance, camPos.z + ray.z * distance };
	}

	bool CameraPicker::binarySearch(glm::vec3& result, int count, float start, float finish, glm::vec3 ray) {

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

	bool CameraPicker::intersectionInRange(float start, float finish, glm::vec3& ray) {
		glm::vec3 startPoint = getPointOnRay(ray, start);
		glm::vec3 endPoint = getPointOnRay(ray, finish);
		return (!isUnderGround(startPoint) && isUnderGround(endPoint));
	}

	bool CameraPicker::isUnderGround(glm::vec3& testPoint) {
		float height = 0;
		height = pWorld->getHeight(testPoint);
		return testPoint.y < height;
	}
};


#pragma clang diagnostic pop
