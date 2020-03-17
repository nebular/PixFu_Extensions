//
//  TerrainShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "Lighting.hpp"

namespace Pix {


	class TerrainShader : public LightingShader {
		
	public:

		inline TerrainShader(const std::string& name) : LightingShader(name) {}

	};
}
