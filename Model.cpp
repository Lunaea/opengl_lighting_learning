#include "Model.h"

void Model::Draw(Shader& shader)
{
    for(u_int i = 0; i < meshes.size(); ++i)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer{};
    const aiScene* scene{ importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs) };

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for(u_int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(u_int i = 0; i < node->mNumChildren; ++i)
        processNode(node->mChildren[i], scene);
}

Mesh processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices{};
    std::vector<unsigned int> indices{};
    std::vector<Texture> textures{};

    for(u_int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex{};
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.Position = vector;

        vector = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        vertex.Normal = vector;

        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f);

        vertices.push_back(vertex);
    }
    // process indices
    for(u_int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face{ mesh->mFaces[i] };
        for(u_int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material{ scene->mMaterials[mesh->mMaterialIndex] };
        std::vector<Texture> diffuseMaps{ loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse") };
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps{ loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular") };
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures{};
    for(u_int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str{};
        mat->GetTexture(type, i, &str);
        Texture texture{};
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str;
        textures.push_back(texture);
    }

    return textures;
}
