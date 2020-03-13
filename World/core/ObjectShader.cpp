//
//  TerrainShader.cpp
//  PixFu Engine
//
//  A Shader for the world Objects.
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ObjectShader.hpp"
#include "Frustum.hpp"
#include "Material.hpp"

#include "glm/gtc/matrix_inverse.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

namespace Pix {

	ObjectShader::ObjectShader(std::string name)
	: LightingShader(name) {
		// cache locators
		LOC_TRANSFORMATIONMATRIX = getLocator("transformationMatrix");
		LOC_VIEWMATRIX = getLocator("viewMatrix");
		LOC_INVVIEWMATRIX = getLocator("invViewMatrix");
		LOC_PROJECTIONMATRIX = getLocator("projectionMatrix");
		LOC_TINTMODE = getLocator("tintMode");
	}

	void ObjectShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
	}

	void ObjectShader::loadTransformationMatrix(glm::mat4 &matrix) {
		setMat4(LOC_TRANSFORMATIONMATRIX, (float *) &matrix);
	}

	void ObjectShader::loadViewMatrix(Camera *camera) {

		glm::mat4 viewMatrix = camera->getViewMatrix();
		setMat4(LOC_VIEWMATRIX, (float *) &viewMatrix);

		glm::mat4& invViewMatrix = camera->getInvViewMatrix();
		setMat4(LOC_INVVIEWMATRIX, (float *) &invViewMatrix);

		mFrustum = new Frustum(mProjectionMatrix * viewMatrix);
	}

	void ObjectShader::loadProjectionMatrix(glm::mat4 &projection) {
		setMat4(LOC_PROJECTIONMATRIX, (float *) &projection);
		mProjectionMatrix = projection;
		mFrustum = nullptr;
	}
	
	void ObjectShader::setTint(glm::vec4 tint) {
		setVec4(LOC_TINTMODE, tint.x, tint.y, tint.z, tint.w);
	}

}

#pragma clang diagnostic pop
