//
//  Circuit.h
//  javid
//
//  Created by rodo on 28/12/2019.
//  Copyright © 2019 rodo. All rights reserved.
//

#pragma once
#include <string>
#include <vector>

#include "Drawable.hpp"
#include "Ball.hpp"
#include "Meta.hpp"
#include "Player.hpp"

#define HEIGHT_EDGE_FLYOVER 4.0

namespace rgl {

// singleton class to hold all PNGs involved in the circuit
// contract: only one circuit can be loaded at the same time.

	class CircuitAssets {

		int iMapWidth, iMapHeight;
		static CircuitAssets *__currentInstance;

	public:

		static CircuitAssets *currentInstance();

		CircuitAssets(std::string name);

		~CircuitAssets();

		Drawable *sprCars = nullptr;        // buffer for trails
		Drawable *sprGround = nullptr;    // ground buffer
		Drawable *sprHeight = nullptr;    // height buffer
		Drawable *sprHoriz = nullptr;    // parallax buffer
		Drawable *sprSky = nullptr;        // sky buffer

		int getMapWidth();

		int getMapHeight();

	};

	class Circuit {

		// circuit is the world where players move. we declare them friends for
		// performance as we need to constantly access and change lots of properties
		// that we don´t really want to be exposed as public

		friend class PlayerArmed;

	public:

		Circuit();

		~Circuit();

	private:

		Player *pPlayer, *pCamPlayer;

		std::vector<Player *> vPlayers;
		std::vector<GameObject *> vObjects;

		std::vector<Ball *> vFakeBalls;
		std::vector <std::pair<Ball *, Ball *>> vCollidingPairs;
		std::vector <std::pair<Ball *, Ball *>> vFutureColliders;

		void init();

		// Spawns a new player
		Player *spawnPlayer(PlayerFeatures_t features = {}, int position = -1, bool human = false,
							  bool autoInit = true);

		// Spawns a new object
		Ball *spawnObject(ObjectInfo_t const *objectInfo, Player *dropper);

		// Spawns a new static object
		Ball *spawnObject(ObjectInfo_t const *objectInfo, glm::vec3 objectPos, float radius);

		// process collisions
		long processCollisions(float fElapsedTime);

		// process static collisions
		void processStaticCollision(Ball *ball, Ball *target);

		// process dynamic collisions
		void processDynamicCollision(Ball *ball, Ball *target, float fElapsedTime);

		float getTerrainHeight(glm::vec2 pos);

	public:

		// Get circuit static objects: player spots, weapons marker, ...
		ObjectPlacement_t *getUniqueObject(int code);

		// Get all circuit objects
		std::vector <ObjectPlacement_t> &getCircuitObjects();

	};

// inline implementations for CircuitAssets

	inline CircuitAssets *CircuitAssets::currentInstance() { return __currentInstance; }

	inline int CircuitAssets::getMapWidth() { return iMapWidth; }

	inline int CircuitAssets::getMapHeight() { return iMapHeight; }


	// Get all circuit objects
	inline std::vector <ObjectPlacement_t> &Circuit::getCircuitObjects() {
		return pGameCircuit->vecObjects;
	}

}