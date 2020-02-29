//
//  TerrainShader.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ObjectShader.hpp"
#include "Frustum.hpp"

namespace rgl {

	ObjectShader::ObjectShader(std::string name) : Shader(std::move(name)) {}

	void ObjectShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
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
		glm::mat4 viewMatrix = camera->getViewMatrix();
		setMat4("viewMatrix", (float *) &viewMatrix);

		glm::mat4 invViewMatrix = glm::inverse(viewMatrix);
		setMat4("invViewMatrix", (float *) &invViewMatrix);

		mFrustum = new Frustum(mProjectionMatrix * viewMatrix);
	}

	void ObjectShader::loadProjectionMatrix(glm::mat4 &projection) {
		setMat4("projectionMatrix", (float *) &projection);
		mProjectionMatrix = projection;
		mFrustum = nullptr;
	}


}
