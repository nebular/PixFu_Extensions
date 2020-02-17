//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Planet.hpp"
#include "PixFu.hpp"
#include "glm.hpp"

namespace rgl {

	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale) {

		glm::mat4 matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1, 0, 0});
		matrix = glm::rotate(matrix, ryrads, {0, 1, 0});
		matrix = glm::rotate(matrix, rzrads, {0, 0, 1});
		matrix = glm::scale(matrix, {scale, scale, scale});
		return matrix;
	}


	std::string Planet::TAG = "Planet";

	Planet::Planet(PlanetConfig_t config, Perspective_t perspective)
			: PERSPECTIVE(perspective), CONFIG(config) {

		pShader = new TerrainShader(CONFIG.shaderName);

	};

	void Planet::add(Terrain *world) {
		vTerrains.push_back(world);
	}


	bool Planet::init(PixFu *engine) {

		pLight = new Light(CONFIG.lightPosition, CONFIG.lightColor);
		pCamera = new Camera();

		pShader->use();

		// load projection matrix
		float aspectRatio = (float) engine->screenWidth() / (float) engine->screenHeight();
		projectionMatrix = glm::perspective(PERSPECTIVE.FOV, aspectRatio, PERSPECTIVE.NEAR_PLANE, PERSPECTIVE.FAR_PLANE);
		pShader->loadProjectionMatrix(projectionMatrix);
		pShader->loadLight(pLight);

		pShader->stop();

		if (DBG) LogV(TAG, SF("Init Planet, FOV %f, aspectRatio %f", PERSPECTIVE.FOV, aspectRatio));
		return true;
	}


	void Planet::tick(PixFu *engine, float fElapsedTime) {

		pShader->use();
		pShader->loadViewMatrix(pCamera);

		for (Terrain *terrain:vTerrains) {
			terrain->render(pShader);
		}

		pShader->stop();
		OpenGlUtils::glError("terrain tick");

	}

};

