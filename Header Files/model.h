#ifndef MODEL_H
#define MODEL_H

#include <ASSIMP/scene.h>

#include <vector>

#include "shader.h"
#include "mesh.h"

class Model
{
public:
	//Model data
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	Model(std::string const& path);
	void Draw(Shader &shader);

private:

	void LoadModel(std::string const& path);
	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif