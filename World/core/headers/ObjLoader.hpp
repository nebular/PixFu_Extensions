//
//  ObjectLoader.hpp
//  PixEngine
//
//  Created by rodo on 19/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#include <vector>
#include <string>
#include "Obj_Loader.hpp"

namespace objl {
	class Loader;
}

namespace rgl {

	class ObjLoader {

		objl::Loader *pLoader;

	public:

		ObjLoader(std::string name);

		float *vertices(int mesh = 0);

		unsigned verticesCount(int mesh = 0);

		unsigned *indices(int mesh = 0);

		unsigned indicesCount(int mesh = 0);

		unsigned meshCount();
	};

	ObjLoader::ObjLoader(std::string filename) {
		pLoader = new objl::Loader();
		if (!pLoader->LoadFile(filename))
			throw std::runtime_error("Problem loading object " + filename);
	}

	inline unsigned ObjLoader::meshCount() {
		return pLoader->LoadedMeshes.size();
	}

	inline float *ObjLoader::vertices(int mesh) {
		return (float *) &pLoader->LoadedMeshes[mesh].Vertices[0];
	}

	inline unsigned *ObjLoader::indices(int mesh) {
		return &pLoader->LoadedMeshes[mesh].Indices[0];
	}

	inline unsigned ObjLoader::verticesCount(int mesh) {
		return pLoader->LoadedMeshes[mesh].Vertices.size();
	}

	inline unsigned ObjLoader::indicesCount(int mesh) {
		return pLoader->LoadedMeshes[mesh].Indices.size();
	}
}
