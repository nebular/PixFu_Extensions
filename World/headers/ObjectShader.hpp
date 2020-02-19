//
//  TerrainShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "NewCamera.hpp"
#include "Camera.hpp"
#include "Shader.hpp"
#include "glm.hpp"

namespace rgl {

	class ObjectShader : public Shader {

	public:
		ObjectShader(std::string name);

		void bindAttributes();

		void loadShineVariables(float damper, float reflectivity);

		void loadTransformationMatrix(glm::mat4 &matrix);

		void loadLight(Light *light);

		void loadViewMatrix(NewCamera *camera);

		void loadProjectionMatrix(glm::mat4 &projection);
	};
}
