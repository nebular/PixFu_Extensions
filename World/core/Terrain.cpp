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
	
	std::string Terrain::TAG = "Terrain";

	/** Convenience to create a transform matrix */
	glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale) {

		glm::mat4 flipMatrix = glm::identity<glm::mat4>();
		
		/*
		  _            _
									|  1  0  0  0  |
									|  0  1  0  0  |
		 Matrix_Mirrored_On_Z = M * |  0  0 -1  0  |
									|_ 0  0  0  1 _|
		 
		 */

		// no flip at the moment but something is going on here
		// flipMatrix[2][2]=-1;

		glm::mat4 matrix = glm::identity<glm::mat4>();

		matrix = glm::translate(matrix, translation);
		matrix = glm::rotate(matrix, rxrads, {1, 0, 0});
		matrix = glm::rotate(matrix, ryrads, {0, 1, 0});
		matrix = glm::rotate(matrix, rzrads, {0, 0, 1});
		matrix = glm::scale(matrix, {scale, scale, scale});
		return matrix * flipMatrix;
	}



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
		wireframe();
		if (DBG) LogV(TAG, SF("Created terrain %s", config.name.c_str()));
	};

	Terrain::~Terrain() {
		delete pTexture;
		pTexture = nullptr;
		if (DBG) LogV(TAG, SF("Destroyed terrain %s", CONFIG.name.c_str()));
	}

	void Terrain::wireframe(int INC) {

		for (int x = 0, l = pDirtCanvas->width(); x<l; x+=INC ) {
			for (int y = 0, m = pDirtCanvas->height(); y<m; y+=INC ) {
				pDirtCanvas->drawLine(x, 0, x, m, rgl::Colors::RED);
				pDirtCanvas->drawLine(0, y, l, y, rgl::Colors::BLUE);
			}
		}
	}
	
	void Terrain::render(TerrainShader *shader) {

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


};


#pragma clang diagnostic pop
