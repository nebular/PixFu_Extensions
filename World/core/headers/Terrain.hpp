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

		Canvas2D *pDirtCanvas = nullptr;
		Texture2D *pDirtTexture = nullptr;
		Texture2D *pTexture = nullptr;
		Drawable *pHeightMap = nullptr;
		ObjLoader *pLoader = nullptr;

		/** Terrain Size */
		glm::vec2 mSize;

		bool bInited = false;

		void init();

		void initMesh(unsigned vertexCount, unsigned twidth, unsigned theight);

	public:

		const TerrainConfig_t CONFIG;
		const WorldConfig_t PLANET;
		
		Terrain(WorldConfig_t planetConfig, TerrainConfig_t config);

		virtual ~Terrain();

		/** Renders the terrain */
		void render(TerrainShader *shader);

		/** Queries heightmap */
		float getHeight(glm::vec3 &posWorld);

		/** Whether the absolute coordinates belong to this terrain (mult-terrain world) */
		bool contains(glm::vec3 &posWorld);

		/** draws a debug grid */
		void wireframe(int inc=100);
		
		/** Canvas rendered over the 3D texture */
		Canvas2D *canvas();

	};


	inline float Terrain::getHeight(glm::vec3 &posWorld3d) {
		return (pHeightMap != nullptr)
			? CONFIG.scaleHeight * 1000 * pHeightMap->getPixel(posWorld3d.x - CONFIG.origin.x, posWorld3d.z - CONFIG.origin.y).r / (float) 255
			: 0;
	}

	inline bool Terrain::contains(glm::vec3 &posWorld) {
		return posWorld.x >= CONFIG.origin.x
			   && posWorld.z >= CONFIG.origin.y
			   && posWorld.x <= CONFIG.origin.x + mSize.x
			   && posWorld.z <= CONFIG.origin.y + mSize.y;
	}

	inline Canvas2D *Terrain::canvas() {return pDirtCanvas;}

}