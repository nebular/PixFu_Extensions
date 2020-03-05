// Base class for a 3d world
//
// Provides methods to add terrains and objects
// and has a camera and lighting. This class does not know about
// physics, that is the task of derived classes.
//
// Created by rodo on 2020-02-17.
//

#include <utility>
#include "World.hpp"
#include "WorldMeta.hpp"
#include "Fu.hpp"
#include "Config.hpp"
#include "OpenGL.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
#pragma ide diagnostic ignored "err_typecheck_subscript_value"
#pragma ide diagnostic ignored "OCSimplifyInspection"
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace Pix {

	std::string World::TAG = "World";

	/** metronome (animaations) */
	float World::METRONOME = 0;

	/** pbject counter */
	int WorldObjectBase::instanceCounter = 0;

	/** Object database */
	std::map<int, ObjectDbEntry_t> ObjectDb::Database;


	World::World(WorldConfig_t config)
			: CONFIG(std::move(config)) {
		if (config.debugMode == DEBUG_WIREFRAME)
			LayerVao::DRAWMODE = GL_LINES;
	};

	World::~World() {

		if (DBG) LogV(TAG, "Destroying planet");
		for (Terrain *terrain : vTerrains) {
			delete terrain;
		}
		vTerrains.clear();
	}

	Terrain *World::add(TerrainConfig_t terrainConfig) {
		Terrain *world = new Terrain(CONFIG, std::move(terrainConfig));
		vTerrains.emplace_back(world);
		return world;
	}

	WorldObject *World::add(ObjectProperties_t features, ObjectLocation_t location, bool setHeight) {
		WorldObject *object = new WorldObject(CONFIG, features, location, WorldObject::CLASSID_CODE);
		add(object, setHeight);
		return object;
	}

	void WorldObject::process(World *world, float fElapsedTime) {

		// process intrinsic animation
		if (CONFIG.animation.enabled) {

			// apply rotation
			rot().x += CONFIG.animation.deltaRotationX * fElapsedTime;
			rot().y += CONFIG.animation.deltaRotationY * fElapsedTime;
			rot().z += CONFIG.animation.deltaRotationZ * fElapsedTime;

			// apply scale pulse
			if (CONFIG.animation.scalePulse > 0)
				fRadiusAnimator = sinf(World::METRONOME) * CONFIG.animation.scalePulse;

		}

		if (world->CONFIG.debugMode == DEBUG_COLLISIONS)
			world->canvas()->drawCircle(static_cast<int32_t>(pos().x), static_cast<int32_t>(pos().z), static_cast<int32_t>(radius()),
										Pix::Colors::RED);

	}


	void World::add(WorldObject *object, bool setHeight) {

		if (setHeight) {
			float height = getHeight(object->pos());
			object->pos().y = height;
		}

		auto clusterItem = mClusters.find(object->CLASS);
		ObjectCluster *cluster;

		if (clusterItem == mClusters.end()) {
			// create object cluster
			cluster = new ObjectCluster(this, object->CLASS, CONFIG.worldTransform);
			vObjects.emplace_back(cluster);
			mClusters[object->CLASS] = cluster;
		} else {
			cluster = clusterItem->second;
		}

		cluster->add(object);
	}

	bool World::init(Fu *engine) {

		auto toRad = [](float degs) { return degs * M_PI / 180.0F; };

		pShader = new TerrainShader(CONFIG.shaderName);
		pShaderObjects = new ObjectShader(CONFIG.shaderName + "_objects");

		pLight = new Light(CONFIG.lightPosition, CONFIG.lightColor);
		pCamera = new Camera();

		pCamera->setHeight(CONFIG.perspective.C_HEIGHT);
		pCamera->setPitch(CONFIG.perspective.C_PITCH);

		pShader->use();
		pShader->bindAttributes();

		// load projection matrix
		float aspectRatio = (float) engine->screenWidth() / (float) engine->screenHeight();

		projectionMatrix = glm::perspective(
				(float) toRad(CONFIG.perspective.FOV),
				aspectRatio,
				CONFIG.perspective.NEAR_PLANE,
				CONFIG.perspective.FAR_PLANE
		);

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
			// light is updated every frame
			// pShaderObjects->loadLight(pLight);
			pShaderObjects->stop();
		}

		if (DBG)
			LogV(TAG, SF("Init World, FOV %f, aspectRatio %f",
						 CONFIG.perspective.FOV, aspectRatio));

		return true;
	}

	void World::tick(Fu *engine, float fElapsedTime) {

		METRONOME += fElapsedTime;

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

		if (!vObjects.empty()) {

			// configure object shader
			pShaderObjects->use();
			pShaderObjects->loadViewMatrix(pCamera);
			pShaderObjects->loadLight(pLight);

			// draw each cluster
			for (ObjectCluster *object:vObjects) {
				object->render(pShaderObjects);
			}

			pShaderObjects->stop();
			if (DBG) OpenGlUtils::glError("terrain tick");
		}

		glDisable(GL_DEPTH_TEST);

		if (CONFIG.debugMode == DEBUG_COLLISIONS)
			canvas()->blank();

	}


	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads,
										 float scale, bool flipX = true, bool flipY = false, bool flipZ = false) {

		auto flipMatrix = glm::identity<glm::mat4>();

		/*
		  create transformation matrix. can flip space.
									 _            _
									|  1  0  0  0  |
									|  0  1  0  0  |
		 Matrix_Mirrored_On_Z = M * |  0  0 -1  0  |
									|_ 0  0  0  1 _|

		 */

		if (flipX) flipMatrix[0][0] = -1.0F;
		if (flipY) flipMatrix[1][1] = -1.0F;
		if (flipZ) flipMatrix[2][2] = -1.0F;


		glm::mat4 matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1.0F, 0.0F, 0.0F});
		matrix = glm::rotate(matrix, ryrads, {0.0F, 1.0F, 0.0F});
		matrix = glm::rotate(matrix, rzrads, {0.0F, 0.0F, 1.0F});
		matrix = glm::scale(matrix, {scale, scale, scale});
		return matrix * flipMatrix;
	}
};

#pragma clang diagnostic pop
