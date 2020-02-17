//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"

#include "glm.hpp"
#include "Planet.hpp"

namespace rgl {


	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale) {

		glm::mat4 matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1,0,0});
		matrix = glm::rotate(matrix, ryrads, {0,1,0});
		matrix = glm::rotate(matrix, rzrads, {0,0,1});
		matrix = glm::scale(matrix, {scale,scale,scale});
		return matrix;
	}


	World::World(WorldConfig_t config, Perspective_t perspective)
			: PERSPECTIVE(perspective), CONFIG(config) {

		// initialize texture and shader
		pTexture = new Texture2D("maps/"+config.name+".png");
		pShader = new TerrainShader(config.shaderName);

	};

	void World::initMesh(uint vertexCount, uint twidth, uint theight) {

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

	bool World::init(PixFu *engine) {


//	initMesh(10, pTexture->width(), pTexture->height());

		Layer::setup((float *) Surface::VERTICES, sizeof(Surface::VERTICES),
					 (unsigned int *) Surface::INDICES, sizeof(Surface::INDICES));

		pShader->use();

		pTexture->upload();

		pShader->textureUnit("modelTexture", pTexture);


		glm::mat4 tmatrix = createTransformationMatrix(
				{CONFIG.origin.x, 0, CONFIG.origin.y},
				0, 0, 0, 1
		);

		pShader->loadTransformationMatrix(tmatrix);

		float aspectRatio = (float) pTexture->width() / (float) pTexture->height();

		projectionMatrix = glm::perspective(PERSPECTIVE.FOV, aspectRatio, PERSPECTIVE.NEAR_PLANE, PERSPECTIVE.FAR_PLANE);

		pShader->loadProjectionMatrix(projectionMatrix);
		pLight = new Light(CONFIG.lightPosition, {1, 1, 1});
		pCamera = new Camera();
		pShader->loadShineVariables(1, 0.7);
		pShader->loadLight(pLight);
		pShader->loadViewMatrix(pCamera);

		return true;
	}

	void World::tick(PixFu *engine, float fElapsedTime) {
		pShader->use();
//	pShader->loadLight(pLight);
		pCamera->move();
//		pShader->loadProjectionMatrix(projectionMatrix);
		pShader->loadViewMatrix(pCamera);
		draw();
	}


};

