//
//  TerrainShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef TerrainShader_hpp
#define TerrainShader_hpp

#include "Shader.hpp"
#include "glm.hpp"

namespace rgl {


class Camera;
class Light;

	class TerrainShader : public Shader {

	public:
		TerrainShader(std::string name);

		void bindAttributes();

		void loadShineVariables(float damper, float reflectivity);

		void loadTransformationMatrix(glm::mat4 &matrix);

		void loadLight(Light *light);

		void loadViewMatrix(Camera *camera);

		void loadProjectionMatrix(glm::mat4 &projection);
	};
}
#endif /* TerrainShader_hpp */
