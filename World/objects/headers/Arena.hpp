//
//  Arena.hpp
//  PixEngine
//
//  Created by rodo on 21/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include <vector>
#include "World.hpp"
#include "ArenaLevel.hpp"

namespace rgl {

class Ball;
class PixFu;
class GameObject;
class Player;


class Arena : public World {
	
	static std::string TAG;
	
	std::vector<Ball *> vFakeBalls;
	std::vector <std::pair<Ball *, Ball *>> vCollidingPairs;
	std::vector <std::pair<Ball *, Ball *>> vFutureColliders;

	Player *pHumanPlayer = nullptr, *pCamPlayer = nullptr;
	const ArenaLevel_t *LEVEL;
	float fPos=0, fMetronome=0;
public:
	
	Arena(std::string name, WorldConfig_t config, Perspective_t perspective = PERSP_FOV90_LOW);
	
protected:

	bool init(PixFu *engine) override;
	
	void tick(PixFu *engine, float fElapsedTime) override;

	void processInput(PixFu *engine, float fElapsedTime);
	void driveSpline(float fElapsedTime);

private:
	// process collisions
	long processCollisions(float fElapsedTime);

	// process static collisions
	void processStaticCollision(Ball *ball, Ball *target);

	// process dynamic collisions
	void processDynamicCollision(Ball *ball, Ball *target, float fElapsedTime);
};


}
