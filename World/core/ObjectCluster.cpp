//
//  ObjectCluster.cpp
//  PixFu Engine
//
//  An objectCluster groups objcts of the same type. They share mesh and material, so
//  are rendered very fast one after another, after configuring the mesh and textures
// 	one time only.
//
//  The World Object automatically creates clusters as new objects are added.
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"
#include "Frustum.hpp"
#include "LayerVao.hpp"
#include "ObjLoader.hpp"
#include "ObjectCluster.hpp"
#include "Camera.hpp"
#include "Config.hpp"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "Fu.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCSimplifyInspection"
#pragma ide diagnostic ignored "OCDFAInspection"
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"

namespace Pix {

	std::string ObjectCluster::TAG = "ObjectCluster";

	glm::mat4
	createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale = 1.0f, bool flipX = false,
							   bool flipY = false, bool flipZ = false);

	ObjectCluster::ObjectCluster(World *planet, std::string name, Transformation_t initialTransform)
			: PLANET(planet->CONFIG),
			  PLACER(initialTransform),
			  NAME(std::move(name)),
			  WORLD(planet) {

		if (DBG) LogV(TAG, "New Object Cluster " + NAME);

		// load object model
		pLoader = new ObjLoader(std::string(PATH_OBJECTS) + "/" + NAME + "/" + NAME + ".obj");

		for (int i = 0; i< pLoader->meshCount(); i++) {
		  pLoader->material(i).init(NAME);
		}

		mPlacer = PLACER.toMatrix();
		vInstances.clear();

		if (DBG) LogV(TAG, SF("Created ObjectCluster %s", NAME.c_str()));

	};

	void ObjectCluster::add(WorldObject *object) {
//		glm::vec3 pos = object->pos();
// TODO	if (setHeight) pos.y = WORLD->getHeight(pos);
		vInstances.emplace_back(object);
		if (DBG) LogV(TAG, SF("Add to cluster %s, total %d", NAME.c_str(), vInstances.size()));
	}

	ObjectCluster::~ObjectCluster() {
		delete pLoader;
		if (DBG) LogV(TAG, SF("Destroyed ObjectCluster %s", NAME.c_str()));
	}

	void ObjectCluster::render(ObjectShader *shader, Camera *camera) {

		if (!bInited) init();

		shader->setFloat("iTime", (float) Fu::METRONOME);

		int frustumHits = 0;

		bool oneMesh = vMeshes.size() == 1;

		Frustum *frustum = camera->getFrustum();

		if (oneMesh) {

			constexpr int MESH = 0;
			
			Material& material = pLoader->material(MESH);
			shader->loadMaterial(material);
			shader->bindMaterial(material);
			bind(0);
		}

		for (WorldObject *object : vInstances) {

			// cache object properties
			glm::vec3 rot = object->rot();
			glm::vec3 pos = object->pos() / 1000.0F;

			float radius = object->drawRadius();

			if (frustum == nullptr || frustum->IsBoxVisible(pos - radius, pos + radius)) {

				// the object is visible, so transform its model to the desired rotation,
				// position and radius

				glm::mat4 tmatrix = createTransformationMatrix(
						pos, //CONFIG.position,
						rot.x, rot.y, rot.z,
						radius
				);

				if (oneMesh) {

					// only one mesh
					glm::mat4 result = tmatrix * mPlacer;
					// load the transformation matrix
					shader->loadTransformationMatrix(result);

					// Set object tint
					shader->setTint(object->isSelected() ? WorldObject::TINT_SELECT:object->tintCode());

					// VAO thunder
					draw(0, false);

				} else {

					Visible_t v = {object, tmatrix * mPlacer};
					// save the transform matrix as we will use it several times to draw he individual meshes
					vVisibles.emplace_back(v);
				}
			} else {
				// we saved drawing this object
				frustumHits++;
			}
		}

		if (oneMesh) {
			unbind();
			if (DBG) LogV(TAG, SF("Frustum Hits %d", frustumHits));
			return;
		}

		// multi-mesh
		// draw the meshes

		for (int i = 0; i < vMeshes.size(); i++) {

			Material& material = pLoader->material(i);
			shader->loadMaterial(material);
			shader->bindMaterial(material);
			bind(i);

			for (Visible_t visible: vVisibles) {

				// load the transformation matrix
				shader->loadTransformationMatrix(visible.transformMatrix);

				// Set object tint
				shader->setTint(visible.object->isSelected() ? WorldObject::TINT_SELECT:visible.object->tintCode());

				// VAO thunder
				draw(i, false);

			}
			unbind();
		}
		vVisibles.clear();
		if (DBG) LogV(TAG, SF("Frustum Hits %d", frustumHits));
	}

	void ObjectCluster::init() {

		for (int i = 0, l = pLoader->meshCount(); i < l; i++) {
			LayerVao::add(
					pLoader->vertices(i), pLoader->verticesCount(i),
					pLoader->indices(i), pLoader->indicesCount(i)
			);
			pLoader->material(i).upload();
		}
		bInited = true;
	}
};


#pragma clang diagnostic pop
