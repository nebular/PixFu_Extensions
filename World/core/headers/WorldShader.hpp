//
//  WorldShader.hpp
//  PixEngine
//
//  Created by rodo on 13/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include "Shader.hpp"
#include "Material.hpp"

namespace Pix {

	class Camera;
	class WorldShader : public Shader3D {

	protected:

		GLuint LOC_MAT_AMBIENT;
		GLuint LOC_MAT_DIFFUSE;
		GLuint LOC_MAT_SPECULAR;
		GLuint LOC_MAT_SHININESS;
		GLuint LOC_MAT_ILLUM;
		GLuint LOC_MAT_ANIMREGION;
		GLuint LOC_MAT_ANIMCONFIG;
		GLuint LOC_MAT_HASTEXTURE;
		GLuint LOC_MAT_TEXTURE;

		
	public:

		WorldShader (const std::string& name);
		void loadMaterial(Material& m);
		void bindMaterial(Material& m);

	};

}
