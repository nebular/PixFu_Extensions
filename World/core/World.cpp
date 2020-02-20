//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"
#include "PixFu.hpp"
#include "OpenGL.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace rgl {

	constexpr Perspective_t World::PERSP_FOV90_LOW;
	constexpr Perspective_t World::PERSP_FOV90_MID;
	constexpr Perspective_t World::PERSP_FOV90_FAR;
	constexpr Perspective_t World::PERSP_FOV60_LOW;
	constexpr Perspective_t World::PERSP_FOV60_MID;
	constexpr Perspective_t World::PERSP_FOV60_FAR;

	std::string World::TAG = "World";

	World::World(WorldConfig_t config, Perspective_t perspective)
			: PERSPECTIVE(perspective), CONFIG(config) {

		pShader = new TerrainShader(CONFIG.shaderName);
		pShaderObjects = new ObjectShader(CONFIG.shaderName + "_objects");

	};

	World::~World() {

		if (DBG) LogE(TAG, "Destroying planet");
		for (Terrain *terrain : vTerrains) {
			delete terrain;
		}
		vTerrains.clear();
	}

	void World::add(TerrainConfig_t terrainConfig) {
		Terrain *world = new Terrain(CONFIG, terrainConfig);
		vTerrains.push_back(world);
	}

	void World::add(const char *name, ObjectConfig_t objectConfig, ObjectConfig_t initialTransform) {

		auto clusterItem = mCluesters.find(name);
		ObjectCluster *cluster;

		if (clusterItem == mCluesters.end()) {
			// create object cluster
			cluster = new ObjectCluster(name, CONFIG, initialTransform);
			vObjects.push_back(cluster);
			mCluesters[name] = cluster;
		} else {
			cluster = clusterItem->second;
		}

		cluster->add(objectConfig);
	}

	bool World::init(PixFu *engine) {

		pLight = new Light(CONFIG.lightPosition, CONFIG.lightColor);
		pCamera = new Camera();

		pCamera->setHeight(PERSPECTIVE.C_HEIGHT);
		pCamera->setPitch(PERSPECTIVE.C_PITCH);

		pShader->use();
		pShader->bindAttributes();

		// load projection matrix
		float aspectRatio = (float) engine->screenWidth() / (float) engine->screenHeight();
		projectionMatrix = glm::perspective(PERSPECTIVE.FOV, aspectRatio, PERSPECTIVE.NEAR_PLANE, PERSPECTIVE.FAR_PLANE);

		pShader->loadProjectionMatrix(projectionMatrix);
		//		pShader->loadLight(pLight);

		pShader->stop();

		if (pShaderObjects != nullptr) {

			for (ObjectCluster *object:vObjects) {
				object->init();
			}

			pShaderObjects->use();
			pShaderObjects->bindAttributes();
			pShaderObjects->loadProjectionMatrix(projectionMatrix);
			//			pShaderObjects->loadLight(pLight);
			pShaderObjects->stop();
		}

//	pGrid->build();

		if (DBG)
			LogV(TAG, SF("Init World, FOV %f, aspectRatio %f",
						 PERSPECTIVE.FOV, aspectRatio));

		return true;
	}

	float World::getHeight(glm::vec2 posWorld) {

		if (vTerrains.size()==1)
			return vTerrains[0]->getHeight(posWorld);

		for (Terrain *terrain:vTerrains) {
			if (terrain->contains(posWorld))
				return terrain->getHeight(posWorld);
		}
	}

	void World::tick(PixFu *engine, float fElapsedTime) {

		glClearColor(CONFIG.backgroundColor.x, CONFIG.backgroundColor.y, CONFIG.backgroundColor.z, 1.0);
		glEnable(GL_DEPTH_TEST);

		pShader->use();
		pShader->loadViewMatrix(pCamera);
		pShader->loadLight(pLight); //

		CameraKeyControlMode_t mode =
			Keyboard::isHeld(Keys::ALT) ? rgl::ADJUST_ANGLES :
			Keyboard::isHeld(Keys::COMMAND) ? rgl::ADJUST_POSITION : rgl::MOVE;
		
		pCamera->inputKey(
					  mode,
					  Keyboard::isHeld(Keys::UP),
					  Keyboard::isHeld(Keys::DOWN),
					  Keyboard::isHeld(Keys::LEFT),
					  Keyboard::isHeld(Keys::RIGHT),
					  fElapsedTime);

		for (Terrain *terrain:vTerrains) {
			terrain->render(pShader);
		}

//	pGrid->draw();

		pShader->stop();


		if (vObjects.size() == 0) return;

		pShaderObjects->use();
		pShaderObjects->loadViewMatrix(pCamera);
		pShaderObjects->loadLight(pLight); //
		pShaderObjects->loadProjectionMatrix(projectionMatrix); //

		for (ObjectCluster *object:vObjects) {
			object->render(pShaderObjects);
		}

		pShaderObjects->stop();
		if (DBG) OpenGlUtils::glError("terrain tick");

		glDisable(GL_DEPTH_TEST);

	}

};

#pragma clang diagnostic pop
