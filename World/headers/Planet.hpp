//
// Created by rodo on 2020-02-17.
//

#pragma once

#include "ext.hpp"
#include "TerrainShader.hpp"
#include "ObjectShader.hpp"
#include "Camera.hpp"
#include "NewCamera.hpp"
#include "ObjectLoader.hpp"
#include <vector>
#include <map>
#include <cmath>

namespace rgl {

typedef struct sPerspective {
	const float FOV;
	const float NEAR_PLANE;
	const float FAR_PLANE;
	const float C_HEIGHT = 0.2;
	const float C_PITCH = 0;
} Perspective_t;

typedef struct sPlanetConfig {
	const std::string shaderName = "world";
	const glm::vec3 lightPosition = {20000, 20000, 2000};
	const glm::vec3 lightColor = {0.8, 0.8, 0.93};
} PlanetConfig_t;

typedef struct sObjectConfig {
	glm::vec3 position = {0, 0, 0};
	glm::vec3 rotation = {0, 0, 0};
	float scale = 0.25;
	const float mass = 1.0;
} ObjectConfig_t;

typedef struct sTerrainConfig {
	const std::string name;
	const glm::vec2 origin = {0, 0};
} TerrainConfig_t;

//////////////////////////////////////////////////////////////////////////////////////////


class Object : public Layer {
	
	static std::string TAG;
	
	Texture2D *pTexture;
	
	bool bInited = false;
	ObjectLoader *pLoader;

	std::vector<ObjectConfig_t> vInstances;
	glm::mat4 mPlacer;
	
public:
	
	const PlanetConfig_t PLANET;
	const ObjectConfig_t PLACER;
	const std::string NAME;
	
	Object(std::string name, PlanetConfig_t planetConfig, ObjectConfig_t normalizeConfiguration=ObjectConfig_t());
	virtual ~Object();

	void add(ObjectConfig_t &config);

	void init();
	
	void render(ObjectShader *shader);
};

//////////////////////////////////////////////////////////////////////////////////////////

class Terrain : public Layer {
	
	static std::string TAG;
	
	static constexpr float VERTICES[32]{
			// positions          // colors           // texture coords
			 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
			 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
			-1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
	};

	static constexpr unsigned int INDICES[6]{
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
	};

	Texture2D *pTexture;
	ObjectLoader *pLoader;

	bool bInited = false;
	
	void initMesh(uint vertexCount, uint twidth, uint theight);
	
public:
	
	const TerrainConfig_t CONFIG;
	const PlanetConfig_t PLANET;
	
	Terrain(PlanetConfig_t planetConfig, TerrainConfig_t config);
	
	virtual ~Terrain();
	
	void init();
	
	void render(TerrainShader *shader);
};


//////////////////////////////////////////////////////////////////////////////////////////
class Grid;

class Planet : public PixFuExtension {
	
	static std::string TAG;
	
	TerrainShader *pShader;
	ObjectShader *pShaderObjects;
	
	glm::mat4 projectionMatrix;
	
	Light *pLight;
	NewCamera *pCamera;
	Grid *pGrid;
	
	std::vector<Terrain *> vTerrains;
	std::vector<Object *> vObjects;
	std::map<std::string, Object *> mCluesters;
	
	bool init(PixFu *engine);
	
	void tick(PixFu *engine, float fElapsedTime);
	
	
public:
	
	const Perspective_t PERSPECTIVE;
	const PlanetConfig_t CONFIG;
	

	//		static constexpr Perspective_t PERSP_FOV90_LOW = {90, 0.005, 0.03};
	static constexpr Perspective_t PERSP_FOV90_LOW = {90, 0.005, 0.1,   0.25};
	static constexpr Perspective_t PERSP_FOV90_MID = {90, 0.005, 100.0, 0.25};
	static constexpr Perspective_t PERSP_FOV90_FAR = {90, 0.5, 1000.0,  0.25};
	static constexpr Perspective_t PERSP_FOV60_LOW = {90, 0.005, 0.1,   0.25};
	static constexpr Perspective_t PERSP_FOV60_MID = {70, 0.1, 1000.0,  0.25};
	static constexpr Perspective_t PERSP_FOV60_FAR = {60, 0.5, 1000.0,  0.25};
	
	Planet(PlanetConfig_t config, Perspective_t perspective = PERSP_FOV90_LOW);
	
	virtual ~Planet();
	
	
	void add(TerrainConfig_t terrainConfig);
	
	void add(const char *name, ObjectConfig_t objectConfig,  ObjectConfig_t initialTransform = ObjectConfig_t());

	NewCamera *camera();
};

inline NewCamera *Planet::camera() { return pCamera; }

class Grid:public VaoLayer {
public:
	Grid(int width, int height);

};

}
