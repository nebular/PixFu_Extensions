//
//  Lighting.hpp
//  PixEngine
//
//  Created by rodo on 12/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include <cmath>
#include "Shader.hpp"

namespace Pix {

	struct DirLight {

		glm::vec3 mDirection;
		const glm::vec3 mAmbient = {1,1,1};
		const glm::vec3 mDiffuse = {0.9,0.9,0.9};
		const glm::vec3 mSpecular= {0.8,0.8,0.8};
		float ka = 0.15;	// ambient light k

	public:
		void load(Shader *shader) const;
	};

	struct PointLight {
		glm::vec3 mPosition;
		const glm::vec3 mAmbient = {1.0,1.0,1.0};
		const glm::vec3 mDiffuse = {0.8,0.8,0.8};
		const glm::vec3 mSpecular= {1.0,1.0,1.0};
		/** Kept at 1 */
		const float constant = 1.0;
		/** Linear Attetnuation */
		const float linear = 5.8f;
		
		const float quadratic = 15.8;
		float ka = 0;	// ambient light k
		bool enabled = true;

	public:
		void load(Shader *shader, int index, bool enable=true) const;
		void update(Shader *shader, int index) const;
	};
	struct SpotLight {

		glm::vec3 mPosition;
		glm::vec3 mDirection;

		float cutOff = 5*M_PI/180;
		float outerCutOff = 15.5*M_PI/180;;
		
		const glm::vec3 mAmbient = {1.0,1.0,1.0};
		const glm::vec3 mDiffuse = {0.8,0.8,0.8};
		const glm::vec3 mSpecular= {1.0,1.0,1.0};
		/** Kept at 1 */
		const float constant = 1.0;
		
		/** Linear Attetnuation */
		const float linear = 5.09f;
		
		const float quadratic = 5.032;

		float ka = 0.01;	// ambient light k
		bool enabled = true;

	public:
		void load(Shader *shader, int index, bool enable=true) const;
		void update(Shader *shader, int index) const;
	};
}
