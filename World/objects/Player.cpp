#include "Player.hpp"
#include "Utils.hpp"

namespace rgl {

Player::Player(PlayerFeatures_t features)
: Ball(features.className, features.config.position,
	   features.config.radius, features.config.mass),
FEATURES(new PlayerFeatures(features)) {
	
}

// process ball iteration
Ball *Player::process(World *world, float fElapsedTime) {
	
	float KHANDLING = 0.5; // 0.01;
	Canvas2D *canvas = world->canvas(position);

	// process the ball parameters
	Ball *obstacle = Ball::process(world, fElapsedTime);
	float fixed = (float)(((int)(fSteerAngle * 10000)) / 10000.0f);
	fAngle += KHANDLING * fElapsedTime * -fixed * fSpeed;
	return obstacle;
	
	// trick tå use fSimTimeRemaining from outside without knowing it
	if (fElapsedTime == NOTIME)
		fElapsedTime = fSimTimeRemaining;

	// http://engineeringdotnet.blogspot.com/2010/04/simple-2d-car-physics-in-games.html
	
	//	float vx = -cosf(fAngle) * fSpeed, vy = -sinf(fAngle) * fSpeed;
	// Update position
	//position.x += -sinf(fAngle) * fSpeed * fElapsedTime * KSPEED;
	//position.z += -cosf(fAngle) * fSpeed * fElapsedTime * KSPEED;

	float ang = fAngle;
	glm::vec3 headingBack = glm::vec3(-sinf(ang),0, -cosf(ang) );
	glm::vec3 headingFront = glm::vec3(-sinf(ang + fSteerAngle),0, -cosf(ang + fSteerAngle) );

	//	glm::vec3 headingFront = glm::vec3(-sinf(fAngle + fSteerAngle),0, -cosf(fAngle + fSteerAngle) );
//	glm::vec3 headingBack = glm::vec3(-sinf(fAngle),0, -cosf(fAngle) );

	auto toDeg = [] (float rad) { return (int)(rad*180/M_PI); };
	
	LogV("Player", SF("angle %d steerAngle %d", toDeg(fAngle), toDeg(fSteerAngle)));

	/**

	 float
		 vx = -cosf(fAngle) * fSpeed,
		 vy = -sinf(fAngle) * fSpeed;

	 // Update position
	 position.z += vx * fTime * KSPEED;
	 position.x += vy * fTime * KSPEED;
 	 
 **/
	
	/**
	 We can use geometry calculations to find the actual positions of the wheels
	 (which are at the centre of each axle). The gap between the axles is given by
	 the wheelBase, so each wheel is half that distance from the car centre, in the
	 direction that the car is facing.
	 */
	
	glm::vec3 backWheel = position - FEATURES->wheelBase() / 2 * headingBack;
	glm::vec3 frontWheel = position + FEATURES->wheelBase() / 2 * headingFront;

	
	canvas->blank();
	canvas->fillCircle(frontWheel.x, frontWheel.z, 2, rgl::Colors::RED);
	canvas->fillCircle(backWheel.x, backWheel.z, 2, rgl::Colors::MAGENTA);

	/**
	 Each wheel should move forward by a certain amount in the direction it is pointing.
	 The distance it needs to move depends on the car speed, and the time between frames
	 The rear wheel is easy, it moves in the same direction the car is heading.
	 For the front wheel, we have to add the steer angle to the car heading
	 */
	
	//	backWheel += carSpeed * dt * new Vector2(cos(carHeading) , sin(carHeading));
	//	frontWheel += carSpeed * dt * new Vector2(cos(carHeading+steerAngle) , sin(carHeading+steerAngle));
	// position 1000,0,800 ... angle 0 , backwheel: 1000,0,808, frontWheel:  1000,0,791
	
	
	backWheel += fSpeed * fElapsedTime * headingFront * 1000.0f; // * KSPEED;
	frontWheel += fSpeed * fElapsedTime * headingBack * 1000.0f; //  * KSPEED;

	canvas->fillCircle(frontWheel.x, frontWheel.z, 2, rgl::Colors::BLACK);
	canvas->fillCircle(backWheel.x, backWheel.z, 2, rgl::Colors::GREY);

	/*
	The new car position can be calculated by averaging the two new wheel positions.
	The new car heading can be found by calculating the angle of the line between the
	two new wheel positions:
	*/

	position = (frontWheel + backWheel) / 2.0f;
	canvas->fillCircle(position.x, position.z, 2, rgl::Colors::BLUE);
	
	fAngle = -M_PI/2 - atan2( frontWheel.z - backWheel.z , frontWheel.x - backWheel.x );

//	fAngle = atan2( frontWheel.z - backWheel.z , frontWheel.x - backWheel.x );
//	fAngle = atan2( frontWheel.z - backWheel.z , frontWheel.x - backWheel.x );
//	fAngle = atan2(-frontWheel.x + backWheel.x , -frontWheel.z + backWheel.z );
//	fAngle = atan2(frontWheel.x - backWheel.x , frontWheel.z - backWheel.z );

	if (fAcceleration != 0)
		fSpeed += fAcceleration * fElapsedTime; //  - TERRAINFRICTION * fTime * (1 - fmin(fSpeed / 0.2, 1));

//	float vx = -cosf(fAngle) * fSpeed, vy = -sinf(fAngle) * fSpeed;

	// Update position
//	position.z += vx * fElapsedTime * 1000;
//	position.x += vy * fElapsedTime * 1000;

	// process heights from heightmap
	return processHeights(world, fElapsedTime);

}

void Player::steer(float perc, float fElapsedTime) {
	
	fSteerAngle = perc * M_PI / 16; // * (- log2(fabs(speedPercent()) + 0.000001));
	
}

void Player::accelerate(float percentage, float fElapsedTime) {
	
	// MAXSPEED == speed -> 0;
	// MINSPEED -> KACCEL
	
	bReverse = false;
	bForward = true;
	
	bForward = fSpeed > 0 && percentage != 0;
	bReverse = fSpeed < 0 && fAcceleration < 0 && percentage != 0;

	fAcceleration = KACCEL * percentage * (FEATURES->maxSpeed() - fmin(fSpeed, FEATURES->maxSpeed())) / FEATURES->maxSpeed();
	
//	if (!bReverse && fSpeed <0.01 ) fAcceleration = fmax(fAcceleration, 0.05);
	
//	fAcceleration = 0;
}

void Player::brake(float percentage, float fElapsedTime) {
	float brake = -KDECCEL * fSpeed * percentage;
	fAcceleration = brake;
}

void Player::jump() {
	
}

}
