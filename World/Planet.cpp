//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Planet.hpp"
#include "PixFu.hpp"
#include "OpenGL.h"
namespace rgl {

constexpr Perspective_t Planet::PERSP_FOV90_LOW;
constexpr Perspective_t Planet::PERSP_FOV90_MID;
constexpr Perspective_t Planet::PERSP_FOV90_FAR;
constexpr Perspective_t Planet::PERSP_FOV60_LOW;
constexpr Perspective_t Planet::PERSP_FOV60_MID;
constexpr Perspective_t Planet::PERSP_FOV60_FAR;

std::string Planet::TAG = "Planet";

Planet::Planet(PlanetConfig_t config, Perspective_t perspective)
: PERSPECTIVE(perspective), CONFIG(config) {
	
	pShader = new TerrainShader(CONFIG.shaderName);
	pShaderObjects = new ObjectShader(CONFIG.shaderName+"_objects");
	
};

Planet::~Planet() {
	
	if (DBG) LogE(TAG, "Destroying planet");
	for (Terrain *terrain : vTerrains) {
		delete terrain;
	}
	vTerrains.clear();
}

void Planet::add(TerrainConfig_t terrainConfig) {
	Terrain *world = new Terrain(CONFIG, terrainConfig);
	vTerrains.push_back(world);
}

void Planet::add(const char *name, ObjectConfig_t objectConfig,  ObjectConfig_t initialTransform) {
	
	auto clusterItem =mCluesters.find(name);
	Object *cluster;
	
	if (clusterItem == mCluesters.end() ) {
		// create object cluster
		cluster = new Object(name, CONFIG, initialTransform);
		vObjects.push_back(cluster);
		mCluesters[name] = cluster;
	} else {
		cluster = clusterItem->second;
	}
	
	cluster->add(objectConfig);
}

bool Planet::init(PixFu *engine) {
	
	pLight = new Light(CONFIG.lightPosition, CONFIG.lightColor);
	pCamera = new NewCamera();

	pCamera->setHeight(PERSPECTIVE.C_HEIGHT);
	pCamera->setPitch(PERSPECTIVE.C_PITCH);
	
	pShader->use();
	pShader->bindAttributes();
	
	// load projection matrix
	float aspectRatio = (float) engine->screenWidth() / (float) engine->screenHeight();
	projectionMatrix = glm::perspective(PERSPECTIVE.FOV, aspectRatio, PERSPECTIVE.NEAR_PLANE, PERSPECTIVE.FAR_PLANE);
	
	pShader->loadProjectionMatrix(projectionMatrix);
	//		pShader->loadLight(pLight);
	
	pShader->stop();
	
	if (pShaderObjects != nullptr) {
		
		for (Object *object:vObjects) {
			object->init();
		}
		
		pShaderObjects->use();
		pShaderObjects->bindAttributes();
		pShaderObjects->loadProjectionMatrix(projectionMatrix);
		//			pShaderObjects->loadLight(pLight);
		pShaderObjects->stop();
	}
	
//	pGrid->build();
	
	if (DBG) LogV(TAG, SF("Init Planet, FOV %f, aspectRatio %f",
						  PERSPECTIVE.FOV, aspectRatio));
	
	return true;
}

void Planet::tick(PixFu *engine, float fElapsedTime) {
	
	glEnable(GL_DEPTH_TEST);
	
	pShader->use();
	pShader->loadViewMatrix(pCamera);
	pShader->loadLight(pLight); //
	
	pCamera->move(fElapsedTime);
	
	for (Terrain *terrain:vTerrains) {
		terrain->render(pShader);
	}

//	pGrid->draw();

	pShader->stop();
	
	
	if (vObjects.size()==0) return;
	
	pShaderObjects->use();
	pShaderObjects->loadViewMatrix(pCamera);
	pShaderObjects->loadLight(pLight); //
	pShaderObjects->loadProjectionMatrix(projectionMatrix); //
	
	for (Object *object:vObjects) {
		object->render(pShaderObjects);
	}
	
	pShaderObjects->stop();
	if (DBG) OpenGlUtils::glError("terrain tick");
	
	glDisable(GL_DEPTH_TEST);
	
}

Grid::Grid(int width, int height) {
	
	for ( int row=0; row<height; row++ ) {
		for ( int col=0; col<width; col++ ) {
			vVertices.push_back({{(float) col, 0.0f, (float) row}});
		}
	}
	
	for ( int row=0; row<height-1; row++ ) {
		if ( (row&1)==0 ) { // even rows
			for ( int col=0; col<width; col++ ) {
				vIndices.push_back( col + row * width);
				vIndices.push_back( col + (row+1) * width);
			}
		} else { // odd rows
			for ( int col=width-1; col>0; col-- ) {
				vIndices.push_back( col + (row+1) * width );
				vIndices.push_back( col - 1 + + row * width );
			}
		}
	}
	if ( (height&1) && height>2 ) {
		vIndices.push_back((height-1) * width);
	}
	
}

};
