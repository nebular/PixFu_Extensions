//
// Created by rodo on 2020-02-20.
//

#pragma once

#include "Ball.hpp"
#include "Meta.hpp"

namespace rgl {

	typedef  struct sWorldMeta {
		std::string name;
		int xMas, yMax;
	} WorldMeta_t;

	typedef  struct sPlayerFeatures {
		sObjectInfo info;
	} PlayerFeatures_t;

	class Player : public Ball {

		Player(int uid, PlayerFeatures_t features, WorldMeta_t worldMeta)
				: Ball(uid, features.info.radius, features.info.mass,  worldMeta.xMas, worldMeta.yMax) {}

	};
	class GameObject : public Ball {

		GameObject(int uid, PlayerFeatures_t features, WorldMeta_t worldMeta)
				: Ball(uid, features.info.radius, features.info.mass,  worldMeta.xMas, worldMeta.yMax) {}

	};



}
