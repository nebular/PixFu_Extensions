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
: World(config, perspective),
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

void Arena::processStaticCollision(Ball *ball, Ball *target) {
	
	glm::vec3 displacement = ball->calculateOverlapDisplacement(target);
	
	// Displace Current Ball away from collision
	ball->position.x -= displacement.x;
	ball->position.y -= displacement.y;
	ball->position.z -= displacement.z;
	
	// Displace Target Ball away from collision
	target->position.x += displacement.x;
	target->position.y += displacement.y;
	target->position.z += displacement.z;
		
}

void Arena::processDynamicCollision(Ball *b1, Ball *b2, float fElapsedTime) {
	
	//	auto grados = [](float rads) { return std::to_string(rads*180/PI); };
	glm::vec3 pos1 = b1->position, pos2 = b2->position;
	
	// Distance between balls TODO
	float fDistance = sqrtf(  (pos1.x - pos2.x) * (pos1.x - pos2.x)
							+ (pos1.z - pos2.z) * (pos1.z - pos2.z) );
	
	// Normal
	float nx = (pos2.x - pos1.x) / fDistance;
	float nz = (pos2.z - pos1.z) / fDistance;

	// this "-" is because the map is rotated, I would like to avoid this but I
	// dont know where to put this fix
//	glm::vec3 b1vel = -b1->speedVector();
//	glm::vec3 b2vel = -b2->speedVector();
	
	glm::vec3 b1vel = -b1->speedVector();
	glm::vec3 b2vel = -b2->speedVector();

	
	float b1mass = b1->mass(), b2mass = b2->mass();
	
	// Wikipedia Version - Maths is smarter but same
	float kx = b1vel.x - b2vel.x;
	float kz = b1vel.z - b2vel.z;
	float p = 2.0 * (nx * kx + nz * kz) / (b1mass + b2mass) ;
	
	b1vel.x = b1vel.x - p * b2mass * nx;
	b1vel.z = b1vel.z - p * b2mass * nz;
	
	b2vel.x = b2vel.x + p * b1mass * nx;
	b2vel.z = b2vel.z + p * b1mass * nz;
	
	float b2speed = sqrt(b2vel.x * b2vel.x + b2vel.z * b2vel.z)* KCRASH_EFFICIENCY;
	float b1speed = sqrt(b1vel.x * b1vel.x + b1vel.z * b1vel.z)* KCRASH_EFFICIENCY;
	
	float b1Angle = atan2(b1vel.z, b1vel.x);
	float b2Angle = atan2(b2vel.z, b2vel.x);

	b1->onCollision(b2, fElapsedTime, b1speed, b1Angle);
	b2->onCollision(b1, fElapsedTime, b2speed, b2Angle);
	
}

