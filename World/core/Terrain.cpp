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
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

namespace Pix {

	std::string Terrain::TAG = "Terrain";

	Terrain::Terrain(WorldConfig_t planetConfig, TerrainConfig_t config)
			: CONFIG(config), PLANET(planetConfig) {

		std::string path = std::string(PATH_LEVELS) + "/" + config.name;

		if (config.staticMesh == nullptr) {

			// load resources
			pLoader = new ObjLoader(path + "/" + config.name + ".obj");
			pTexture = new Texture2D(path + "/" + config.name + ".png");
			pHeightMap = Drawable::fromFile(path + "/" + config.name + ".heights.png");

		} else {

			pLoader = new ObjLoader(config.staticMesh);
			pTexture = new Texture2D(2000, 2000);
			pTexture->buffer()->clear(Pix::Colors::GREEN.scale(0.3));
			pHeightMap = nullptr;

		}

		mSize = {pTexture->width(), pTexture->height()};

		// 3d canvas
		if (PLANET.withCanvas) {
			pDirtTexture = new Texture2D(new Drawable(static_cast<int>(mSize.x), static_cast<int>(mSize.y)));
			pDirtCanvas = new Canvas2D(pDirtTexture->buffer(), new Font(PLANET.withFont));
			pDirtCanvas->blank();
			if (config.wireframe) wireframe();
		}


		if (DBG) LogV(TAG, SF("Created terrain %s", config.name.c_str()));
	};

	Terrain::~Terrain() {
		if (pTexture != nullptr) {
			delete pTexture;
			pTexture = nullptr;
		}
		if (PLANET.withCanvas) {
			delete pDirtCanvas;
			delete pDirtTexture;
			pDirtCanvas = nullptr;
			pDirtTexture = nullptr;
		}
		if (DBG) LogV(TAG, SF("Destroyed terrain %s", CONFIG.name.c_str()));
	}

	void Terrain::wireframe(int INC) {

		for (int x = 0, l = pDirtCanvas->width(); x < l; x += INC) {
			for (int y = 0, m = pDirtCanvas->height(); y < m; y += INC) {
				pDirtCanvas->drawLine(x, 0, x, m, Pix::Colors::RED);
				pDirtCanvas->drawLine(0, y, l, y, Pix::Colors::BLUE);
			}
		}
	}

	void Terrain::render(TerrainShader *shader) {

		if (!bInited) init(shader);

		shader->textureUnit("modelTexture", pTexture);
		shader->loadShineVariables(1, 0.7);

		if (pDirtTexture != nullptr) {
			if (pDirtTexture->buffer()->clearDirty()) pDirtTexture->update();
			shader->textureUnit("dirtyTexture", pDirtTexture);
		}

		draw();

	}

	void Terrain::init(TerrainShader *shader) {

		glm::mat4 tmatrix = createTransformationMatrix(
				{CONFIG.origin.x / 1000, 0, CONFIG.origin.y / 1000},
				0, 0, 0, 1, false, false, false) * PLANET.terrainTransform.toMatrix();

		shader->loadTransformationMatrix(tmatrix);

		LayerVao::add(
				pLoader->vertices(), pLoader->verticesCount(),
				pLoader->indices(), pLoader->indicesCount());

		pTexture->upload();
		if (pDirtTexture != nullptr) pDirtTexture->upload();

		bInited = true;
	}
};


#pragma clang diagnostic pop
