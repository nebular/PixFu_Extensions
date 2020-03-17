//
//  ObjLoader.cpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ObjLoader.hpp"
#include "Obj_Loader.hpp"
#include "Fu.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "RedundantCast"

namespace Pix {

	// Load an OBJ Model
	ObjLoader::ObjLoader(std::string filename) : pLoader(new objl::Loader()) {
		if (!pLoader->LoadFile(Pix::FuPlatform::getPath(filename)))
			throw std::runtime_error("Problem loading object " + filename);
	}

	// Load a static mesh
	ObjLoader::ObjLoader(const Static3DObject_t *staticObject) : pLoader(new objl::Loader()) {

		std::vector<unsigned> *Indices = new std::vector<unsigned>(staticObject->indices,
																   staticObject->indices + staticObject->indicesCount);

		std::vector<objl::Vertex> *Vertices = new std::vector<objl::Vertex>((objl::Vertex *) staticObject->vertices,
																			(objl::Vertex *) (staticObject->vertices +
																							  staticObject->verticesCount));

		pLoader->LoadObject(*Vertices, *Indices);

	}

	ObjLoader::~ObjLoader() {
		delete pLoader;
	}

	unsigned ObjLoader::meshCount() {
		return (unsigned) pLoader->LoadedMeshes.size();
	}

	float *ObjLoader::vertices(int mesh) {
		return (float *) &pLoader->LoadedMeshes[mesh].Vertices[0];
	}

	unsigned *ObjLoader::indices(int mesh) {
		return &pLoader->LoadedMeshes[mesh].Indices[0];
	}

	unsigned ObjLoader::verticesCount(int mesh) {
		return (unsigned) pLoader->LoadedMeshes[mesh].Vertices.size();
	}

	unsigned ObjLoader::indicesCount(int mesh) {
		return (unsigned) pLoader->LoadedMeshes[mesh].Indices.size();
	}

	Material& ObjLoader::material(int mesh) {
		return pLoader->LoadedMeshes[mesh].MeshMaterial;
	}

	std::vector<Material>& ObjLoader::materials() {
		return pLoader->LoadedMaterials;
	}
}

#pragma clang diagnostic pop
