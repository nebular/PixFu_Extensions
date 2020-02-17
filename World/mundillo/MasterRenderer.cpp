//
//  MasterRenderer.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "MasterRenderer.hpp"

namespace rgl {

MasterRenderer::MasterRenderer(std::string shaderName, int screenWidth, int screenHeight){

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	createProjectionMatrix(screenWidth, screenHeight);

	//		renderer = new EntityRenderer(shader,projectionMatrix);
	pTerrainShader = new TerrainShader(shaderName);
	pTerrainRenderer = new TerrainRenderer(pTerrainShader, projectionMatrix);
}

void MasterRenderer::render(Light *sun, Camera *camera, Terrain *terrain){
	prepare();
	pTerrainShader->use();
	pTerrainShader->loadLight(sun);
	pTerrainShader->loadViewMatrix(camera);
	pTerrainRenderer->render(terrain);
	pTerrainShader->stop();
	OpenGlUtils::glError("render");
	//		terrains->clear();
	//		entities->clear();
}
/*
 public void processTerrain(Terrain terrain){
 terrains.add(terrain);
 }
 
 public void processEntity(Entity entity){
 TexturedModel entityModel = entity.getModel();
 List<Entity> batch = entities.get(entityModel);
 if(batch!=null){
 batch.add(entity);
 }else{
 List<Entity> newBatch = new ArrayList<Entity>();
 newBatch.add(entity);
 entities.put(entityModel, newBatch);
 }
 }
 */
void MasterRenderer::cleanUp(){
	//		pTerrainShader->cleanUp();
}

void MasterRenderer::prepare() {
//	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.69f, 0.89f, 0.98f, 1);
}

void MasterRenderer::createProjectionMatrix(int screenWidth, int screenHeight) {

	float aspectRatio = (float) screenWidth / (float) screenHeight;
	projectionMatrix = glm::perspective(FOV, aspectRatio, NEAR_PLANE, FAR_PLANE);
//	projectionMatrix = glm::ortho(0.0f,  (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);

}

}
