// Base class for a 3d world
//
// Provides methods to add terrains and objects
// and has a camera and lighting. This class does not know about
// physics, that is the task of derived classes.
//
// Created by rodo on 2020-02-17.
//

#include "World.hpp"
#include "PixFu.hpp"
#include "OpenGL.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace rgl {

	constexpr Perspective_t World::PERSP_FOV90_LOW;
	constexpr Perspective_t World::PERSP_FOV90_MID;
	constexpr Perspective_t World::PERSP_FOV90_FAR;
	constexpr Perspective_t World::PERSP_FOV60_LOW;
	constexpr Perspective_t World::PERSP_FOV60_MID;
	constexpr Perspective_t World::PERSP_FOV60_FAR;
	constexpr Transformation_t World::TRANSFORM_FLIPX, World::TRANSFORM_FLIPY, World::TRANSFORM_FLIPZ;

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

//	void World::add(const char *name, ObjectConfig_t objectConfig, ObjectConfig_t initialTransform) {

	void World::add(WorldObject *object) {

		auto clusterItem = mCluesters.find(object->CLASS);
		ObjectCluster *cluster;

		if (clusterItem == mCluesters.end()) {
			// create object cluster
			cluster = new ObjectCluster(this, object->CLASS, CONFIG.worldTransform);
			vObjects.push_back(cluster);
			mCluesters[object->CLASS] = cluster;
		} else {
			cluster = clusterItem->second;
		}

		cluster->add(object, false);
	}

	bool World::init(PixFu *engine) {

		auto toRad = [](float degs) { return degs * M_PI / 180; };

		pLight = new Light(CONFIG.lightPosition, CONFIG.lightColor);
		pCamera = new Camera();

		pCamera->setHeight(PERSPECTIVE.C_HEIGHT);
		pCamera->setPitch(PERSPECTIVE.C_PITCH);

		pShader->use();
		pShader->bindAttributes();

		// load projection matrix
		float aspectRatio = (float) engine->screenWidth() / (float) engine->screenHeight();
		projectionMatrix = glm::perspective((float) toRad(PERSPECTIVE.FOV), aspectRatio, PERSPECTIVE.NEAR_PLANE,
											PERSPECTIVE.FAR_PLANE);

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

	void World::tick(PixFu *engine, float fElapsedTime) {

		pCamera->update(fElapsedTime);

		glClearColor(CONFIG.backgroundColor.x, CONFIG.backgroundColor.y, CONFIG.backgroundColor.z, 1.0);
		glEnable(GL_DEPTH_TEST);

		pShader->use();
		pShader->loadViewMatrix(pCamera);
		pShader->loadLight(pLight); //

		for (Terrain *terrain:vTerrains) {
			terrain->render(pShader);
		}

		pShader->stop();

		if (vObjects.size() == 0) return;

		pShaderObjects->use();
		pShaderObjects->loadViewMatrix(pCamera);
		pShaderObjects->loadLight(pLight);

		for (ObjectCluster *object:vObjects) {
			object->render(pShaderObjects);
		}


		pShaderObjects->stop();
		if (DBG) OpenGlUtils::glError("terrain tick");

		glDisable(GL_DEPTH_TEST);

	}


	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads,
										 float scale, bool flipX = true, bool flipY = false, bool flipZ = false) {

		glm::mat4 flipMatrix = glm::identity<glm::mat4>();

		/*
		  create transformation matrix. can flip space.
									 _            _
									|  1  0  0  0  |
									|  0  1  0  0  |
		 Matrix_Mirrored_On_Z = M * |  0  0 -1  0  |
									|_ 0  0  0  1 _|

		 */

		if (flipX) flipMatrix[0][0] = -1.0f;
		if (flipY) flipMatrix[1][1] = -1.0f;
		if (flipZ) flipMatrix[2][2] = -1.0f;


		glm::mat4 matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1.0f, 0.0f, 0.0f});
		matrix = glm::rotate(matrix, ryrads, {0.0f, 1.0f, 0.0f});
		matrix = glm::rotate(matrix, rzrads, {0.0f, 0.0f, 1.0f});
		matrix = glm::scale(matrix, {scale, scale, scale});
		return matrix * flipMatrix;
	}
};

#pragma clang diagnostic pop
