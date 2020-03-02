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

#include "WorldMeta.hpp"

namespace objl {
	class Loader;
}

namespace Pix {

	class ObjLoader {

		objl::Loader *pLoader;

	public:

		ObjLoader(std::string name);
		ObjLoader(const Static3DObject_t *object);

		float *vertices(int mesh = 0);

		unsigned verticesCount(int mesh = 0);

		unsigned *indices(int mesh = 0);

		unsigned indicesCount(int mesh = 0);

		unsigned meshCount();
	};

}
