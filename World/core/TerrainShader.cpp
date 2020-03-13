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


	TerrainShader::TerrainShader(const std::string& name) : LightingShader(name) {}

	void TerrainShader::bindAttributes() {
		bindAttribute(0, "position");
		bindAttribute(1, "normal");
		bindAttribute(2, "textureCoordinates");
	}

}
