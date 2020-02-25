//
//  WorldMeta.hpp
//  The structs that hold world metadata
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include <string>
#include "glm.hpp"

namespace rgl {

	// Perspective presets (with initial camera height / pitch)
	typedef struct sPerspective {
		const float FOV;
		const float NEAR_PLANE;
		const float FAR_PLANE;
		const float C_HEIGHT = 0.2;
		const float C_PITCH = 0;
	} Perspective_t;

	// World configuration
	typedef struct sWorldConfig {
		// determines which shader to use (assets)
		const std::string shaderName = "world";
		// global background color
		const glm::vec3 backgroundColor = {0.8, 0.8, 1};
		// light position and color
		const glm::vec3 lightPosition = {20000, 20000, 2000};
		const glm::vec3 lightColor = {0.8, 0.8, 0.93};
		// whether to create a 3d canvas to draw over the terrain
		const bool withCanvas = true;
	} WorldConfig_t;

	// determines  initial object properties
	typedef struct sObjectConfig {
		glm::vec3 position = {0, 0, 0};
		glm::vec3 rotation = {0, 0, 0};
		float radius = 1.0;
		float mass = 1000.0;
	} ObjectConfig_t;

	// defines an initiali transforation for the world and objects
	typedef struct sTransformation {
		glm::vec3 position = {0, 0, 0};
		glm::vec3 rotation = {0, 0, 0};
		float scale = 1.0;
		bool flipx = false, flipy = false, flipz = false;
	} Transformation_t;

	// Definition of a terrain
	typedef struct sTerrainConfig {
		// determines also where the resources are
		const std::string name;
		// world can hold several terrains
		const glm::vec2 origin = {0, 0};
		// sync Heightmap heights with the terrain. Terrain has to be pre-scaled, this is just only
		// to sync the heightmap to whatever heght scale the terrain was created with.
		const float scaleHeight = 0.1;
	} TerrainConfig_t;

// base class for any object in the world

	class WorldObject {
	public:
		const std::string CLASS;

		inline WorldObject(std::string objectClass) : CLASS(objectClass) {}

		inline virtual ~WorldObject() = default;

		// normalized position
		virtual glm::vec3 pos() = 0;

		// rotation
		virtual glm::vec3 rot() = 0;

		// radius
		virtual float radius() = 0;
	};

}
