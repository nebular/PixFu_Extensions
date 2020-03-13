//
//  TerrainShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "Camera.hpp"
#include "Shader.hpp"
#include "Material.hpp"

#include "OpenGL.h"
#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"

namespace Pix {

	class Frustum;

	class ObjectShader : public LightingShader {

		GLuint LOC_TRANSFORMATIONMATRIX;
		GLuint LOC_VIEWMATRIX;
		GLuint LOC_INVVIEWMATRIX;
		GLuint LOC_PROJECTIONMATRIX;
		GLuint LOC_TINTMODE;

		// keey this for frustum calculations
		glm::mat4 mProjectionMatrix;

		// we wil evaluate each draw() to check if inside the frustum
		Frustum *mFrustum;

	public:

		ObjectShader(std::string name);

		void bindAttributes();

		void loadTransformationMatrix(glm::mat4 &matrix);

		void loadViewMatrix(Camera *camera);

		void loadProjectionMatrix(glm::mat4 &projection);

		void setTint(glm::vec4 tint);

		Frustum *frustum() { return mFrustum; }
	};
}
