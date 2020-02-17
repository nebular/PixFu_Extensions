//
//  TerrainShader.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ObjectShader.hpp"

namespace rgl {
	
	glm::mat4 createViewMatrix(Camera *camera);

	ObjectShader::ObjectShader(std::string name) : Shader(name) {}

	void ObjectShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "textureCoordinates");
		bindAttribute(2, "normal");
	}

	void ObjectShader::loadShineVariables(float damper, float reflectivity) {
		setFloat("shineDamper", damper);
		setFloat("reflectivity", reflectivity);
	}

	void ObjectShader::loadTransformationMatrix(glm::mat4 &matrix) {
		setMat4("transformationMatrix", (float *) &matrix);
	}

	void ObjectShader::loadLight(Light *light) {
		glm::vec3 l = light->position();
		glm::vec3 c = light->getColour();
		setVec3("lightPosition", l.x, l.y, l.z);
		setVec3("lightColour", c.x, c.y, c.z);
	}

	void ObjectShader::loadViewMatrix(Camera *camera) {
		glm::mat4 viewMatrix = createViewMatrix(camera);
		setMat4("viewMatrix", (float *) &viewMatrix);
	}

	void ObjectShader::loadProjectionMatrix(glm::mat4 &projection) {
		setMat4("projectionMatrix", (float *) &projection);
	}


}
