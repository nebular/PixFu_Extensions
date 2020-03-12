// World.cpp
// PixFu Engine
//
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
#include "Utils.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
#pragma ide diagnostic ignored "err_typecheck_subscript_value"
#pragma ide diagnostic ignored "OCSimplifyInspection"
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace Pix {

	/** pbject counter */
	int WorldObjectBase::instanceCounter = 0;

	/** Object database */
	std::map<int, ObjectDbEntry_t> ObjectDb::Database;

	World::World(WorldConfig_t &config)
			: FuExtension(true),                                // require add on constructor
			  CONFIG(config) {
		if (config.debugMode == DEBUG_WIREFRAME)
			LayerVao::DRAWMODE = GL_LINES;
	};

	World::~World() {

		if (DBG) LogV(TAG, "Destroying World");
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

	WorldObject *World::add(ObjectProperties_t& features, ObjectLocation_t location, bool setHeight) {
		WorldObject *object = new WorldObject(CONFIG, features, location, WorldObject::CLASSID_CODE);
		add(object, setHeight);
		return object;
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
		CONFIG.light.load(pShader);
		pShader->stop();

		if (pShaderObjects != nullptr) {

			for (ObjectCluster *object:vObjects) {
				object->init();
			}

			pShaderObjects->use();
			pShaderObjects->bindAttributes();
			pShaderObjects->loadProjectionMatrix(projectionMatrix);
			CONFIG.light.load(pShaderObjects);
			pShaderObjects->stop();
		}

		if (DBG)
			LogV(TAG, SF("Init World, FOV %f, aspectRatio %f",
						 CONFIG.perspective.FOV, aspectRatio));

		return true;
	}

	void World::tick(Fu *engine, float fElapsedTime) {

		pCamera->update(fElapsedTime);

		glClearColor(CONFIG.backgroundColor.x, CONFIG.backgroundColor.y, CONFIG.backgroundColor.z, 1.0);
		glEnable(GL_DEPTH_TEST);

		pShader->use();
		pShader->loadViewMatrix(pCamera);
		
		if (bLightsChanged) {
			loadLights(pShader);
		}

		for (Terrain *terrain:vTerrains) {
			terrain->render(pShader);
		}

		pShader->stop();

		if (!vObjects.empty()) {

			// configure object shader
			pShaderObjects->use();
			pShaderObjects->loadViewMatrix(pCamera);
			if (bLightsChanged) {
				loadLights(pShader);
				loadLights(pShaderObjects);
				bLightsChanged = false;
			}
			// draw each cluster
			for (ObjectCluster *object:vObjects) {
				object->render(pShaderObjects);
			}

			pShaderObjects->stop();
			if (DBG) OpenGlUtils::glError("terrain tick");
		} else bLightsChanged = false;

		glDisable(GL_DEPTH_TEST);

		if (CONFIG.debugMode == DEBUG_COLLISIONS)
			canvas()->blank();

	}

	WorldObject *World::select(glm::vec3 &ray, bool exclusive) {
		WorldObject *selected = nullptr;
		iterateObjects([this, &ray, &selected, exclusive](WorldObject *obj) {
			// only select first one (simple behavior)
			// future might return a vector
			glm::vec3 pos = pCamera->getPosition();
			bool sel = obj->checkRayCollision(pos, ray);

			if (exclusive) {
				obj->setSelected(sel);
			} else {
				// on multiselect, toggle on reclick selected
				if (sel) obj->setSelected(!obj->isSelected());
			}

			if (sel && selected == nullptr) selected = obj;
		});
		return selected;
	}

	void World::selectAll(bool stat) {
		iterateObjects([stat](WorldObject *obj) {
			obj->setSelected(stat);
		});
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

		auto matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1.0F, 0.0F, 0.0F});
		matrix = glm::rotate(matrix, ryrads, {0.0F, 1.0F, 0.0F});
		matrix = glm::rotate(matrix, rzrads, {0.0F, 0.0F, 1.0F});
		matrix = glm::scale(matrix, {scale, scale, scale});
		return matrix * flipMatrix;
	}

	void DirLight::load(Shader *shader) const {
		shader->setVec3("dirLight.direction", mDirection);
		shader->setVec3("dirLight.ambient", mAmbient);
		shader->setVec3("dirLight.diffuse",  mDiffuse);
		shader->setVec3("dirLight.specular", mSpecular);
		shader->setFloat("dirLight.ka", ka);
	}

	void World::addLight(PointLight& p) {
		vPointLights.emplace_back(p);
		bLightsChanged = true;
	}

	void World::loadLights(Shader *shader) {
		const int MAXLIGHTS = 4;
		for (int i = 0, l = (int)vPointLights.size(); i<MAXLIGHTS; i++) {
			if (i<l) vPointLights.at(i).load(shader, i);
			else {
				std::string lex = std::string(SF("pointLights[%d]", i));
				shader->setInt(lex+".enabled", 0);
			}
		}
	}

	void PointLight::load(Shader *shader, int index, bool enable) const {
		std::string lex = std::string(SF("pointLights[%d]", index));
		shader->setInt(lex+".enabled", enable?1:0);
		if (enable) {
			shader->setVec3(lex+".position", mPosition / 1000.0f);
			shader->setVec3(lex+".ambient", mAmbient);
			shader->setVec3(lex+".diffuse",  mDiffuse);
			shader->setVec3(lex+".specular", mSpecular);
			shader->setFloat(lex+".constant", constant);
			shader->setFloat(lex+".quadratic", quadratic);
			shader->setFloat(lex+".linear", linear);
			shader->setFloat(lex+".ka", ka);
		}
	}

	void PointLight::update(Shader *shader, int index) const {
		std::string lex = std::string(SF("pointLights[%d]", index));
		shader->setVec3(lex+".position", mPosition);
		shader->setFloat(lex+".ka", ka); // interesting for animation
	}

};

#pragma clang diagnostic pop
