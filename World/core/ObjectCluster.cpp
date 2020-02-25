//
//  World.cpp
//  PixEngine
//
//  Created by rodo on 17/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "World.hpp"

#include "glm.hpp"
#include "ObjectCluster.hpp"

#include "LayerVao.hpp"
#include "ObjLoader.hpp"
#include "Frustum.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "err_typecheck_invalid_operands"
namespace rgl {

glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads, float rzrads, float scale);

std::string ObjectCluster::TAG = "ObjectCluster";

ObjectCluster::ObjectCluster(World *planet, std::string name, ObjectConfig_t normalizeConfiguration)
: NAME(std::move(name)),
PLANET(planet->CONFIG),
WORLD(planet),
PLACER(std::move(normalizeConfiguration)) {
	
	if (DBG) LogV(TAG, "New Object Cluster " + NAME);

	glm::mat4 flipMatrix = glm::identity<glm::mat4>();
	
	/*
	  _            _
								|  1  0  0  0  |
								|  0  1  0  0  |
	 Matrix_Mirrored_On_Z = M * |  0  0 -1  0  |
								|_ 0  0  0  1 _|
	 
	 */
//	flipMatrix[0][0]=-1;
	
	
	pLoader = new ObjLoader("objects/" + NAME + "/"+NAME+".obj");

	for (int i = 0; i<pLoader->meshCount(); i++) {
		if (DBG) LogV(TAG, SF("- texture %s, name %d", NAME.c_str(), i));
		vTextures.push_back(new Texture2D("objects/" + NAME + "/"+NAME+"-"+std::to_string(i)+".png"));
	}
	
	const glm::vec3 &pos = PLACER.position;
	mPlacer = createTransformationMatrix(
										 pos,//CONFIG.position,
										 PLACER.rotation.x, PLACER.rotation.y, PLACER.rotation.z,
										 PLACER.radius
										 ) * flipMatrix;
	
	if (DBG) LogV(TAG, SF("Created ObjectCluster %s", NAME.c_str()));
	
};

void ObjectCluster::add(WorldObject *object, bool setHeight) {
	glm::vec3 pos = object->pos();
	if (setHeight) pos.y  = WORLD->getHeight(pos);
	vInstances.push_back(object);
	if (DBG) LogV(TAG, SF("Add to cluster %s, total %d", NAME.c_str(), vInstances.size()));
}

ObjectCluster::~ObjectCluster() {
	delete pTexture;
	pTexture = nullptr;
	if (DBG) LogV(TAG, SF("Destroyed ObjectCluster %s", NAME.c_str()));
}

void ObjectCluster::render(ObjectShader *shader) {
	
	if (!bInited) init();
	
	int frustumHits = 0;	
	
	bool oneMesh= vMeshes.size() == 1;

	if (oneMesh) {
		shader->textureUnit("modelTexture", vTextures[0]);
		shader->loadShineVariables(1, 0.7);
		vTextures[0]->bind();
		bind(0);
	}

	for (WorldObject *object : vInstances) {
		
		// cache object properties
		glm::vec3 rot = object->rot();
		glm::vec3 pos = object->pos();
		
		float radius = object->radius() / 1000;

		// our shader has a Frustum class initialized already initialized with the
		// projection & view matrix, so we can now trivially check if an object will be
		// visible.

		Frustum *f = shader->frustum();
		if (f == nullptr || f->IsBoxVisible(pos - radius, pos + radius)) {

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

				// VAO thunder
				draw(0, false);
				
			} else {
				
				Visible_t v = { object, tmatrix * mPlacer };
				// save the transform matrix as we will use it several times to draw he individual meshes
				vVisibles.push_back(v);
			}
		} else {
			// we saved drawing this object
			frustumHits++;
		}
	}

	if (oneMesh) {
		unbind();
		return;
	}
	
	// multi-mesh
	// draw the meshes
	
	for (int i=0; i<vMeshes.size(); i++) {
	
		shader->textureUnit("modelTexture", vTextures[i]);
		shader->loadShineVariables(1, 0.7);
		vTextures[i]->bind();
		bind(i);

		for (Visible_t visible: vVisibles) {

			// load the transformation matrix
			shader->loadTransformationMatrix(visible.transformMatrix);

			// VAO thunder
			draw(i, false);

		}
		unbind();
	}
	vVisibles.clear();
//	if (DBG) LogV(TAG, SF("Frustum Hits %d", frustumHits));
}

void ObjectCluster::init() {
	
	for (int i = 0; i< pLoader->meshCount(); i++) {
		LayerVao::add(
					pLoader->vertices(i), pLoader->verticesCount(i),
					pLoader->indices(i), pLoader->indicesCount(i));
	
		vTextures[i]->upload();
	}
	bInited = true;
	
}

};


#pragma clang diagnostic pop