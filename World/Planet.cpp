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

	constexpr Perspective_t Planet::PERSP_FOV90_LOW;
	constexpr Perspective_t Planet::PERSP_FOV90_MID;
	constexpr Perspective_t Planet::PERSP_FOV90_FAR;

	std::string Planet::TAG = "Planet";

	Planet::Planet(PlanetConfig_t config, Perspective_t perspective)
			: PERSPECTIVE(perspective), CONFIG(config) {

		pShader = new TerrainShader(CONFIG.shaderName);

	};

	 Planet::~Planet() {

		if (DBG) LogE(TAG, "Destroying planet");
		for (Terrain *terrain : vTerrains) {
			delete terrain;
		}
		vTerrains.clear();
	}

	void Planet::add(TerrainConfig_t terrainConfig) {
		Terrain *world = new Terrain(CONFIG, terrainConfig);
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

		pCamera->move();
		
		for (Terrain *terrain:vTerrains) {
			terrain->render(pShader);
		}

		pShader->stop();
		OpenGlUtils::glError("terrain tick");

	}

};

