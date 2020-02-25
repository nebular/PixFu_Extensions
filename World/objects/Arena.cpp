//
//  Arena.cpp
//  PixEngine
//
//  Created by rodo on 21/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Arena.hpp"
#include "Ball.hpp"
#include "Player.hpp"
#include "AxisController.hpp"
#include "Utils.hpp"
#include "Config.hpp"

#define DBG_NOHEIGHTMAPCOLLISIONS

// 0.25 energy loss on crash
#define KCRASH_EFFICIENCY 0.75

namespace rgl {

// heigh over which
constexpr int HEIGHT_EDGE_FLYOVER=1000;

std::string Arena::TAG = "Arena";

Arena::Arena(std::string name, WorldConfig_t config, Perspective_t perspective)
: BallWorld(config, perspective),
  LEVEL(new ArenaLevel_t(name)) {
	LogV(TAG, SF("Level Name %s", name.c_str()));
	add({name});
}

bool Arena::init(PixFu *engine) {
	
	if (!World::init(engine)) return false;
	
	pCamPlayer =
	pHumanPlayer =  new Player({
		"kart_peach",
		{
			{150, 100, 150},		// pos
			{0.0, 0.0 ,0.0},		// rot
			10,						// radius
			10						// mass
		}
	});

	add(pHumanPlayer, {
		{0,0,0},
		{0,0,0},
		1
	});
	
	camera()->setTargetMode(true);

 #define  TREES
	#ifdef TREES
		for (int i=0; i<1700; i+=200) {
			for (int j=0; j<1300; j+=200) {
				add(new GameObject({
					"tree" , {
						{ i, 0, j },	// pos
						{0.0, 0, 0.0},					// rot
						(3.0f + random()%12),
						100000
					}
				}));
			}
		}
	#endif
	
	GenericAxisController::enable(-1,1, -1,1, true, true, false, true);
	engine->addInputDevice(GenericAxisController::instance());

	return true;
}

void Arena::driveSpline(float fElapsedTime) {
	
	sSpline spl = LEVEL->sPaths[1];

	if (fPos>spl.fTotalSplineLength) fPos = 0;
	
	fPos+=fElapsedTime;

	sPoint2D splPoint = spl.GetSplinePoint(fPos);

	glm::vec3 &pos = pHumanPlayer->posWorld();
	pos.x = splPoint.x; //1000;
	pos.z = splPoint.y; //1000;
	
	pHumanPlayer->fAngle = -spl.GetSplineAngle(fPos) - M_PI / 2;
		
}

void Arena::tick(PixFu *engine,float fElapsedTime) {
	

//	driveSpline(fElapsedTime);
	processCollisions(LEVEL->vecLines, fElapsedTime);
	processInput(engine, fElapsedTime);

	engine->canvas()->drawString(0,70,rgl::SF("PLY acc %f spd %f ang %f",
									  pHumanPlayer->fAcceleration,
									  pHumanPlayer->speed(),
											  pHumanPlayer->angle()),
						 rgl::Colors::WHITE, 3);
	engine->canvas()->drawString(0,40,rgl::SF("PLY x %f y %f z %f",
									  pHumanPlayer->position.x,
									  pHumanPlayer->position.y,
									  pHumanPlayer->position.z),
						 rgl::Colors::WHITE, 3);
	engine->canvas()->drawString(0,10,rgl::SF("CAM x %f y %f z %f",
									  camera()->getPosition().x,
									  camera()->getPosition().y,
									  camera()->getPosition().z),
						 rgl::Colors::YELLOW, 3);

	World::tick(engine, fElapsedTime);
	


}

void Arena::processInput(PixFu *engine, float fElapsedTime) {

	
	if (Keyboard::isPressed(Keys::SPACE)) {
		pCamPlayer = pCamPlayer == nullptr ? pHumanPlayer : nullptr;
		camera()->setTargetMode(pCamPlayer != nullptr);
	}
	
	// Select mode with ALT/CMD
	CameraKeyControlMode_t mode =
			Keyboard::isHeld(Keys::ALT) ? rgl::ADJUST_ANGLES :
			Keyboard::isHeld(Keys::COMMAND) ? rgl::ADJUST_POSITION : rgl::MOVE;
		
	if (pCamPlayer == nullptr) // || mode != MOVE)
		camera()->inputKey(
					  mode,
					  Keyboard::isHeld(Keys::UP),
					  Keyboard::isHeld(Keys::DOWN),
					  Keyboard::isHeld(Keys::LEFT),
					  Keyboard::isHeld(Keys::RIGHT),
					  fElapsedTime
					  );



	float xdelta = 0, ydelta = 0, K = 0.01;
	if (pCamPlayer!=nullptr) {
		camera()->follow(pHumanPlayer);
		if (mode != MOVE) {
			camera()->inputKey(
							  mode == ADJUST_ANGLES ? ADJUST_PLAYER_ANGLES : ADJUST_PLAYER_POSITION,
							  Keyboard::isHeld(Keys::UP),
							  Keyboard::isHeld(Keys::DOWN),
							  Keyboard::isHeld(Keys::LEFT),
							  Keyboard::isHeld(Keys::RIGHT),
							  fElapsedTime
							  );


		} else {
			if (Keyboard::isHeld(Keys::UP)) 	ydelta = K;
			if (Keyboard::isHeld(Keys::DOWN)) 	ydelta = -K;
			if (Keyboard::isHeld(Keys::LEFT)) 	xdelta = -K;
			if (Keyboard::isHeld(Keys::RIGHT)) 	xdelta = K;
		
			if (ydelta !=0 || xdelta != 0)
				GenericAxisController::instance()->inputIncremental(xdelta, ydelta);
		}
	}

	GenericAxisController::instance()->drawSelf(engine->canvas(), rgl::Colors::RED);
	if (pCamPlayer != nullptr) {
		pHumanPlayer->steer(-GenericAxisController::instance()->x() , fElapsedTime);
//		pHumanPlayer->steer(-GenericAxisController::instance()->xInterp(), fElapsedTime);
		pHumanPlayer->accelerate(GenericAxisController::instance()->yInterp(), fElapsedTime);
	}
}
};
