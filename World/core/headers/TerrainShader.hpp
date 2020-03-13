//
//  TerrainShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "Shader.hpp"
#include "Material.hpp"
#include "WorldMeta.hpp"
#include "Lighting.hpp"

#include "glm/mat4x4.hpp"
#include "OpenGL.h"

namespace Pix {

	class Camera;

	class Light;

	class TerrainShader : public LightingShader {
		
	public:

		TerrainShader(const std::string& name);

		void bindAttributes();

	};
}
