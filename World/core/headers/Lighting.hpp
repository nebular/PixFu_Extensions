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

	/** Light color parameters */
	typedef struct sLightColor {

		/** Ambient Color */
		const glm::vec3 ambient = {0.15F,0.15F,0.15F};
		/** Diffuse Color */
		const glm::vec3 diffuse = {0.9F,0.9F,0.9F};
		/** Specular Color */
		const glm::vec3 specular= {0.8F,0.8F,0.8F};
		
	} LightColor_t;

	// some LightColor preset
	constexpr LightColor_t LIGHTCOLOR_WHITE_COLD_NOAMBIENT = { {0,0,0}, {0.9F,0.9F, 0.9F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_WHITE_COLD_AMBIENTLO = { {0.05,0.05,0.05}, {0.9F,0.9F, 0.9F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_WHITE_COLD_AMBIENTMI = { {0.10,0.10,0.10}, {0.9F,0.9F, 0.9F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_WHITE_COLD_AMBIENTHI = { {0.15,0.15,0.15}, {0.9F,0.9F, 0.9F}, {0.7F, 0.7F, 0.7F}};

	constexpr LightColor_t LIGHTCOLOR_WHITE_WARM_NOAMBIENT = { {0,0,0}, 		 {1.0F, 1.0F, 0.8F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_WHITE_WARM_AMBIENTLO = { {0.05,0.05,0.05}, {1.0F, 1.0F, 0.8F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_WHITE_WARM_AMBIENTMI = { {0.10,0.10,0.10}, {1.0F, 1.0F, 0.8F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_WHITE_WARM_AMBIENTHI = { {0.15,0.15,0.15}, {1.0F, 1.0F, 0.8F}, {0.7F, 0.7F, 0.7F}};

	constexpr LightColor_t LIGHTCOLOR_RED_NOAMBIENT = { {0,0,0}, {0.9F,0.0f,0.0F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_RED_AMBIENTLO = { {0.05,0.05,0.05}, {0.9F,0.0f,0.0F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_RED_AMBIENTMI = { {0.10,0.10,0.10}, {0.9F,0.0f,0.0F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_RED_AMBIENTHI = { {0.15,0.15,0.15}, {0.9F,0.0f,0.0F}, {0.7F, 0.7F, 0.7F}};

	constexpr LightColor_t LIGHTCOLOR_GREEN_NOAMBIENT = { {0,0,0}, 			{0.0F, 0.9F,0.0F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_GREEN_AMBIENTLO = { {0.05,0.05,0.05}, {0.0F, 0.9F,0.0F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_GREEN_AMBIENTMI = { {0.10,0.10,0.10}, {0.0F, 0.9F,0.0F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_GREEN_AMBIENTHI = { {0.15,0.15,0.15}, {0.0F, 0.9F,0.0F}, {0.7F, 0.7F, 0.7F}};

	constexpr LightColor_t LIGHTCOLOR_BLUE_NOAMBIENT = { {0,0,0}, 		   {0.0F,0.0f,0.9F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_BLUE_AMBIENTLO = { {0.05,0.05,0.05}, {0.0F,0.0f,0.9F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_BLUE_AMBIENTMI = { {0.10,0.10,0.10}, {0.0F,0.0f,0.9F}, {0.5F, 0.5F, 0.5F}};
	constexpr LightColor_t LIGHTCOLOR_BLUE_AMBIENTHI = { {0.15,0.15,0.15}, {0.0F,0.0f,0.9F}, {0.7F, 0.7F, 0.7F}};

	/** PointLight parameters */
	typedef struct sPointLightParams {
		
		/** "Kept at 1" they say ! :S */
		const float constant = 1.0F;
		/** Linear Attenuation (r/distance) */
		const float linear = 15.8F;
		/** Quadratic Attenuation (r/distance) */
		const float quadratic = 50.8F;
		
	} PointLightParams_t;

	// some pointlight presets
	constexpr PointLightParams_t POINTLIGHT_SMALL = { 1.0F, 15.8F, 50.0F};
	constexpr PointLightParams_t POINTLIGHT_MEDIUM = { 1.0F, 10.8F, 20.0F};
	constexpr PointLightParams_t POINTLIGHT_BIG = { 1.0F, 5.8F, 3.0F};

	/** Spotlight parameters */
	typedef struct sSpotLightParams {
		/** "Kept at 1" they say ! :S */
		const float constant = 1.0F;
		/** Linear Attenuation (r/distance) */
		const float linear = 15.8F;
		/** Quadratic Attenuation (r/distance) */
		const float quadratic = 50.8F;
		/** Light Cutoff - this is a cooked cosine value  */
		const float cutOff = 45*M_PI/180;
		/** Light Outer CutOff (not used yet) */
		const float outerCutOff = 50*M_PI/180;;
	} SpotLightParams_t;

	// some spotlight presets
	constexpr SpotLightParams_t SPOTLIGHT_SMALL  = { 1.0F, 15.8F, 50.0F, 30*M_PI/180, 35*M_PI/180};
	constexpr SpotLightParams_t SPOTLIGHT_MEDIUM = { 1.0F, 10.8F, 20.0F, 45*M_PI/180, 50*M_PI/180};
	constexpr SpotLightParams_t SPOTLIGHT_BIG    = { 1.0F, 5.8F, 3.0F, 60*M_PI/180, 65*M_PI/180};

	/**
	 * A directional light
	 */

	struct DirLight {
		/** color properties */
		const LightColor_t color = LIGHTCOLOR_WHITE_COLD_AMBIENTHI;
		/** Light Direction */
		const glm::vec3 direction;
	};

	/**
	 * A PointLight, like an omnidirectional bulb
	 */

	struct PointLight {

		/** Color parameters */
		const LightColor_t color = LIGHTCOLOR_WHITE_COLD_AMBIENTMI;

		/** PointLight parameters */
		const PointLightParams_t params = POINTLIGHT_SMALL;
		
		/** Light Position */
		glm::vec3 position;

		/** Whether */
		bool enabled = true;
		
		float calcRadius(float targetAttenuation) const {
			// 1/att =  (constant + linear * distance + quadratic * (distance2);
			// 1/att = qx2 + lx + c
			// qx2 + lx + (c - 1/a) = 0
			// qx2 + lx +     d     = 0
			// x = -l + sqrt((l*2 - 4*q*(c-1/a)))/(2 *q)
			float d = params.constant - 1/targetAttenuation;
			return  ( -params.linear + sqrt( params.linear*params.linear - 4 * params.quadratic * d  )) / 2 * params.quadratic;
		}

	};

	/**
	 * Spotlight with a cutoff angle
	 */

	struct SpotLight {

		/* Light properties: ambient, diffuse, specular */
		const LightColor_t color = LIGHTCOLOR_WHITE_COLD_NOAMBIENT;
		
		/** Parameters */
		const SpotLightParams_t params = SPOTLIGHT_SMALL;

		/** Light Position */
		glm::vec3 position;
		/** Light Direction */
		glm::vec3 direction;
		/** Whether */
		bool enabled = true;
		
		float calcRadius(float targetAttenuation) const {

			// 1/att =  (constant + linear * distance + quadratic * (distance2);
			// 1/att = qx2 + lx + c
			// qx2 + lx + (c - 1/a) = 0
			// qx2 + lx +     d     = 0
			// x = -l + sqrt((l*2 - 4*q*(c-1/a)))/(2 *q)

			float d = params.constant - 1/targetAttenuation;
			return ( -params.linear + sqrt( params.linear*params.linear - 4 * params.quadratic * d  )) / 2 * params.quadratic;
		}
	};

	constexpr DirLight DIRLIGHT_AFTERNOON = {
		LIGHTCOLOR_WHITE_WARM_AMBIENTHI,
		{1.0F,-1.0F,1.0F}			// rays direction
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
		setInt(SL_ENABLED[index], enable?1:0);
	}

	inline void LightingShader::enablePointLight(int index, bool enable) const {
		setInt(PL_ENABLED[index], enable?1:0);
	}


}
