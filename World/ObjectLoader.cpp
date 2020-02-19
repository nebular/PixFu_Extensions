//
//  ObjectLoader.cpp
//  PixEngine
//
//  Created by rodo on 19/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "ObjectLoader.hpp"
#include "Obj_Loader.hpp"

namespace rgl {

	ObjectLoader::ObjectLoader(std::string filename) {
		pLoader = new objl::Loader();
		if (!pLoader->LoadFile(filename))
			throw std::runtime_error("Problem loading object " + filename);
	}

	float *ObjectLoader::vertices() {
		return (float *) &pLoader->LoadedVertices[0];
	}

	unsigned *ObjectLoader::indices() {
		return &pLoader->LoadedIndices[0];
	}

	unsigned ObjectLoader::verticesCount() {
		return pLoader->LoadedVertices.size();
	}

	unsigned ObjectLoader::indicesCount() {
		return  pLoader->LoadedIndices.size();
	}

}
