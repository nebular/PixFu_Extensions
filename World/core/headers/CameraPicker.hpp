//
//  MousePicker.hpp
//  PixFu
//
//  Created by rodo on 07/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include "glm/matrix.hpp"

#include "Camera.hpp"
#include "Mouse.hpp"
#include "World.hpp"
#include "FuExtension.hpp"

namespace Pix {

	class CameraPicker:public FuExtension {

		static constexpr int 	RECURSION_COUNT = 200;
		static constexpr float 	RAY_RANGE = 3000;

		float fLastMouseX, fLastMouseY;

		glm::vec3 currentRay;
		glm::vec3 currentTerrainPoint;
		bool terrainPointValid;
		
		glm::mat4 invertedProjectionMatrix;
		Pix::World *pWorld;

		glm::vec3 calculateMouseRay(float mouseXndc, float mouseYndc);

		glm::vec3 toWorldCoords(glm::vec4& eyeCoords);

		glm::vec4 toEyeCoords(glm::vec4& clipCoords);

		//**********************************************************

		glm::vec3 getPointOnRay(glm::vec3& ray, float distance);

		bool binarySearch(glm::vec3 &result, int count, float start, float finish, glm::vec3 ray);
		bool intersectionInRange(float start, float finish, glm::vec3& ray);
		bool isUnderGround(glm::vec3& testPoint);

		public:
			CameraPicker(World *world);
			glm::vec3 *getCurrentTerrainPoint();
			glm::vec3& getCurrentRay();
			bool init(Fu *engine) override;
			void tick(Fu *engine, float fElapsedTime) override;
	};
}