long Arena::processCollisions(float fElapsedTime) {
	
	long crono = nowns();
	
	vFakeBalls.clear();
	vCollidingPairs.clear();
	
	// Break up the frame elapsed time into smaller deltas for each simulation update
	float fSimElapsedTime = fElapsedTime / (float) Ball::SIMULATIONUPDATES;
	
	// Main simulation loop
	for (int i = 0; i < Ball::SIMULATIONUPDATES; i++) {
		
		// Set all balls time to maximum for this epoch
		
		iterateObjects([fSimElapsedTime](WorldObject *ball) {
			static_cast<Ball *>(ball)->fSimTimeRemaining = fSimElapsedTime;
		});
		
		// Erode simulation time on a per objec tbasis, depending upon what happens
		// to it during its journey through this epoch
		
		for (int j = 0; j < Ball::MAXSIMULATIONSTEPS; j++) {
			
			// Update Ball Positions
			iterateObjects([this](WorldObject *w) {
				Ball *ball = (Ball*)w;
				if (!ball->bDisabled) {
					if (ball->fSimTimeRemaining > 0.0f) {
						Ball *obstacle;
						Player *player = dynamic_cast<Player *>(ball);
						obstacle = player
						? player->process(this, NOTIME)
						: ball->process(this, NOTIME);
						
#ifndef DBG_NOHEIGHTMAPCOLLISIONS
						
						// these are collisions against height map
						
						if (obstacle != nullptr) {
							vFakeBalls.push_back(obstacle);
							// Add collision to vector of collisions for dynamic resolution
							vCollidingPairs.push_back({ball, obstacle});
							if (DBG) LogV(TAG, "- Ball collided with wall");
						}
#endif
					}
				}
			});
			
#ifdef DBG_NOCARCOLLISIONS
			// this would disable car collisions (with other cars)
			return pnow()-crono;
#endif
			int ostias = 0;
			
			// Work out static collisions with walls and displace balls so no overlaps
			iterateObjects([this, fSimElapsedTime,&ostias](WorldObject *b) {
				
				Ball *ball = (Ball*)b;
				
				if (!ball->isStatic() && !ball->bDisabled)
					for (auto &edge : this->LEVEL->vecLines) {
						
						// float fDeltaTime = ball->fSimTimeRemaining;
						// Against Edges
						
						// Check that line formed by velocity vector, intersects with line segment
						float fLineX1 = edge.ex - edge.sx;
						float fLineY1 = edge.ey - edge.sy;
						
						float fLineX2 = ball->position.x - edge.sx;
						float fLineY2 = ball->position.z - edge.sy;
						
						float fEdgeLength = fLineX1 * fLineX1 + fLineY1 * fLineY1;
						
						// This is nifty - It uses the DP of the line segment vs the line to the object, to work out
						// how much of the segment is in the "shadow" of the object vector. The min and max clamp
						// this to lie between 0 and the line segment length, which is then normalised. We can
						// use this to calculate the closest point on the line segment
						
						float t = fmax(0, fmin(fEdgeLength, (fLineX1 * fLineX2 + fLineY1 * fLineY2))) / fEdgeLength;
						
						// Which we do here
						float fClosestPointX = edge.sx + t * fLineX1;
						float fClosestPointY = edge.sy + t * fLineY1;
						
						// And once we know the closest point, we can check if the ball has collided with the segment in the
						// same way we check if two balls have collided
						
						float fDistance = sqrtf((ball->position.x - fClosestPointX) * (ball->position.x - fClosestPointX) +
												(ball->position.z - fClosestPointY) * (ball->position.z - fClosestPointY));

						if (ball->height() < HEIGHT_EDGE_FLYOVER) {

							if (fDistance <= ball->radius() + edge.radius) {
								
								ostias++;
								if (DBG)
									LogV(TAG, SF("collision, dist %f, bradius %f", fDistance, ball->radius()));
								
								// Collision has occurred - treat collision point as a ball that cannot move. To make this
								// compatible with the dynamic resolution code below, we add a fake ball with an infinite mass
								// so it behaves like a solid object when the momentum calculations are performed
								
								Ball *fakeball = ball->makeCollisionBall((edge.radius + 2.0f), ball->mass()*1000); // ball->mass() * .8f);
								fakeball->position = {fClosestPointX, ball->position.y, fClosestPointY};
								
								//						fakeball->fSpeed=ball->fSpeed;
								fakeball->fSpeed = 0;
								//						fakeball->fMass = ball->fMass; // 900000;
								//fakeball->fAngle = 0;
								// fakeball->fAngle = -ball->fAngle;

								// TODO: smartly calculating fHeight and Radius here will allow
								// us to jump over walls if so desired. Idea is to make edges
								// jumpable unless their height in the heightmap is 1
								// Add collision to vector of collisions for dynamic resolution
								
								vCollidingPairs.push_back({ball, fakeball});
								
								// Calculate displacement required
								float fOverlap = 1.00f * (fDistance - ball->radius() - fakeball->radius());
								
								// Displace Current Ball away from collision
								ball->position.x -= fOverlap * (ball->position.x - fakeball->position.x) / fDistance;
								ball->position.z -= fOverlap * (ball->position.z - fakeball->position.z) / fDistance;
								
								if (DBG)
									LogV(TAG, SF("overlap %f and after displacement %d", fOverlap, ball->overlaps(fakeball)));
								
							}
							}
					}
				
				// Against other balls
				iterateObjects([this, ball](WorldObject *targ) {
					Ball *target = (Ball *)targ;

					if (!ball->bDisabled
						&& !target->bDisabled							// disabled balls
						&& ball->id() != target->id()					// same ball
						&& (!ball->isStatic() || !target->isStatic())		// 2 static objs wont collide (opt)
						) {
						switch (ball->overlaps(target)) {
							case OVERLAPS:
								// Collision has occured
								vCollidingPairs.push_back({ball, target});
								processStaticCollision(ball, target);
								break;
							case OVERLAPS_OUTER:
								vFutureColliders.push_back({ball, target});
								break;
							default:
								break;
						}
					}
				});
				
				ball->commitSimulation();
				
			});
			
			// Now work out dynamic collisions
			for (auto c : vCollidingPairs)
				processDynamicCollision(c.first, c.second, fElapsedTime);
			
			for (auto c : vFutureColliders)
				c.first->onFutureCollision(c.second);
			
			vCollidingPairs.clear();
			vFutureColliders.clear();
			
			// Remove fake balls
			for (auto &b : vFakeBalls) delete b;
			vFakeBalls.clear();
		}
	}
	return nowns() - crono;
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
	processCollisions(fElapsedTime);
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
