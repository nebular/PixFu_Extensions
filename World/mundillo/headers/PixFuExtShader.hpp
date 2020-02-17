//
//  PixFuExtShader.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef PixFuExtShader_hpp
#define PixFuExtShader_hpp

#include "PixFu.hpp"
#include "Shader.hpp"
#include "glm.hpp"

namespace rgl {
class PixFUExtShader:public PixFuExtension {

protected:
	Shader *pShader;
public:
	PixFUExtShader(std::string shaderName);
	
	virtual ~PixFUExtShader();
};

inline PixFUExtShader::PixFUExtShader(std::string shaderName) {
	pShader = new Shader(shaderName);
}

inline PixFUExtShader::~PixFUExtShader() {
	delete pShader;
	pShader = nullptr;
}
}
#endif /* PixFuExtShader_hpp */
