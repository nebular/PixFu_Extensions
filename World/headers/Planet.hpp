//
// Created by rodo on 2020-02-17.
//

#ifndef PIXFU_ANDROID_PLANET_HPP
#define PIXFU_ANDROID_PLANET_HPP

#include "ext.hpp"
#include "TerrainShader.hpp"
#include "Camera.hpp"
#include <vector>

namespace rgl {


	typedef struct sPerspective {
		const float FOV;
		const float NEAR_PLANE;
		const float FAR_PLANE;
	} Perspective_t;

	typedef struct sPlanetConfig {
		const std::string shaderName = "world";
		const glm::vec3 lightPosition = {20000, 20000, 2000};
		const glm::vec3 lightColor = {0.8, 0.8, 0.93};
	} PlanetConfig_t;

	typedef struct sTerrainConfig {
		const std::string name;
		const glm::vec2 origin = {0, 0};
	} TerrainConfig_t;

	class Terrain : public Layer {

		static std::string TAG;

		Texture2D *pTexture;

		bool bInited = false;

		void initMesh(uint vertexCount, uint twidth, uint theight);

		bool init(PixFu *engine);

		void tick(PixFu *engine, float fElapsedTime);


	public:

		const TerrainConfig_t CONFIG;
		const PlanetConfig_t PLANET;

		Terrain(PlanetConfig_t planetConfig, TerrainConfig_t config);
		virtual ~Terrain();

		void init();

		void render(TerrainShader *shader);
	};

	class Planet : public PixFuExtension {

		static std::string TAG;

		TerrainShader *pShader;

		glm::mat4 projectionMatrix;

		Light *pLight;
		Camera *pCamera;

		std::vector<Terrain *> vTerrains;

		bool init(PixFu *engine);

		void tick(PixFu *engine, float fElapsedTime);


	public:

		const Perspective_t PERSPECTIVE;
		const PlanetConfig_t CONFIG;

		static constexpr Perspective_t PERSP_FOV90_LOW = {90, 0.005, 0.03};
		static constexpr Perspective_t PERSP_FOV90_MID = {90, 0.005, 100.0};
		static constexpr Perspective_t PERSP_FOV90_FAR = {90, 0.005, 1000.0};

		void add(TerrainConfig_t terrainConfig);

		Planet(PlanetConfig_t config, Perspective_t perspective = PERSP_FOV90_LOW);
		virtual ~Planet();
		Camera *camera();
	};

	inline Camera *Planet::camera() { return pCamera; }


}
#endif //PIXFU_ANDROID_PLANET_HPP
