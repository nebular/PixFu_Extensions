//
//  Lighting.cpp
//  PixFu Engine
//
//  Directional, Spot and Point Lights.
//  Provides a base shader to extend that exports functions to update the lights
//
//  Implemented as taught in https://learnopengl.com/Lighting/Multiple-lights
//
//  Created by rodo on 12/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Lighting.hpp"
#include "Utils.hpp"

namespace Pix {
	
	LightingShader::LightingShader(const std::string& name, int maxLights)
	: WorldShader(name) {
		
		// cache all locators

		L_LIGHTMODE = getLocator("lightMode");

		// cache directional light locators

		DL_DIRECTION = getLocator("dirLight.direction");
		DL_AMBIENT = getLocator("dirLight.ambient");
		DL_DIFFUSE = getLocator("dirLight.diffuse");
		DL_SPECULAR = getLocator("dirLight.specular");

		for (int i = 0; i < maxLights; i++) {

			// cache pointlight locators
			// would really hurt a lot to do this string messing on every frame !!!
			
			std::string lex = std::string(SF("pointLights[%d]", i));
			PL_POSITION.emplace_back(getLocator(lex+".position"));
			PL_AMBIENT.emplace_back(getLocator(lex+".ambient"));
			PL_DIFFUSE.emplace_back(getLocator(lex+".diffuse"));
			PL_SPECULAR.emplace_back(getLocator(lex+".specular"));
			PL_PARAMS.emplace_back(getLocator(lex+".params"));
			PL_ENABLED.emplace_back(getLocator(lex+".enabled"));
			
			// cache spotlight locators

			lex = std::string(SF("spotLights[%d]", i));
			SL_POSITION.emplace_back(getLocator(lex+".position"));
			SL_DIRECTION.emplace_back(getLocator(lex+".direction"));
			SL_AMBIENT.emplace_back(getLocator(lex+".ambient"));
			SL_DIFFUSE.emplace_back(getLocator(lex+".diffuse"));
			SL_SPECULAR.emplace_back(getLocator(lex+".specular"));
			SL_CUTOFF.emplace_back(getLocator(lex+".cutOff"));
			SL_PARAMS.emplace_back(getLocator(lex+".params"));
			SL_ENABLED.emplace_back(getLocator(lex+".enabled"));
		}
	}

	void LightingShader::loadLight(const DirLight& light) const {
		setVec3(DL_DIRECTION, light.direction);
		setVec3(DL_AMBIENT,   light.color.ambient);
		setVec3(DL_DIFFUSE,   light.color.diffuse);
		setVec3(DL_SPECULAR,  light.color.specular);
	}

	void LightingShader::loadLight(PointLight& light, int index, bool enable) const {

		setInt(PL_ENABLED[index], enable?1:0);
		
		if (enable) {
			setVec3(PL_POSITION[index], light.position / 1000.0f);
			setVec3(PL_AMBIENT[index], 	light.color.ambient);
			setVec3(PL_DIFFUSE[index],  light.color.diffuse);
			setVec3(PL_SPECULAR[index], light.color.specular);
			setVec3(PL_PARAMS[index], 	light.params.constant, light.params.linear, light.params.quadratic);
		}
	}

	void LightingShader::loadLight(SpotLight& light, int index, bool enable) const {
		setInt(SL_ENABLED[index], light.enabled ? 1:0);
		if (enable) {
			setVec3(SL_POSITION[index],		light.position / 1000.0f);
			setVec3(SL_DIRECTION[index], 	light.direction);
			setVec3(SL_AMBIENT[index], 		light.color.ambient);
			setVec3(SL_DIFFUSE[index], 		light.color.diffuse);
			setVec3(SL_SPECULAR[index], 	light.color.specular);
			setVec3(SL_PARAMS[index], 		light.params.constant, light.params.linear, light.params.quadratic);
			setVec2(SL_CUTOFF[index], 		cosf(light.params.cutOff), cosf(light.params.outerCutOff));
		}
	}
}
