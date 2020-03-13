//
//  TerrainShader.cpp
//  PixFu Engine
//
//  The Terrain Shader
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "TerrainShader.hpp"
#include "Camera.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace Pix {

	TerrainShader::TerrainShader(const std::string& name) : LightingShader(name) {
		LOC_TRANSFORMATIONMATRIX = getLocator("transformationMatrix");
		LOC_VIEWMATRIX = getLocator("viewMatrix");
		LOC_INVVIEWMATRIX = getLocator("invViewMatrix");
		LOC_PROJECTIONMATRIX = getLocator("projectionMatrix");
	}

	void TerrainShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
	}

	void TerrainShader::loadTransformationMatrix(glm::mat4 &matrix) {
		setMat4(LOC_TRANSFORMATIONMATRIX, (float *) &matrix);
	}

	void TerrainShader::loadViewMatrix(Camera *camera) {
		glm::mat4& viewMatrix = camera->getViewMatrix();
		setMat4(LOC_VIEWMATRIX, (float *) &viewMatrix);
		glm::mat4& invViewMatrix = camera->getInvViewMatrix();
		setMat4(LOC_INVVIEWMATRIX, (float *) &invViewMatrix);
	}

	void TerrainShader::loadProjectionMatrix(glm::mat4 &projection) {
		setMat4(LOC_PROJECTIONMATRIX, (float *) &projection);
	}


}
