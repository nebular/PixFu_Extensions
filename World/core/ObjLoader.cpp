//
//  ObjectLoader.cpp
//  PixEngine
//
//  Created by rodo on 19/02/2020.
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

	float *ObjLoader::vertices() {
		return (float *) &pLoader->LoadedVertices[0];
	}

	unsigned *ObjLoader::indices() {
		return &pLoader->LoadedIndices[0];
	}

	unsigned ObjLoader::verticesCount() {
		return (unsigned)pLoader->LoadedVertices.size();
	}

	unsigned ObjLoader::indicesCount() {
		return  (unsigned)pLoader->LoadedIndices.size();
	}

}
