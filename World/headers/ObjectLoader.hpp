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

namespace objl {
	class Loader;
}

namespace rgl {

	class ObjectLoader {

		objl::Loader *pLoader;

	public:

		ObjectLoader(std::string name);

		float *vertices();

		unsigned verticesCount();

		unsigned *indices();

		unsigned indicesCount();
	};
}
