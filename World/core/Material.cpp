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

}
