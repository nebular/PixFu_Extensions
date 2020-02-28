//
//  Ball.hpp
//  javid
//
//  Created by rodo on 11/01/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

// Flag No Time Info
#define NOTIME -1
// car decceleration constant TODO move to sFeatures
#define TERRAINFRICTION 0.005
// height irregularities gradient we can ignore and drive through unaffected
#define RIDEHEIGHT_SEAMLESS 5
// 0.1

#define FAKE_BALL_ID 9999

#include "glm.hpp"
#include "Drawable.hpp"
#include "World.hpp"
#include "BallWorld.hpp"

namespace rgl {


	typedef enum eOverlaps {
		OVERLAPS, OVERLAPS_OUTER, NO_OVERLAPS
	} Overlaps_t;

	class Ball : public WorldObject {

		friend class BallWorld;
		
		std::string TAG;
		
		static int instanceCounter;
		
		float fMetronome = 0;

	protected:

		// Circuit has the tight collision detection loops
		// so it is cool to have private access to vars, mostly to position.

		friend class Arena;

		friend class Orbit;

		static constexpr float FEATURES_SCRATCHING_NEW = 0.6; //89;	// TODO
		static constexpr float FEATURES_CLIMB_LIMIT = 0.3;        // TODO
		static constexpr float FEATURES_FALL_LIMIT = 0.3;        // TODO move to features

		// Threshold indicating stability of object
		static constexpr float STABLE = 0.001;

		// Multiple simulation updates with small time steps permit more accurate physics
		// and realistic results at the expense of CPU time of course
		// TODO: research which values to use as jdavidx example uses hundreds of balls but
		// TODO: we are an order of magnitude below, at the most around 30 objects and very spaced

		static constexpr int SIMULATIONUPDATES = 2; // 4

		// Multiple collision trees require more steps to resolve. Normally we would
		// continue simulation until the object has no simulation time left for this
		// epoch, however this is risky as the system may never find stability, so we
		// can clamp it here

		static constexpr int MAXSIMULATIONSTEPS = 15; // 15

		const int nId;					// Ball unique ID
		const float fRadius;			// ball radius
		const float fMass;				// ball mass
		const bool bStatic;				// whether this is a static object (so wont collide with another static object)
		const bool bPlayer;				// (cheat/optimization) to avoid using dynamic cast, flag is set if this is a player descendent class
		
		glm::vec3 position = { 0, 0, 0};        // ball position in world coordinates
		glm::vec3 rotation = { 0,0,0 };			// ball rotation
		glm::vec3 mSpeed = { 0,0,0 };			// ball speed
		glm::vec3 acceleration = { 0,0,0 };		// ball acceleration

		// extensions
		static float stfBaseScale;				// A Base scale for all balls in the simulation
		static float stfHeightScale; 			// heights pixel height

		float fOuterRadius = 0;					// outer radius to predict next collisions
		float fMassMultiplier = 1.0;			// multiply ball mass (game powerups)
		float fRadiusMultiplier = 1.0;			// multiply ball radius (game powerups)

		// Height extensions
		float fHeightTarget = 0.0;				// target height (gravity effect)

		float fAccelerationZ = 0.0; 			// upwards acceleration TODO move to acceleration

		float fAngleTerrain = 0.0;				// player horizontal angle / terrain

		float fPenalty = 1.0;					// penalty in speed percent imposed by terrain irregularities

		bool  bFlying = false;					// whether the ball is currently "flying"
		
		bool bReverse = false; 					// reverse gear flag
		bool bForward = false; 					// forward gear flag
		bool bDisabled = false;					// disable the player (will not be updated & behave as ghost) (debug)


		// Internal Simulation vars
		glm::vec3 origPos;
		float fSimTimeRemaining;

		Ball(const WorldConfig_t &planetConfig, std::string className, glm::vec3 position, float radi, float mass, bool isStatic = false, bool isPlayer = false);
		Ball(const WorldConfig_t &planetConfig, float radi, float mass, glm::vec3 position, glm::vec3 speed);

	public:

		static void setBaseScale(float scale);
		static void setHeightScale(float scale);

	protected:
		
		float id() const;

		glm::vec3 pos() override;		// ball normalized position
		glm::vec3 &posWorld();			// ball world position

