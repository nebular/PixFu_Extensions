//
//  Mundillo.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Mundillo.hpp"
#include "Utils.hpp"
#include "Texture2D.hpp"
#include "Drawable.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"
#include "rotate_vector.hpp"
#include "OpenGL.h"
#include "Terrain.hpp"
#include "Keyboard.hpp"
#include "MasterRenderer.hpp"

using namespace rgl;

std::string Mundillo::TAG="Mundillo";

Mundillo::Mundillo(std::string name) {
	
	// ground texture
	pGround = Drawable::fromFile(name+".png");
	
	// height map
	pHeight = Drawable::fromFile(name+".heights.png");
	
	int iMapWidth = pGround->width, iMapHeight = pGround->height;
	
	// populate ground texture with height (optimization for opengl shader)
	for (int y = 0; y<iMapHeight;y++)
		for (int x = 0; x<iMapWidth; x++) {
			Pixel ground = pGround->getPixel(x,y);
			Pixel height = pHeight->getPixel(x,y);
			pGround->setPixel(x,y,{ground.r, ground.g, ground.b, height.r});
		}
	
//	pTexture = new Texture2D(pGround);
//	pTexture->upload();
}

bool Mundillo::init(PixFu *engine) {

	pTerrain = new Terrain(pGround);
	pLight = new Light({20000,20000,2000}, {1,1,1});
	pCamera = new Camera();
	pRenderer = new MasterRenderer("world", engine->screenWidth(), engine->screenHeight());

	return true;
}

void Mundillo::tick(PixFu *engine, float fElapsedTime) {
	if (Keyboard::isHeld(Keys::UP)) pCamera->moveUp();
	if (Keyboard::isHeld(Keys::DOWN)) pCamera->moveDown();
	if (Keyboard::isHeld(Keys::LEFT)) pCamera->moveLeft();
	if (Keyboard::isHeld(Keys::RIGHT)) pCamera->moveRight();
	pRenderer->render(pLight, pCamera, pTerrain);
}
