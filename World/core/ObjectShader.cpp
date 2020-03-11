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
	: Shader(name) {}

	void ObjectShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
	}

	// depecated
//	void ObjectShader::loadShineVariables(float damper, float reflectivity) {
//		setFloat("shineDamper", damper);
//		setFloat("reflectivity", reflectivity);
//	}

	void ObjectShader::loadTransformationMatrix(glm::mat4 &matrix) {
		setMat4("transformationMatrix", (float *) &matrix);
	}

	void ObjectShader::loadLight(const Light& light) {
		light.load(this);
	}

	void ObjectShader::loadViewMatrix(Camera *camera) {

		glm::mat4 viewMatrix = camera->getViewMatrix();
		setMat4("viewMatrix", (float *) &viewMatrix);

		glm::mat4& invViewMatrix = camera->getInvViewMatrix();
		setMat4("invViewMatrix", (float *) &invViewMatrix);

		mFrustum = new Frustum(mProjectionMatrix * viewMatrix);
	}

	void ObjectShader::loadProjectionMatrix(glm::mat4 &projection) {
		setMat4("projectionMatrix", (float *) &projection);
		mProjectionMatrix = projection;
		mFrustum = nullptr;
	}
	
	void ObjectShader::setTint(glm::vec4 tint) {
		setVec4("tintMode", tint.x, tint.y, tint.z, tint.w);
	}

}

#pragma clang diagnostic pop
