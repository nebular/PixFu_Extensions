//
//  World.hpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef MundiPeke_hpp
#define MundiPeke_hpp

#include "ext.hpp"
#include "Layer.hpp"
#include "Surface.hpp"
#include "TerrainShader.hpp"
#include "Camera.hpp"
#include "Planet.hpp"

namespace rgl {

	typedef struct sWorldConfig {
		const std::string name;
		const std::string shaderName = "world";
		const glm::vec3 lightPosition = {20000, 20000, 2000};
		const glm::vec3 lightColor = {0.8, 0.8, 0.93};
		const glm::vec2 origin = { 0,0 };
	} WorldConfig_t;


	class World : public PixFuExtension, public Layer {

		const Perspective_t PERSPECTIVE;
		const WorldConfig_t CONFIG;

		glm::mat4 projectionMatrix;

		TerrainShader *pShader;
		Texture2D *pTexture;

		Light *pLight;
		Camera *pCamera;

		void initMesh(uint vertexCount, uint twidth, uint theight);

		bool init(PixFu *engine);

		void tick(PixFu *engine, float fElapsedTime);


	public:

		World(WorldConfig_t config, Perspective_t perspective = Planet::PERSP_FOV90_LOW);

		Camera *camera();
	};

	inline Camera *World::camera() { return pCamera; }
}

#endif /* MundiPeke_hpp */
