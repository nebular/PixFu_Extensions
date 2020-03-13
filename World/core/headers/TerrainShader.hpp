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
		
		GLuint LOC_TRANSFORMATIONMATRIX;
		GLuint LOC_VIEWMATRIX;
		GLuint LOC_INVVIEWMATRIX;
		GLuint LOC_PROJECTIONMATRIX;

	public:

		TerrainShader(const std::string& name);

		void bindAttributes();

		void loadShineVariables(float damper, float reflectivity);

		void loadTransformationMatrix(glm::mat4 &matrix);

		void loadViewMatrix(Camera *camera);

		void loadProjectionMatrix(glm::mat4 &projection);
	};
}
