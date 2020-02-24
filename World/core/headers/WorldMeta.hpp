//
//  WorldMeta.hpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include <string>
#include "glm.hpp"

namespace rgl {

typedef struct sPerspective {
	const float FOV;
	const float NEAR_PLANE;
	const float FAR_PLANE;
	const float C_HEIGHT = 0.2;
	const float C_PITCH = 0;
} Perspective_t;

typedef struct sWorldConfig {
	const std::string shaderName = "world";
	const glm::vec3 backgroundColor = {0.8, 0.8, 1};
	const glm::vec3 lightPosition = {20000, 20000, 2000};
	const glm::vec3 lightColor = {0.8, 0.8, 0.93};
} WorldConfig_t;

typedef struct sObjectConfig {
	glm::vec3 position = {0, 0, 0};
	glm::vec3 rotation = {0, 0, 0};
	float radius = 1.0;
	float mass = 1000.0;
} ObjectConfig_t;

typedef struct sTerrainConfig {
	const std::string name;
	const glm::vec2 origin = {0, 0};
	const float scaleHeight = 0.1;
} TerrainConfig_t;


}
