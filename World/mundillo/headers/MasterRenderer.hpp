//
//  MasterRenderer.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef MasterRenderer_hpp
#define MasterRenderer_hpp

#include "../../headers/TerrainShader.hpp"
#include "TerrainRenderer.hpp"
#include "../../headers/Camera.hpp"

#include "OpenGL.h"

namespace rgl {
class MasterRenderer {
	
	const float FOV = 70;
	const float NEAR_PLANE = 0.1f;
	const float FAR_PLANE = 1000;
	
	glm::mat4 projectionMatrix;
		
	TerrainRenderer *pTerrainRenderer;
	TerrainShader *pTerrainShader;
		
//	private Map<TexturedModel,List<Entity>> entities = new HashMap<TexturedModel,List<Entity>>();
//	private List<Terrain> terrains = new ArrayList<Terrain>();
	
public:
	MasterRenderer(std::string shadername, int screenWidth, int screenHeight);
	
	void render(Light *sun, Camera *camera, Terrain *terrain);
	
	/*
	void processTerrain(Terrain *terrain);
	void processEntity(Entity *entity);
	*/
	
	void cleanUp();
	
	void prepare();
	
	void createProjectionMatrix(int screenWidth, int screenHeight);

};
}
#endif /* MasterRenderer_hpp */
