//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Planet.hpp"

#include "glm.hpp"
#include "Obj_Loader.hpp"

namespace rgl {


	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale);

	std::string Object::TAG = "Object";

	Object::Object(Object *clonable, ObjectConfig_t config)
			: CONFIG(std::move(config)), PLANET(clonable->PLANET), bMyTexture(false) {
		pTexture = clonable->pTexture;
		pLoader = clonable->pLoader;
	}

	Object::Object(PlanetConfig_t planetConfig, ObjectConfig_t config)
			: CONFIG(std::move(config)), PLANET(std::move(planetConfig)), bMyTexture(true) {

		pTexture = new Texture2D("objects/" + config.name + ".png");

		pLoader = new objl::Loader();

		if (!pLoader->LoadFile("objects/"+config.name+".obj"))
			throw std::runtime_error("Problem loading object "+config.name);

		if (DBG) LogV(TAG, SF("Created Object %s", config.name.c_str()));

	};

	Object::~Object() {
		if (bMyTexture) delete pTexture;
		pTexture = nullptr;
		if (DBG) LogV(TAG, SF("Destroyed Object %s", CONFIG.name.c_str()));
	}

	Object *Object::clone(ObjectConfig_t config) {
		return new Object(this, std::move(config));
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

	void Object::render(ObjectShader *shader) {

		if (!bInited) init();

		shader->textureUnit("modelTexture", pTexture);
		shader->loadShineVariables(1, 0.7);

		glm::mat4 tmatrix = createTransformationMatrix(
				CONFIG.position,
				CONFIG.rotation.x, CONFIG.rotation.y, CONFIG.rotation.z, CONFIG.scale
		);

		shader->loadTransformationMatrix(tmatrix);

		draw();
	}

	void Object::init() {

		std::vector<objl::Mesh> &meshes = pLoader->LoadedMeshes;
		std::vector<unsigned int> indices = pLoader->LoadedIndices;

		Layer::setup(
				(float *)&meshes[0], (int)meshes.size(),
				&indices[0], (int)indices.size());

		if (bMyTexture) pTexture->upload();

		bInited = true;

	}

};

