//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"
#include "LayerVao.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
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

		pLoader = new ObjLoader("maps/" + config.name + ".obj");
		pTexture = new Texture2D("maps/" + config.name + ".png");
		pHeightMap = Drawable::fromFile("maps/" + config.name + ".heights.png");
		mSize = {pTexture->width()/1000.0f, pTexture->height()/1000.0f};

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

		LayerVao::setup(
				pLoader->vertices(), pLoader->verticesCount(),
				pLoader->indices(), pLoader->indicesCount());

		pTexture->upload();

		bInited = true;

	}

	float Terrain::getHeight(glm::vec2 posWorld) {


		if (pHeightMap != nullptr) {

			posWorld -= CONFIG.origin;

			// posWorld is texturepx / 1000 (by definition)
			posWorld *= 1000;
			posWorld = { fmod(posWorld.x, mSize.x) * 1000, fmod(posWorld.y, mSize.y) * 1000};

			return pHeightMap->getPixel(static_cast<int>(posWorld.x), static_cast<int>(posWorld.y)).r / (float) 255;
		}
		return 0;

	}

	bool Terrain::contains(glm::vec2 posWorld) {
		return posWorld.x >= CONFIG.origin.x
			   && posWorld.y >= CONFIG.origin.y
			   && posWorld.x <= CONFIG.origin.x + mSize.x
			   && posWorld.y <= CONFIG.origin.y + mSize.y;
	}

};


#pragma clang diagnostic pop