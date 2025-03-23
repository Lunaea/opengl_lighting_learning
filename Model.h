#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "Mesh.h"
#include "shader.h"
#include "stb_image.h"

unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
    Model(const std::string& path) { loadModel(path); }
    void Draw(Shader& shader);

private:
    // model data
    std::vector<Texture> textures_loaded{};
    std::vector<Mesh> meshes{};
    std::string directory{};

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif
