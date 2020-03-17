//
//  TerrainShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "Lighting.hpp"
#include "OpenGL.h"
#include "glm/vec4.hpp"

namespace Pix {

	class Frustum;

	class ObjectShader : public LightingShader {

		GLuint LOC_TINTMODE;

	public:

		ObjectShader(std::string name);

		void setTint(glm::vec4 tint);

	};

}
