//
//  TerrainRenderer.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef TerrainRenderer_hpp
#define TerrainRenderer_hpp
#include "../../headers/TerrainShader.hpp"
#include "TerrainRenderer.hpp"
#include "../../headers/Camera.hpp"

#include "../../headers/TerrainShader.hpp"
#include "Terrain.hpp"

namespace rgl {
class TerrainRenderer {
	
	TerrainShader *pShader;
	
public:
	TerrainRenderer(TerrainShader *shader, glm::mat4 &projectionMatrix) ;
	
	void render(Terrain *terrain);
	
	void prepareTerrain(Terrain *terrain);
	
	void unbindTexturedModel();
	
	void loadModelMatrix(Terrain *terrain);
	
};

}

#endif /* TerrainRenderer_hpp */
