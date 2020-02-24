//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"
#include "Terrain.hpp"
#include "Config.hpp"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
namespace rgl {

	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale) {

		glm::mat4 flipMatrix = glm::identity<glm::mat4>();
		
		/*
		  _            _
									|  1  0  0  0  |
									|  0  1  0  0  |
		 Matrix_Mirrored_On_Z = M * |  0  0 -1  0  |
									|_ 0  0  0  1 _|
		 
		 */
//		flipMatrix[2][2]=-1;

		glm::mat4 matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1, 0, 0});
		matrix = glm::rotate(matrix, ryrads, {0, 1, 0});
		matrix = glm::rotate(matrix, rzrads, {0, 0, 1});
		matrix = glm::scale(matrix, {scale, scale, scale});
		return matrix * flipMatrix;
	}


	std::string Terrain::TAG = "Terrain";

	Terrain::Terrain(WorldConfig_t planetConfig, TerrainConfig_t config)
			: CONFIG(config), PLANET(planetConfig) {

				std::string path =std::string(PATH_LEVELS)+"/" + config.name;
		pLoader = new ObjLoader(path + "/"+config.name+".obj");
		pTexture = new Texture2D(path+"/" + config.name + ".png");
		pHeightMap = Drawable::fromFile(path+"/" + config.name + ".heights.png");
		mSize = {pTexture->width(), pTexture->height()};

		pDirtTexture = new Texture2D(new Drawable(mSize.x, mSize.y));
		pDirtCanvas = new Canvas2D(pDirtTexture->buffer());
		pDirtCanvas->blank();
		if (DBG) LogV(TAG, SF("Created terrain %s", config.name.c_str()));
		pDirtCanvas->drawCircle(0,0,100,rgl::Colors::RED);
	};

	Terrain::~Terrain() {
		delete pTexture;
		pTexture = nullptr;
		if (DBG) LogV(TAG, SF("Destroyed terrain %s", CONFIG.name.c_str()));
	}


	void Terrain::render(TerrainShader *shader) {

		
		if (Keyboard::isHeld(Keys::A)) {
			scaletest+=0.001;
			LogE(TAG, SF("scale %f",scaletest));
		}
		
		if (Keyboard::isHeld(Keys::Z)) {
			scaletest-=0.001;
			LogE(TAG, SF("scale %f",scaletest));
		}
		

		
		if (!bInited) init();
		shader->textureUnit("modelTexture", pTexture);
		shader->loadShineVariables(1, 0.7);

		if (pDirtTexture != nullptr) {
			if (pDirtTexture->buffer()->clearDirty()) pDirtTexture->update();
			shader->textureUnit("dirtyTexture", pDirtTexture);
		}

		glm::mat4 tmatrix = createTransformationMatrix(
				{CONFIG.origin.x/1000, 0, CONFIG.origin.y/1000},
				0, 0, 0, 1
		);

		shader->loadTransformationMatrix(tmatrix);

		draw();

	}

	void Terrain::init() {

		LayerVao::add(
				pLoader->vertices(), pLoader->verticesCount(),
				pLoader->indices(), pLoader->indicesCount());

		pTexture->upload();
		if (pDirtTexture != nullptr) pDirtTexture->upload();

		bInited = true;

	}

	float Terrain::getHeight(glm::vec3 &posWorld3d) {
		if (pHeightMap != nullptr) {
			glm::vec2 posWorld = { (posWorld3d.x - CONFIG.origin.x), (posWorld3d.z - CONFIG.origin.y)};
			return CONFIG.scaleHeight * 1000 * pHeightMap->getPixel(posWorld.x, posWorld.y).r / (float) 255;
		}
		return 0;
	}

	bool Terrain::contains(glm::vec3 &posWorld) {
		return posWorld.x >= CONFIG.origin.x
			   && posWorld.z >= CONFIG.origin.y
			   && posWorld.x <= CONFIG.origin.x + mSize.x
			   && posWorld.z <= CONFIG.origin.y + mSize.y;
	}

};


#pragma clang diagnostic pop
