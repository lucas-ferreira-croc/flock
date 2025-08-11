#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>

#include "log/logger.hpp"
#include "render/shader.hpp"
#include "render/mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
    Model(const char* path)
    {
        loadModel(path);

        Logger::log("model created");
    }
    
    void render(Shader& shader);
    
private:
    std::vector<Mesh> _meshes;
    std::string _directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    //std::vector<Texture> loadMaterialTexture(aiMaterial* material, aiTextureType type, std::string typeName);
};

#endif