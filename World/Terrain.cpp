//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Planet.hpp"

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

	Terrain::Terrain(PlanetConfig_t planetConfig, TerrainConfig_t config)
			: CONFIG(config), PLANET(planetConfig) {

		pTexture = new Texture2D("maps/" + config.name + ".png");

		if (DBG) LogV(TAG, SF("Created terrain %s", config.name.c_str()));

	};

	Terrain::~Terrain() {
		delete pTexture;
		pTexture = nullptr;
		if (DBG) LogV(TAG, SF("Destroyed terrain %s", CONFIG.name.c_str()));
	}

	void Terrain::initMesh(uint vertexCount, uint twidth, uint theight) {

		const uint count = vertexCount * vertexCount;
		const uint verticesSize = count * 3;
		const uint normalsSize = count * 3;
		const uint texturesSize = count * 2;

		const uint compsize = verticesSize + normalsSize + texturesSize;

		const uint indicesSize = 6 * (vertexCount - 1) * (vertexCount - 1);

		float *tortilla = new float[compsize]{0};

		GLuint *indices = new GLuint[indicesSize]{0};
		int vertexPointer = 0;

		for (uint i = 0; i < vertexCount; i++) {
			for (uint j = 0; j < vertexCount; j++) {
				tortilla[vertexPointer * 3] = (float) j / ((float) vertexCount - 1) * twidth;
				tortilla[vertexPointer * 3 + 1] = 0; // i / (float)vertexCount; // (random()%100)/100.0 - 2;
				tortilla[vertexPointer * 3 + 2] = (float) i / ((float) vertexCount - 1) * theight;
				tortilla[vertexPointer * 3 + 3] = 0;
				tortilla[vertexPointer * 3 + 4] = 1;
				tortilla[vertexPointer * 3 + 5] = 0;
				tortilla[vertexPointer * 3 + 6] = (float) j / ((float) vertexCount - 1);
				tortilla[vertexPointer * 3 + 7] = (float) i / ((float) vertexCount - 1);
				vertexPointer++;
			}
		}

		int pointer = 0;
		for (unsigned int gz = 0; gz < vertexCount - 1; gz++) {
			for (uint gx = 0; gx < vertexCount - 1; gx++) {
				uint topLeft = (gz * vertexCount) + gx;
				uint topRight = topLeft + 1;
				uint bottomLeft = ((gz + 1) * vertexCount) + gx;
				uint bottomRight = bottomLeft + 1;
				indices[pointer++] = topLeft;
				indices[pointer++] = bottomLeft;
				indices[pointer++] = topRight;
				indices[pointer++] = topRight;
				indices[pointer++] = bottomLeft;
				indices[pointer++] = bottomRight;
			}
		}
		Layer::setup(tortilla, compsize,
					 indices, indicesSize);

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

		Layer::setup((float *) Surface::VERTICES, sizeof(Surface::VERTICES),
					 (unsigned int *) Surface::INDICES, sizeof(Surface::INDICES));

		pTexture->upload();

		bInited = true;

	}

};

