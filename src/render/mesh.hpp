#ifndef MESH_HPP
#define MESH_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include "render/shader.hpp"
#include "texture/texture.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct MeshTexture
{
    unsigned int id;
    std::string type;
    std::string path;
    std::shared_ptr<Texture> texture;
};

class Mesh
{
public:

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures);
    void render(Shader& shader);
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

private:
    unsigned int _VAO, _VBO, _EBO;
    
    void setupMesh();
};

#endif