		glm::vec3 rot() override;		// ball 3d rotation

		float radius() override;		// ball radius

		float angle();  				// ball angle (heading)
		float speed();  				// ball speed
		float mass();   				// ball mass
		float outerRadius();    		// outer radius (collision prediction)

		glm::vec3 speedVector();    	// speed vector
		bool flying();            		// whether ball is flying

		// Mass and radius multipliers (generic game powerups)
		void setMassMultiplier(float massMultiplier);

		void setRadiusMultiplier(float radiusMultiplier);

		// distance to another ball
		float distance(Ball *target);

		bool isPointInBall(glm::vec3 point);

		bool intersects(Ball *b2, bool outerRadius);
		
		bool isStatic() const;

		// whether a ball overlaps this one
		Overlaps_t overlaps(Ball *b2);

		// whether a ball overlaps this one
		Overlaps_t overlaps2D(Ball *b2);

		// convenience displacement required to avoid an overlap
		glm::vec3 calculateOverlapDisplacement(Ball *target, bool outer = false);

		// make a collision ball (used to model crashes against walls & terrain)
		Ball *makeCollisionBall(float radi, glm::vec3 position);

		// subclasses MUST call this
		virtual void onCollision(Ball *other, float fElapsedTime, glm::vec3 newSpeed);

		virtual void onFutureCollision(Ball *other);

		// disable ball
		void disable(bool disabled);

		// internal loop function to commit simulation steps
		void commitSimulation();

		// process ball iteration
		Ball *process(World *world, float fTime);

		// heights
		float getTerrainHeight(glm::vec3 pos, World *world);

		// process Height effects (height calcs separated from 2D calcs)
		Ball *processHeights(World *world, float fTime);

	};

// INLINE IMPLEMENTATION BELOW THIS POINT

	inline float Ball::id() const { return nId; }
	inline glm::vec3 Ball::pos() { return position/1000.0f; }                        // ball world position
	inline glm::vec3 &Ball::posWorld() { return position; }                        // ball world position
	inline glm::vec3 Ball::rot() { return rotation;}                        // ball world position

	inline float Ball::mass() { return fMass * fMassMultiplier; }            // ball final mass
	inline float Ball::radius() { return fRadius * fRadiusMultiplier; }        // ball final radius
	inline float Ball::outerRadius() { return fOuterRadius * fRadiusMultiplier; }
	inline float Ball::angle() { return rotation.y; }                            // ball angle (heading)
	inline float Ball::speed() { return sqrtf(mSpeed.x * mSpeed.x + mSpeed.z * mSpeed.z); }   // TODO fabs             		// ball speed
	inline glm::vec3 Ball::speedVector() { return mSpeed; }

	inline bool Ball::isStatic() const { return bStatic;}
	inline bool Ball::flying() { return bFlying; }                            // whether ball is flying

	inline void Ball::setMassMultiplier(float massMultiplier) { fMassMultiplier = massMultiplier; }
	inline void Ball::setRadiusMultiplier(float radiusMultiplier) { fRadiusMultiplier = radiusMultiplier * stfBaseScale; }

	// distance to another ball
	inline float Ball::distance(Ball *target) {
		return sqrtf(
				(position.x - target->position.x) * (position.x - target->position.x)
				+ (position.z - target->position.z) * (position.z - target->position.z));
	}

	inline void Ball::setBaseScale(float scale) {
		stfBaseScale = scale;
	}

	inline void Ball::setHeightScale(float scale) {
		stfHeightScale = scale;
	}

	class LinearDelayer {

		float fTarget;
		float fCurrent;
		float fIncrement;

	public:
		LinearDelayer(float increment);

		void setTarget(float target, bool force);

		float get();

		float tick(float fElapsedTime);

		bool isStable();
	};


	inline LinearDelayer::LinearDelayer(float increment) { fIncrement = increment; }

	inline void LinearDelayer::setTarget(float target, bool force = false) {
		fTarget = target;
		if (force)
			fCurrent = target;
	}

	inline float LinearDelayer::get() { return fCurrent; }

	inline bool LinearDelayer::isStable() { return fCurrent == fTarget; }
}
#pragma clang diagnostic pop
