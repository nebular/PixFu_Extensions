//
//  World.hpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef MundiPeke_hpp
#define MundiPeke_hpp

#include "Layer.hpp"
#include "Surface.hpp"
#include "TerrainShader.hpp"
#include "Camera.hpp"
#include "Planet.hpp"

namespace rgl {


	typedef struct sTerrainConfig {
		const std::string name;
		const glm::vec2 origin = {0, 0};
	} TerrainConfig_t;

	class Terrain : public Layer {

		static std::string TAG = "Terrain";

		Texture2D *pTexture;

		bool bInited = false;

		void initMesh(uint vertexCount, uint twidth, uint theight);

		bool init(PixFu *engine);

		void tick(PixFu *engine, float fElapsedTime);


	public:

		const TerrainConfig_t CONFIG;
		const PlanetConfig_t PLANET;

		Terrain(PlanetConfig_t planetConfig, TerrainConfig_t config);

		void init();

		void render(TerrainShader *shader);
	};

}

#endif /* MundiPeke_hpp */
