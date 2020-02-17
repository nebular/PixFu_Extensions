//
//  TerrainRenderer.cpp
//  PixEngine
//
//  Created by rodo on 16/02/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "TerrainRenderer.hpp"
namespace rgl {


glm::mat4 createTransformationMatrix(glm::vec3 translation, float rxrads, float ryrads,
									 float rzrads, float scale) {
	
	glm::mat4 matrix = glm::identity<glm::mat4>();
	
	matrix = glm::translate(matrix, translation);
	matrix = glm::rotate(matrix, rxrads, {1,0,0});
	matrix = glm::rotate(matrix, ryrads, {0,1,0});
	matrix = glm::rotate(matrix, rzrads, {0,0,1});
	matrix = glm::scale(matrix, {scale,scale,scale});
	return matrix;
}


TerrainRenderer::TerrainRenderer(TerrainShader *shader, glm::mat4 &projectionMatrix) {
	pShader = shader;
	pShader->use();
	pShader->loadProjectionMatrix(projectionMatrix);
	pShader->stop();
	OpenGlUtils::glError("terrain");

}

void TerrainRenderer::render(Terrain *terrain) {
	prepareTerrain(terrain);
	loadModelMatrix(terrain);
	glDrawElements(GL_TRIANGLES, terrain->getModel().vertexCount, GL_UNSIGNED_INT, 0);
	
	unbindTexturedModel();
	OpenGlUtils::glError("render");
}

void TerrainRenderer::prepareTerrain(Terrain *terrain) {
	RawModel rawModel = terrain->getModel();
	glBindVertexArray ( rawModel.vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	Texture2D *texture = terrain->getTexture();
	pShader->loadShineVariables(1,0);
	pShader->textureUnit("modelTexture", texture);
	texture->bind();
}

void TerrainRenderer::unbindTexturedModel() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

void TerrainRenderer::loadModelMatrix(Terrain *terrain) {
	
	glm::mat4 tmatrix = createTransformationMatrix(
												   {terrain->getX(), 0, terrain->getZ()},
												   0, 0, 0, 1
												   );
	pShader->loadTransformationMatrix(tmatrix);
}


}

