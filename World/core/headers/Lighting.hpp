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

#pragma once
#include <cmath>
#include "WorldShader.hpp"
#include "glm/vec3.hpp"

namespace Pix {

	typedef enum eLightMode { LIGHTS_OFF, LIGHTS_ON} LightMode_t;

	/**
	 * A directional light
	 */
	struct DirLight {

		/** Light Direction */
		glm::vec3 direction;
		
		/** Ambient Color */
		const glm::vec3 ambient = {0.15F,0.15F,0.15F};
		/** Diffuse Color */
		const glm::vec3 diffuse = {0.9F,0.9F,0.9F};
		/** Specular Color */
		const glm::vec3 specular= {0.8F,0.8F,0.8F};

	};

	struct PointLight {
		/** Light Position */
		glm::vec3 position;
		/** Ambient Color */
		const glm::vec3 ambient = {0.1F,0.1F,0.1F};
		/** Diffuse Color */
		const glm::vec3 diffuse = {0.8F,0.8F,0.8F};
		/** Specular Color */
		const glm::vec3 specular= {1.0F,1.0F,1.0F};
		/** "Kept at 1" they say ! :S */
		const float constant = 1.0F;
		/** Linear Attenuation (r/distance) */
		const float linear = 5.8F;
		/** Quadratic Attenuation (r/distance) */
		const float quadratic = 15.8F;
		/** Whether */
		bool enabled = true;
	};

	struct SpotLight {

		/** Light Position */
		glm::vec3 position;
		/** Light Direction */
		glm::vec3 direction;

		/** Light Cutoff - this is a cooked cosine value  */
		float cutOff = 50*M_PI/180;
		/** Light Outer CutOff (not used yet) */
		float outerCutOff = 15.5*M_PI/180;;
		
		/** Ambient Color */
		const glm::vec3 ambient = {0.05F,0.05F,0.05F};
		/** Diffuse Color */
		const glm::vec3 diffuse = {0.8F,0.8F,0.8F};
		/** Specular Color */
		const glm::vec3 specular= {1.0F,1.0F,1.0F};
		
		/** Kept at 1 */
		const float constant = 1.0F;
		
		/** Linear Attetnuation */
		const float linear = 5.09F;
		/** Quadratic Attetnuation */
		const float quadratic = 5.032F;
		/** Whether */
		bool enabled = true;
	};
	
	constexpr DirLight DIRLIGHT_AFTERNOON = {
		{1.0F,-1.0F,1.0F},			// rays direction
		{0.15F,0.15F,0.15F},		// 15% ambient
		{1.0F,1.0F,1.0F},			// diffuse
		{0.5F,0.5F,0.5F}			// specular
	};


	class LightingShader : public WorldShader {
		
		// cached locators for quickest update
		
		GLuint L_LIGHTMODE;
			
		GLuint DL_DIRECTION;
		GLuint DL_AMBIENT;
		GLuint DL_SPECULAR;
		GLuint DL_DIFFUSE;

		// cached pointlight locators, will be a lot of them !
		std::vector<GLuint> PL_POSITION;
		std::vector<GLuint> PL_AMBIENT;
		std::vector<GLuint> PL_DIFFUSE;
		std::vector<GLuint> PL_SPECULAR;
		std::vector<GLuint> PL_PARAMS;
		std::vector<GLuint> PL_ENABLED;

		// cached spotlight locators, will be a lot of them !
		std::vector<GLuint> SL_POSITION;
		std::vector<GLuint> SL_DIRECTION;
		std::vector<GLuint> SL_AMBIENT;
		std::vector<GLuint> SL_DIFFUSE;
		std::vector<GLuint> SL_SPECULAR;
		std::vector<GLuint> SL_PARAMS;
		std::vector<GLuint> SL_CUTOFF;
		std::vector<GLuint> SL_ENABLED;

	public:
		
		LightingShader(const std::string& name, int maxLights = 4);
		/** Loads the directional light */
		void loadLight(const DirLight& light) const;
		/** Loads a spotlight */
		void loadLight(SpotLight& light, int index, bool enable) const;
		/** Loads a pointlight */
		void loadLight(PointLight& light, int index, bool enable) const;
		/** Updates a spotlight */
		void updateLight(SpotLight& light, int index) const;
		/** Updates a pointlight */
		void updateLight(PointLight& light, int index) const;
		/** Enables/disables a spotlight (pre-configured, added, etc...) */
		void enableSpotLight(int index, bool enable) const;
		/** Enables/disables a pointlight (pre-configured, added, etc...) */
		void enablePointLight(int index, bool enable) const;
		/** Sets lighting mode */
		void setLightingMode(LightMode_t lightMode) const;
	};

	// this is a per-frame function so lets try to make it faster
	inline void LightingShader::updateLight(PointLight& p, int index) const {
		setVec3(PL_POSITION[index], p.position/1000.0f);
	}


	// this is a per-frame function so lets try to make it faster
	inline void LightingShader::updateLight(SpotLight& s, int index) const {
		setVec3(SL_POSITION[index], s.position/1000.0f);
		setVec3(SL_DIRECTION[index], s.direction);
	}

	inline void LightingShader::setLightingMode(LightMode_t lightMode) const {
		setInt(L_LIGHTMODE, lightMode);
	}

	inline void LightingShader::enableSpotLight(int index, bool enable) const {
		setInt(PL_ENABLED[index], enable?1:0);
	}

	inline void LightingShader::enablePointLight(int index, bool enable) const {
		setInt(PL_ENABLED[index], enable?1:0);
	}


}
