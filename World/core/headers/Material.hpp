//
//  Material.hpp
//  PixFu engine
//
//  A parsed OBJ Material
//  extracted from objl::OBJ_Loader
//
//  Created by rodo on 10/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include <string>
#include <vector>

#include "Texture2D.hpp"
#include "Shader.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace Pix {

	class Material {
		public:
		Material() {
			Ns = 0.0f;
			Ni = 0.0f;
			d = 0.0f;
			illum = 0;
		}

		// Material Name
		std::string name;
		// Ambient Color
		glm::vec3 Ka;
		// Diffuse Color
		glm::vec3 Kd;
		// Specular Color
		glm::vec3 Ks;
	
		/**
		 * Specular Exponent = "shininess".
		 * The higher the shininess value of an object, the more it properly reflects the
		 * light instead of scattering it all around and thus the smaller the highlight
		 * becomes.
		 */
		float Ns;
		
		// Optical Density
		float Ni;
		// Dissolve
		float d;
		// Illumination (code)
		int illum;
		// Ambient Texture Map
		std::string map_Ka;
		// Diffuse Texture Map. ONLY SUPPORTED.
		std::string map_Kd;
		// Specular Texture Map
		std::string map_Ks;
		// Specular Hightlight Map
		std::string map_Ns;
		// Alpha Texture Map
		std::string map_d;
		// Bump Map
		std::string map_bump;
		
		/** Animation Region is a rectangular region on the Texture to animate*/
		glm::vec4 AnR = {0,0,0,0};
		
		/** Animation Config has the deltas to apply and the last component to enable/disable */
		glm::vec3 AnC = {0,0,0};

		/** Material Texture, only Kd supported at the moment! */
		Texture2D *textureKd = nullptr;

		~Material();

		/** Initialize the material and load the texture from disk */
		void init(std::string NAME, std::string FOLDER="objects");
		/** Upload the texture to the gfx card */
		void upload();
		
	};

	inline Material::~Material() {
		if (textureKd != nullptr) {
			delete textureKd;
			textureKd = nullptr;
		}
	}

	inline void Material::upload() {
		if (textureKd != nullptr) textureKd->upload();
	}

}

