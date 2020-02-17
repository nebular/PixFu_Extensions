//
// Created by rodo on 2020-02-17.
//

#ifndef PIXFU_ANDROID_PLANET_HPP
#define PIXFU_ANDROID_PLANET_HPP

#include "ext.hpp"
#include "Layer.hpp"
#include "Surface.hpp"
#include "TerrainShader.hpp"
#include "Camera.hpp"
#include "Terrain.hpp""
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


	class Planet : public PixFuExtension {

		static std::string TAG;

		static constexpr Perspective_t PERSP_FOV90_LOW = {90, 0.005, 0.03};
		static constexpr Perspective_t PERSP_FOV90_MID = {90, 0.005, 100.0};
		static constexpr Perspective_t PERSP_FOV90_FAR = {90, 0.005, 1000.0};

		const Perspective_t PERSPECTIVE;
		const PlanetConfig_t CONFIG;

		TerrainShader *pShader;

		glm::mat4 projectionMatrix;

		Light *pLight;
		Camera *pCamera;

		std::vector<Terrain *> vTerrains;

		bool init(PixFu *engine);

		void tick(PixFu *engine, float fElapsedTime);

		void add(Terrain *terrain);

	public:

		Planet(PlanetConfig_t config, Perspective_t perspective = PERSP_FOV90_LOW);

		Camera *camera();
	};

	inline Camera *Planet::camera() { return pCamera; }


}
#endif //PIXFU_ANDROID_PLANET_HPP
