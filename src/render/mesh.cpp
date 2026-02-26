#include "mesh.hpp"
#include <unordered_map>

#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures)
    : vertices(vertices), indices(indices), textures(textures)
{
    weldVertices();
    setupMesh();
    constructFaces();
    constructEdges();
}

void Mesh::weldVertices(float epsilon)
{
    std::vector<Vertex> newVertices;
    std::vector<unsigned int> newIndices;

    std::unordered_map<uint64_t, unsigned int> positionsMap;

    auto makePositionKey = [&](const glm::vec3& p)
    {
        int x = static_cast<int>(std::round(p.x / epsilon));
        int y = static_cast<int>(std::round(p.y / epsilon));
        int z = static_cast<int>(std::round(p.z / epsilon));

        return (static_cast<uint64_t>(x) << 42) ^
               (static_cast<uint64_t>(y) << 21) ^
               static_cast<uint64_t>(z);
    }; 

    
    std::unordered_map<uint64_t, unsigned int> positionMap;
    for (unsigned int i = 0; i < indices.size(); i++)
    {
        unsigned int oldIndex = indices[i];
        const Vertex& v = vertices[oldIndex];

        uint64_t key = makePositionKey(v.position);

        if (positionMap.find(key) == positionMap.end())
        {
            unsigned int newIndex = static_cast<unsigned int>(newVertices.size());

            Vertex newVertex = v;
            newVertex.id = newIndex;

            newVertices.push_back(newVertex);
            positionMap[key] = newIndex;

            newIndices.push_back(newIndex);
        }
        else
        {
            newIndices.push_back(positionMap[key]);
        }
    }

    vertices = std::move(newVertices);
    indices = std::move(newIndices);
}


void Mesh::constructFaces()
{
    faces.clear();
    for(int i = 0; i < indices.size(); i+= 3)
    {
        int vId1 = indices[i];
        int vId2 = indices[i + 1];
        int vId3 = indices[i + 2];
        Face face(vId1, vId2, vId3, i / 3);
        faces.push_back(face);
    }

}

void Mesh::constructEdges()
{
    edges.clear();

    std::unordered_map<uint64_t, int> edgeMap;

    auto makeKey = [](int a, int b)
    {
        int minId = std::min(a, b);
        int maxId = std::max(a, b);
        return (static_cast<uint64_t>(minId) << 32) | static_cast<uint64_t>(maxId);
    };

    int edgeId = 0;
    for(const auto& face : faces)
    {
        int tri[3][2] = {
            {face.vId1, face.vId2},
            {face.vId2, face.vId3},
            {face.vId3, face.vId1}
        };

        for(int i = 0; i < 3; i++)
        {
            int a = tri[i][0];
            int b = tri[i][1];
            
            uint64_t key = makeKey(a, b);

            if(edgeMap.find(key) == edgeMap.end())
            {
                edges.emplace_back(a, b, edgeId);
                edgeMap[key] = edgeId;
                edgeId++;
            }
        }
    }
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::render(Shader& shader)
{   
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    if(textures.size() > 0)
    {
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);
    
            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
    }
    else{
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::updateVertexPosition(int vertexId)
{
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    
    glBufferSubData(GL_ARRAY_BUFFER, 
    vertexId * sizeof(Vertex),
    sizeof(glm::vec3),
    &vertices[vertexId].position
    );
}
