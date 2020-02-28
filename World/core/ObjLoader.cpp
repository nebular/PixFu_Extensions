//
//  ObjLoader.cpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ObjLoader.hpp"
#include "Obj_Loader.hpp"

namespace rgl {
	ObjLoader::ObjLoader(std::string filename) {
		pLoader = new objl::Loader();
		if (!pLoader->LoadFile(filename))
			throw std::runtime_error("Problem loading object " + filename);
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
}
