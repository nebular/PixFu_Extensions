//
// Created by rodo on 2020-02-17.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma once

#include "WorldMeta.hpp"
#include "Terrain.hpp"
#include "ObjectCluster.hpp"

#include <vector>
#include <map>
#include <cmath>
#include "ext.hpp"

namespace rgl {

//////////////////////////////////////////////////////////////////////////////////////////


	class WorldObject {
	public:
		const std::string NAME;
		inline WorldObject(std::string name):NAME(name){}
		inline virtual ~WorldObject() = default;
		virtual glm::vec3 pos() = 0;
		virtual glm::vec3 rot() = 0;
		virtual float radius() = 0;
	};

	class Arena;
	class World : public PixFuExtension {

			static std::string TAG;

			TerrainShader *pShader;
			ObjectShader *pShaderObjects;

			glm::mat4 projectionMatrix;

			Light *pLight;
			Camera *pCamera;
		
			std::vector<Terrain *> vTerrains;
			std::vector<ObjectCluster *> vObjects;
			std::map<std::string, ObjectCluster *> mCluesters;

		protected:

			virtual bool init(PixFu *engine);
			virtual void tick(PixFu *engine, float fElapsedTime);
			void add(TerrainConfig_t terrainConfig);
			void add(WorldObject *object, ObjectConfig_t initialTransform = ObjectConfig_t());
	
			template<typename Func>
			void iterateObjects(Func callback) {
				for (ObjectCluster *cluster:vObjects) {
					std::for_each(cluster->vInstances.begin(), cluster->vInstances.end(), callback);
				}
			}



		public:

			const Perspective_t PERSPECTIVE;
			const WorldConfig_t CONFIG;

			static constexpr Perspective_t PERSP_FOV90_LOW = {90, 0.005, 0.1, 0.25};
			static constexpr Perspective_t PERSP_FOV90_MID = {90, 0.005, 100.0, 0.25};
			static constexpr Perspective_t PERSP_FOV90_FAR = {90, 0.5, 1000.0, 0.25};
			static constexpr Perspective_t PERSP_FOV60_LOW = {90, 0.005, 0.1, 0.25};
			static constexpr Perspective_t PERSP_FOV60_MID = {70, 0.005, 1000.0, 0.25};
			static constexpr Perspective_t PERSP_FOV60_FAR = {60, 0.5, 1000.0, 0.25};

			World(WorldConfig_t config, Perspective_t perspective = PERSP_FOV90_LOW);

			virtual ~World();
		
			float getHeight(glm::vec3 &posWorld);

			Camera *camera();

		};

		inline Camera *World::camera() { return pCamera; }


}

#pragma clang diagnostic pop
