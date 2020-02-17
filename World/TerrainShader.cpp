//
//  TerrainShader.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "TerrainShader.hpp"
#include "Camera.hpp"
#include "ext.hpp"

namespace rgl {

glm::mat4 createViewMatrix(Camera *camera) {
	glm::mat4 matrix = glm::identity<glm::mat4>();
	
	matrix = glm::rotate(matrix, camera->getPitch(), {1,0,0});
	matrix = glm::rotate(matrix, camera->getYaw(), {0,1,0});
	matrix = glm::rotate(matrix, camera->getRoll(), {0,0,1});

	glm::vec3 cameraPos = camera->getPosition();
	glm::vec3 negativeCameraPos = {-cameraPos.x,-cameraPos.y,-cameraPos.z};
	matrix = glm::translate(matrix, negativeCameraPos);
	
	return matrix;
}


TerrainShader::TerrainShader(std::string name):Shader(name) {}

void TerrainShader::bindAttributes() {
	bindAttribute(0, "position");
	bindAttribute(1, "textureCoordinates");
	bindAttribute(2, "normal");
}

void TerrainShader::loadShineVariables(float damper,float reflectivity){
	setFloat("shineDamper", damper);
	setFloat("reflectivity", reflectivity);
}

void TerrainShader::loadTransformationMatrix(glm::mat4 &matrix){
	setMat4("transformationMatrix", (float *)&matrix);
}

void TerrainShader::loadLight(Light *light){
	glm::vec3 l = light->position();
	glm::vec3 c = light->getColour();
	setVec3("lightPosition",  l.x, l.y, l.z);
	setVec3("lightColour", c.x,c.y,c.z);
}

void  TerrainShader::loadViewMatrix(Camera *camera){
	glm::mat4 viewMatrix = createViewMatrix(camera);
	setMat4("viewMatrix", (float *)&viewMatrix);
}

void  TerrainShader::loadProjectionMatrix(glm::mat4 &projection){
	setMat4("projectionMatrix", (float *)&projection);
}


}
