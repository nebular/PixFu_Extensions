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
		LOC_TINTMODE = getLocator("tintMode");
	}

	void ObjectShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
	}
	
	void ObjectShader::setTint(glm::vec4 tint) {
		setVec4(LOC_TINTMODE, tint.x, tint.y, tint.z, tint.w);
	}

}

#pragma clang diagnostic pop
