#include "Player.hpp"

namespace rgl {

Player::Player(PlayerFeatures_t features)
: Ball(features.className, features.config.position,
	   features.config.radius, features.config.mass),
FEATURES(new PlayerFeatures(features)) {
	
}

// process ball iteration
Ball *Player::process(World *world, float fElapsedTime) {
	
	// process the ball parameters
	Ball *obstacle = Ball::process(world, fElapsedTime);
	
	// trick tå use fSimTimeRemaining from outside without knowing it
	if (fElapsedTime == NOTIME)
		fElapsedTime = fSimTimeRemaining;
	
	// decrease steer "momentum" (auto center / oversteered sprite on turns)
//	if (fSteerMomentum>0) fSteerMomentum-=fElapsedTime*100;
//	if (fSteerMomentum<0) fSteerMomentum+=fElapsedTime*100;
	
	// some elasticity for the angle on turns
//	float k =  1;// + (- log2(fabs(getSpeedPercent()) + 0.0001));
//	float kg = 1; // todo bGrip ? 1.5:1;
	
//	if (fSteerAngle < -FEATURES->handling()) fSteerAngle = -FEATURES->handling();
//	if (fSteerAngle > FEATURES->handling()) fSteerAngle = FEATURES->handling();
	
//	fAngle += fSteerAngle * fSpeed * k * kg * fElapsedTime;

	float KHANDLING = 5;
	
//	fAngle += KHANDLING * fElapsedTime * fSteerAngle * getSpeedPercent();

	fAngle += KHANDLING * fElapsedTime * fSteerAngle * (fSpeed/0.145);



	//	fAngle += fSteerAngle * k * kg * fElapsedTime;

//	if (fAngle < -M_PI) fAngle += 2*M_PI;
//	if (fAngle > M_PI) fAngle -= 2*M_PI;
	
//	if (fSteerAngle>0) fSteerAngle -= FEATURES->traction() * 30 * fElapsedTime;
//	if (fSteerAngle<0) fSteerAngle += FEATURES->traction() * 30 * fElapsedTime;;
	
	return obstacle;
	
}

void Player::steer(float perc, float fElapsedTime) {
	
	fSteerAngle = perc; // * (- log2(fabs(speedPercent()) + 0.000001));
	
}

void Player::accelerate(float percentage, float fElapsedTime) {
	
	// MAXSPEED == speed -> 0;
	// MINSPEED -> KACCEL
	
	bReverse = false;
	bForward = true;
	
	bForward = fSpeed > 0 && percentage != 0;
	bReverse = fSpeed < 0 && percentage != 0;

	fAcceleration = KACCEL * percentage * (FEATURES->maxSpeed() - fmin(fSpeed, FEATURES->maxSpeed()))
	/ FEATURES->maxSpeed();
	
	if (fSpeed <0.01) fAcceleration = fmax(fAcceleration, 0.05);
	
//	fAcceleration = 0;
}

void Player::brake(float percentage, float fElapsedTime) {
	float brake = -KDECCEL * fSpeed * percentage;
	fAcceleration = brake;
}

void Player::jump() {
	
}

}
