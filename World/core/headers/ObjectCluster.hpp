//
//  ObjectCluster.hpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include "LayerVao.hpp"
#include "WorldMeta.hpp"
#include "ObjectShader.hpp"


namespace rgl {

class World;
class WorldObject;

typedef struct sVisible {
	WorldObject *object;
	glm::mat4 transformMatrix;
} Visible_t;

class ObjectCluster : public LayerVao {

	friend class World;
	
	static std::string TAG;

	Texture2D *pTexture;

	bool bInited = false;
	ObjLoader *pLoader;

	std::vector<Visible_t> vVisibles;
	std::vector<Texture2D*> vTextures;
	std::vector<WorldObject *> vInstances;
	glm::mat4 mPlacer;

public:

	const WorldConfig_t PLANET;
	const ObjectConfig_t PLACER;
	const std::string NAME;
	World *WORLD;

	ObjectCluster(World *planet, std::string name, ObjectConfig_t normalizeConfiguration = ObjectConfig_t());

	virtual ~ObjectCluster();

	void add(WorldObject *object, bool setHeight = true);

	void init();

	void render(ObjectShader *shader);
};

}
