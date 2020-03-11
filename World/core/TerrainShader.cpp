//
//  TerrainShader.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "TerrainShader.hpp"
#include "Camera.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace Pix {

	TerrainShader::TerrainShader(const std::string& name) : Shader(name) {}

	void TerrainShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
	}

//	void TerrainShader::loadShineVariables(float damper, float reflectivity) {
//		setFloat("shineDamper", damper);
//		setFloat("reflectivity", reflectivity);
//	}

	void TerrainShader::loadTransformationMatrix(glm::mat4 &matrix) {
		setMat4("transformationMatrix", (float *) &matrix);
	}

	void TerrainShader::loadLight(const Light& light) {
		light.load(this);
	}

	void TerrainShader::loadViewMatrix(Camera *camera) {
		glm::mat4& viewMatrix = camera->getViewMatrix();
		setMat4("viewMatrix", (float *) &viewMatrix);
		glm::mat4& invViewMatrix = camera->getInvViewMatrix();
		setMat4("invViewMatrix", (float *) &invViewMatrix);

//		glm::mat4 invViewMatrix = glm::affineInverse(viewMatrix);
//		setMat4("invViewMatrix", (float *) &invViewMatrix);
	}

	void TerrainShader::loadProjectionMatrix(glm::mat4 &projection) {
		setMat4("projectionMatrix", (float *) &projection);
	}


}
