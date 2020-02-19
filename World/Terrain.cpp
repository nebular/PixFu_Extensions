//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"
#include "Layer.hpp"


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


	std::string Terrain::TAG = "Terrain";

	Terrain::Terrain(WorldConfig_t planetConfig, TerrainConfig_t config)
			: CONFIG(config), PLANET(planetConfig) {

		pTexture = new Texture2D("maps/" + config.name + ".png");
		pLoader = new ObjLoader("maps/" + config.name + ".obj");

		if (DBG) LogV(TAG, SF("Created terrain %s", config.name.c_str()));

	};

	Terrain::~Terrain() {
		delete pTexture;
		pTexture = nullptr;
		if (DBG) LogV(TAG, SF("Destroyed terrain %s", CONFIG.name.c_str()));
	}


	void Terrain::render(TerrainShader *shader) {

		if (!bInited) init();
		shader->textureUnit("modelTexture", pTexture);
		shader->loadShineVariables(1, 0.7);

		glm::mat4 tmatrix = createTransformationMatrix(
				{CONFIG.origin.x, 0, CONFIG.origin.y},
				0, 0, 0, 1
		);

		shader->loadTransformationMatrix(tmatrix);

		draw();

	}

	void Terrain::init() {

		Layer::setup(
				pLoader->vertices(), pLoader->verticesCount(),
				pLoader->indices(), pLoader->indicesCount());

		pTexture->upload();

		bInited = true;

	}

};

