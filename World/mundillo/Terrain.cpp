//
//  Terrain.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "Planet.hpp"

namespace rgl {

std::string Terrain::TAG = "Terrain";

Terrain::Terrain(rgl::Drawable *text) {
	x = 0 * text->width;
	z = 0 * text->height;
	texture = new Texture2D(text);
	model = generateTerrain(text, 1.0);
	
	texture->upload();
}

float Terrain::getX() {
	return x;
}

float Terrain::getZ() {
	return z;
}

Texture2D *Terrain::getTexture() { return texture; }

RawModel Terrain::generateTerrain(Drawable *buf, float scale) {

	std::vector<MeshV3> mesh;

	int imageWidth = buf->width, imageHeight = buf->height;
	
	//  Generate Vertices and texture coordinates
	for (int row = 0; row <= imageHeight; row++)
	{
		for (int column = 0; column <= imageWidth; column++)
		{
			float x = (float)column / (float)imageWidth;
			float y = (float)row / (float)imageHeight;
			
			float pixel = buf->getPixel(column, row).a / 255;
			
			float z;
			if (row == imageHeight || column == imageWidth || row == 0 || column == 0)
			{
				z = 0.0f;
			}
			else
			{
				z = float(pixel / 256.0)*scale;
			}
			
			MeshV3 meshItem = {glm::vec3(x, y, z),glm::vec3(0.0, 0.0, 0.0),glm::vec2(x, y)};
			
			mesh.push_back(meshItem);
			
		}
	}
	
	std::vector<glm::uvec3> indices;
	
	// Generate indices
	for (int row = 0; row < imageHeight; row++)
	{
		for (int column = 0; column < imageWidth; column++)
		{
			int row1 = row * (imageWidth + 1);
			int row2 = (row + 1) * (imageWidth + 1);
			
			// triangle 1
			indices.push_back(glm::uvec3(row1 + column, row1 + column + 1, row2 + column + 1));
			
			// triangle 2
			indices.push_back(glm::uvec3(row1 + column, row2 + column + 1, row2 + column));
		}
	}
	
	// Generate normals
	for (int i = 0; i < indices.size(); i++)
	{
		glm::vec3 v1 = mesh[indices[i].x].position;
		glm::vec3 v2 = mesh[indices[i].y].position;
		glm::vec3 v3 = mesh[indices[i].z].position;
		
		glm::vec3 edge1 = v1 - v2;
		glm::vec3 edge2 = v1 - v3;
		glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
		
		mesh[indices[i].x].normal += normal;
		mesh[indices[i].y].normal += normal;
		mesh[indices[i].z].normal += normal;
	}
	return loadToVAO(indices, mesh);
}

RawModel Terrain::generateTerrain(int vertexCount){
	
	const int count = vertexCount * vertexCount;
	const int verticesSize = count * 3;
	const int normalsSize = count * 3;
	const int texturesSize = count * 2;
	const int indicesSize = 6*(vertexCount-1)*(vertexCount-1);
	
	float *vertices=new float[verticesSize] {0};
	float *normals=new float [normalsSize] {0};
	float *textureCoords=new float [count*2] {0};
	GLuint *indices=new GLuint[indicesSize] {0};
	int vertexPointer = 0;
	
	for(int i=0;i<vertexCount;i++){
		for(int j=0;j<vertexCount;j++){
			vertices[vertexPointer*3] = (float)j/((float)vertexCount - 1) * texture->width();
			vertices[vertexPointer*3+1] = 0;
			vertices[vertexPointer*3+2] = (float)i/((float)vertexCount - 1) * texture->height();
			normals[vertexPointer*3] = 0;
			normals[vertexPointer*3+1] = 1;
			normals[vertexPointer*3+2] = 0;
			textureCoords[vertexPointer*2] = (float)j/((float)vertexCount - 1);
			textureCoords[vertexPointer*2+1] = (float)i/((float)vertexCount - 1);
			vertexPointer++;
		}
	}
	
	int pointer = 0;
	for(int gz=0;gz<vertexCount-1;gz++){
		for(int gx=0;gx<vertexCount-1;gx++){
			int topLeft = (gz*vertexCount)+gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz+1)*vertexCount)+gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}
	return loadToVAO(vertices, verticesSize, textureCoords, texturesSize, normals, normalsSize, indices, indicesSize);
}

RawModel Terrain::loadToVAO(std::vector<glm::uvec3> &indices, std::vector<MeshV3> &mesh) {

	GLuint vaoID = createVAO();

	int indSize = (int)indices.size() * 3 * sizeof(GLuint);
	
	bindIndicesBuffer((GLuint*)&indices[0], indSize);

	GLuint vboID;
	glGenBuffers(1, &vboID);
	vbos.push_back(vboID);
	
	float *tortilla = (float*)&mesh[0];
	int tortillaSize = (int)mesh.size() * (3+3+2);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, tortillaSize, tortilla, GL_STATIC_DRAW);
	
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
	glEnableVertexAttribArray(0);
	
	// normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
						  (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
						  (void *) (6 * sizeof(float)));
	
	glEnableVertexAttribArray(2);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return {vaoID, indSize};

}

RawModel Terrain::loadToVAO(float *positions, int posSize, float *textureCoords, int texSize, float *normals, int normSize, GLuint *indices, int indSize){
	GLuint vaoID = createVAO();
	bindIndicesBuffer(indices, indSize);
	storeDataInAttributeList(0,3,positions, posSize);
	storeDataInAttributeList(1,2,textureCoords, texSize);
	storeDataInAttributeList(2,3,normals, normSize);
	unbindVAO();
	return {vaoID, indSize};
}

GLuint Terrain::createVAO(){
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	vaos.push_back(vaoID);
	glBindVertexArray(vaoID);
	return vaoID;
}

void Terrain::storeDataInAttributeList(int attributeNumber, int coordinateSize,float *data, int dataSize){
	GLuint vboID;
	glGenBuffers(1, &vboID);
	vbos.push_back(vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	
	glVertexAttribPointer(attributeNumber,coordinateSize,GL_FLOAT,GL_FALSE, 0,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::unbindVAO(){
	glBindVertexArray(0);
}

void Terrain::bindIndicesBuffer(GLuint *indices, int indicesSize) {
	GLuint vboID;
	glGenBuffers(1, &vboID);
	vbos.push_back(vboID);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
}

void Terrain::cleanup() {
	
	for(GLuint vao:vaos){
		glDeleteVertexArrays(1, &vao);
	}
	for(GLuint vbo:vbos){
		glDeleteBuffers(1, &vbo);
	}
	for(GLuint texture:textures){
		glDeleteTextures(1, &texture);
	}
}

RawModel &Terrain::getModel() {
	return model;
}


}
