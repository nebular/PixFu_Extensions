//
// Base class for any object in the world. This is an abstract class.
// You will extend this class to provide physics to your objects.
//
//  Created by rodo on 08/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "WorldMeta.hpp"

namespace Pix {

	class World;

	class WorldObjectBase {

		static int instanceCounter;

	protected:

		/** World Configuration */
		const WorldConfig_t &WORLD;

	public:

		const unsigned CLASSID;

		/** Object ID */
		const int ID;

		/** Object Classname (maps to resources) */
		const std::string CLASS;

		inline WorldObjectBase(const WorldConfig_t &worldConfig, std::string objectClass, unsigned classId, int overrideId = -1) :
				WORLD(worldConfig),
				CLASSID(classId),
				ID(overrideId >= 0 ? overrideId : instanceCounter++),
				CLASS(objectClass) {}

		inline virtual ~WorldObjectBase() = default;

		/**
		 * Return object position
		 * @return The object position
		 */
		virtual glm::vec3 &pos() = 0;

		/**
		 * Return object rotation around xyz axis
		 * @return The rotation vector in radians
		 */
		virtual glm::vec3 &rot() = 0;

		/**
		 * Return object radius
		 * @return Radius in World Coordinates
		 */
		virtual float radius() = 0;

		/**
		 * Return object radius
		 * @return Radius in World Coordinates
		 */
		virtual float drawRadius() = 0;

	};

	/**
	 * Convenience class for a static world object without any physics. Will just get rendered
	 * in a static location.
	 */

	class WorldObject : public WorldObjectBase {


		/** Whether the object is selected (settable flag) */
		bool bSelected = false;

		/** Object Tint */
		glm::vec4 mTintCode = TINT_NONE;

	protected:

		float fRadiusAnimator = 0;

		// Object Location
		ObjectLocation_t LOCATION;
		

	public:

		inline static constexpr glm::vec4 TINT_NONE = { 0,0,0,0};
		inline static constexpr glm::vec4 TINT_SELECT = { 2,0,0,1};
		static constexpr unsigned CLASSID_CODE = 1;

		// Object metadata
		const ObjectProperties_t CONFIG;

		inline WorldObject(const WorldConfig_t &worldConfig, const ObjectProperties_t objectMeta, ObjectLocation_t location,
						   unsigned int classid = CLASSID_CODE, int overrideId = -1) :
				WorldObjectBase(worldConfig, objectMeta.CLASSNAME, classid, overrideId),
				LOCATION(std::move(location)),
				CONFIG(std::move(objectMeta))
				{}

		inline virtual glm::vec3 &pos() override { return LOCATION.position; }

		inline virtual glm::vec3 &rot() override { return LOCATION.rotation; }

		inline virtual float radius() override { return CONFIG.radius; }

		// todo comment why this is normaized !!
		inline virtual float drawRadius() override { return CONFIG.radius * CONFIG.drawRadiusMultiplier / 1000.0F; }

		inline virtual glm::vec4 &tintCode() { return mTintCode; }

		inline bool isSelected() { return bSelected; };

		/**
		 * Selects this object
		 */
		inline void setSelected(bool selected=true) { bSelected = selected; };

		/**
		 * Tints this object
		 */
		inline void setTint(glm::vec4 tintCode) { mTintCode = tintCode; };

		/** process animations */
		virtual void process(World *world, float fElapsedTime);
		
		// https://gamedev.stackexchange.com/questions/21552/picking-objects-with-mouse-ray
		bool checkRayCollision(glm::vec3& origin, glm::vec3& direction);
	};
}
