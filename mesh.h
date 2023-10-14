#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <GLM/glm.hpp>

#include "Header Files/shader.h"


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoords;
};

struct Texture {
	unsigned int id;
	std::string type;
};


class Mesh {
public:
	//Mesh Data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> texture);
	void Draw(Shader &shader);

private:
	//Render data
	unsigned int VAO, VBO, EBO;
	void SetupMesh();
};

#endif