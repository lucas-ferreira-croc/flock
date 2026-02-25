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
    int id;
};

struct Edge
{
    Edge(int vId1, int vId2, int id)
        : vId1(vId1), vId2(vId2), id(id)
    {

    }
    int vId1;
    int vId2;
    int id;
};

struct Face
{
    Face(int vId1, int vId2, int vId3, int id)
        : vId1(vId1), vId2(vId2), vId3(vId3), id(id)
    {
    }
    int vId1;
    int vId2;
    int vId3;
    int id;
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
    virtual void render(Shader& shader);
    
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Edge> edges;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    void updateVertexPosition(int vertexId);
    unsigned int& getVBO() { return _VBO; };
protected:
    unsigned int _VAO, _VBO, _EBO;
    
    void setupMesh();
    void constructFaces();
    void constructEdges();

    void weldVertices(float epsilon = 1e-6f);
};

#endif