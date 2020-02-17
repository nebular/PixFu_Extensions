//
//  Terrain.hpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#ifndef Terrain_hpp
#define Terrain_hpp
#include <vector>
#include "OpenGL.h"
#include "Drawable.hpp"
#include "Texture2D.hpp"
#include "Shader.hpp"
#include "glm.hpp"
#include "ext.hpp"

#include "../../headers/TerrainShader.hpp"
#include "../../headers/Camera.hpp"

namespace rgl {

struct RawModel {
	
	GLuint vaoID;
	int vertexCount;
	
};


struct MeshV3 {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
};


class Terrain {
	
	RawModel model;
	
	std::vector<GLuint> vaos;
	std::vector<GLuint> vbos;
	std::vector<GLuint> textures;
	
	static constexpr int VERTEX_COUNT = 128;
	
	float x;
	float z;
	
	Texture2D *texture;
	
public:
	Terrain(rgl::Drawable *text);
	
	float getX();
	
	float getZ();
	
	RawModel generateTerrain(int vertexCount = VERTEX_COUNT);
	RawModel generateTerrain(Drawable *buf, float scale);

	Texture2D *getTexture();
	
	GLuint createVAO();
	RawModel loadToVAO(std::vector<glm::uvec3> &indices, std::vector<MeshV3> &mesh);
	RawModel loadToVAO(float *positions, int posSize, float *textureCoords, int texSize, float *normals, int normSize, GLuint *indices, int indSize);
	
	void storeDataInAttributeList(int attributeNumber, int coordinateSize,float *data, int dataSize);
	
	void unbindVAO();
	
	void bindIndicesBuffer(GLuint *indices, int indicesSize);
	
	void cleanup();
	
	RawModel &getModel();
	
};


}
#endif /* Terrain_hpp */
