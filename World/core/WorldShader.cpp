//
//  WorldShader.cpp
//  PixEngine
//
//  A 3d + materials shader
//
//  Created by rodo on 13/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "WorldShader.hpp"
#include "Camera.hpp"

namespace Pix {

	WorldShader::WorldShader(const std::string& name)
	: Shader3D(name) {
	
		// material locators
		LOC_MAT_ILLUM =getLocator("material.illum");
		LOC_MAT_AMBIENT =getLocator("material.ambient");
		LOC_MAT_DIFFUSE =getLocator("material.diffuse");
		LOC_MAT_SPECULAR =getLocator("material.specular");
		LOC_MAT_SHININESS =getLocator("material.shininess");
		LOC_MAT_ANIMREGION =getLocator("material.animRegion");
		LOC_MAT_ANIMCONFIG =getLocator("material.animConfig");
		LOC_MAT_HASTEXTURE =getLocator("material.hasTexture");
		LOC_MAT_TEXTURE =getLocator("materialTexture");
	}

	void WorldShader::loadMaterial(Material& m) {
		setVec3 (LOC_MAT_AMBIENT,    m.Ka.x, m.Ka.y, m.Ka.z);
		setVec3 (LOC_MAT_DIFFUSE,    m.Kd.x, m.Kd.y, m.Kd.z);
		setVec3 (LOC_MAT_SPECULAR,   m.Ks.x, m.Ks.y, m.Ks.z);
		setFloat(LOC_MAT_SHININESS,  m.Ns);
		setInt  (LOC_MAT_ILLUM, 	 m.illum);
		setVec4 (LOC_MAT_ANIMREGION, m.AnR.x, m.AnR.y, m.AnR.z, m.AnR.w);
		setVec3 (LOC_MAT_ANIMCONFIG, m.AnC.x, m.AnC.y, m.AnC.z);
	}

	void WorldShader::bindMaterial(Material& m) {
		if (m.textureKd != nullptr) {
			setInt(LOC_MAT_HASTEXTURE, 1);
			textureUnit(LOC_MAT_TEXTURE, m.textureKd);
			m.textureKd->bind();
		} else {
			setInt(LOC_MAT_HASTEXTURE, 0);
		}
	}

}
