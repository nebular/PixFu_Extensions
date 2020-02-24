//
//  Terrain.hpp
//  PixEngine
//
//  Created by rodo on 25/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once
#include "Canvas2D.hpp"
#include "Texture2D.hpp"
#include "LayerVao.hpp"
#include "ObjLoader.hpp"
#include "TerrainShader.hpp"


namespace rgl {


//////////////////////////////////////////////////////////////////////////////////////////

	class Terrain : public LayerVao {

		static std::string TAG;

		static constexpr float VERTICES[32]{
				// positions          // colors           // texture coords
				1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
				1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
				-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
		};

		static constexpr unsigned int INDICES[6]{
				0, 1, 3, // first triangle
				1, 2, 3  // second triangle
		};

		Canvas2D *pDirtCanvas;
		Texture2D *pDirtTexture;
		Texture2D *pTexture;
		Drawable *pHeightMap;
		ObjLoader *pLoader;

		float scaletest = 0.1;
		
		bool bInited = false;

		void initMesh(unsigned vertexCount, unsigned twidth, unsigned theight);

	public:

		const TerrainConfig_t CONFIG;
		const WorldConfig_t PLANET;

		glm::vec2 mSize;

		Terrain(WorldConfig_t planetConfig, TerrainConfig_t config);

		virtual ~Terrain();

		void init();

		void render(TerrainShader *shader);

		float getHeight(glm::vec3 &posWorld);

		bool contains(glm::vec3 &posWorld);

		Canvas2D *canvas();

	};

}
