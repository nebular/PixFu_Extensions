//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"

#include "glm.hpp"
#include "Layer.hpp"
#include "ObjectLoader.hpp"

namespace rgl {

	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale);

	std::string ObjectCluster::TAG = "ObjectCluster";

	ObjectCluster::ObjectCluster(std::string name, WorldConfig_t planetConfig, ObjectConfig_t normalizeConfiguration)
			: NAME(std::move(name)), PLANET(std::move(planetConfig)), PLACER(std::move(normalizeConfiguration)) {

		if (DBG) LogV(TAG, "New Object Cluster " + NAME);

		pTexture = new Texture2D("objects/" + NAME + ".png");

		pLoader = new ObjectLoader("objects/" + NAME + ".obj");

		mPlacer = createTransformationMatrix(
				PLACER.position, //CONFIG.position,
				PLACER.rotation.x, PLACER.rotation.y, PLACER.rotation.z, PLACER.scale
		);


		if (DBG) LogV(TAG, SF("Created ObjectCluster %s", NAME.c_str()));

	};

	void ObjectCluster::add(ObjectConfig_t &config) {
		vInstances.push_back(config);
		if (DBG) LogV(TAG, SF("Add to cluster %s, total %d", NAME.c_str(), vInstances.size()));
	}

	ObjectCluster::~ObjectCluster() {
		delete pTexture;
		pTexture = nullptr;
		if (DBG) LogV(TAG, SF("Destroyed ObjectCluster %s", NAME.c_str()));
	}

	void ObjectCluster::render(ObjectShader *shader) {

		if (!bInited) init();

		shader->textureUnit("modelTexture", pTexture);
		shader->loadShineVariables(1, 0.7);
		pTexture->bind();

		bind();

		for (ObjectConfig_t &config : vInstances) {

			glm::mat4 tmatrix = createTransformationMatrix(
					config.position, //CONFIG.position,
					config.rotation.x, config.rotation.y, config.rotation.z, config.scale
			);
			glm::mat4 result = tmatrix * mPlacer;

			shader->loadTransformationMatrix(result);

			draw(false);
		}
		unbind();
	}

	void ObjectCluster::init() {

		Layer::setup(
				pLoader->vertices(), pLoader->verticesCount(),
				pLoader->indices(), pLoader->indicesCount());

		pTexture->upload();

		bInited = true;

	}

};

