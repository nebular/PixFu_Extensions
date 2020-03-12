//
//  Lighting.cpp
//  PixEngine
//
//  Created by rodo on 12/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Lighting.hpp"
#include "Utils.hpp"

namespace Pix {

	void PointLight::load(Shader *shader, int index, bool enable) const {
		std::string lex = std::string(SF("pointLights[%d]", index));
		shader->setInt(lex+".enabled", enable?1:0);
		if (enable) {
			shader->setVec3(lex+".position", mPosition / 1000.0f);
			shader->setVec3(lex+".ambient", mAmbient);
			shader->setVec3(lex+".diffuse",  mDiffuse);
			shader->setVec3(lex+".specular", mSpecular);
			shader->setFloat(lex+".constant", constant);
			shader->setFloat(lex+".quadratic", quadratic);
			shader->setFloat(lex+".linear", linear);
			shader->setFloat(lex+".ka", ka);
		}
	}

	void PointLight::update(Shader *shader, int index) const {
		std::string lex = std::string(SF("pointLights[%d]", index));
		shader->setVec3(lex+".position", mPosition);
		shader->setFloat(lex+".ka", ka); // interesting for animation
	}


	void SpotLight::load(Shader *shader, int index, bool enable) const {
		std::string lex = std::string(SF("spotLights[%d]", index));
		shader->setInt(lex+".enabled", enable?1:0);
		if (enable) {
			shader->setVec3(lex+".position", mPosition / 1000.0f);
			shader->setVec3(lex+".direction", mDirection);
			shader->setVec3(lex+".ambient", mAmbient);
			shader->setVec3(lex+".diffuse",  mDiffuse);
			shader->setVec3(lex+".specular", mSpecular);
			shader->setFloat(lex+".constant", constant);
			shader->setFloat(lex+".quadratic", quadratic);
			shader->setFloat(lex+".linear", linear);
			shader->setFloat(lex+".cutOff", cutOff);
			shader->setFloat(lex+".outerCutOff", outerCutOff);
			shader->setFloat(lex+".ka", ka);
		}
	}

	void SpotLight::update(Shader *shader, int index) const {
		std::string lex = std::string(SF("spotLights[%d]", index));
		shader->setVec3(lex+".position", mPosition);
		shader->setFloat(lex+".ka", ka); // interesting for animation
	}

}
