//
//  Material.cpp
//  PixFu Engine
//
//  A Material for a WaveFront OBJ model
//  Includes proprietary extensions for (very simple) UV animation
//
//  Created by rodo on 10/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Material.hpp"

namespace Pix {

	void Material::init(std::string NAME, std::string FOLDER) {

		// strip filename from path, ignore path.
		auto getName = [](std::string path) {
			std::size_t found = path.find_last_of("/\\");
			return path.substr(found+1);
		};

		if (map_Kd.size()>0) {
			std::string filename = getName(map_Kd);
			textureKd = new Texture2D(FOLDER+"/" + NAME + "/"+filename, true);
		}

	}

	void Material::load(Shader *shader) {
		shader->setVec3("material.ambient", Ka.x, Ka.y, Ka.z);
		shader->setVec3("material.diffuse", Kd.x, Kd.y, Kd.z);
		shader->setVec3("material.specular", Ks.x, Ks.y, Ks.z);
		shader->setFloat("material.shininess", Ns);
		shader->setInt("material.illum", illum);
		shader->setVec4("material.animRegion", AnR.x, AnR.y, AnR.z, AnR.w);
		shader->setVec3("material.animConfig", AnC.x, AnC.y, AnC.z);
	}

	void Material::bind(Shader *shader) {
		if (textureKd != nullptr) {
			shader->setInt("material.hasTexture", 1);
			shader->textureUnit("materialTexture", textureKd);
			textureKd->bind();
		} else {
			shader->setInt("material.hasTexture", 0);
		}
	}

}
