//
//  Mundillo.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef Mundillo_hpp
#define Mundillo_hpp

#include "PixFuExtShader.hpp"
#include "Terrain.hpp"
#include "MasterRenderer.hpp"

using namespace rgl;

class Mundillo:public Layer {
	
	Texture2D *pTexture;
	Drawable *pGround, *pHeight;
	Terrain *pTerrain;
	
	Camera *pCamera;
	Light *pLight;
	MasterRenderer *pRenderer;

public:
	static std::string TAG;
	
	Mundillo(std::string mapName);

	void initGl(GLuint *indices, GLfloat *vertices, int num1, int num2);

	bool init(PixFu *engine) override;

	void tick(PixFu *engine, float fElapsedTime) override;
	
};



#endif /* Mundillo_hpp */
