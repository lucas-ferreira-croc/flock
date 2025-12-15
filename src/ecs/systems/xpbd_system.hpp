#ifndef XPBD_SYSTEM_HPP
#define XPBD_SYSTEM_HPP

#include "ecs/ecs.hpp"
#include "ecs/components/cloth_component.hpp"
#include "ecs/components/mesh_component.hpp"

class XPBDSystem : public System
{
public:
    
    XPBDSystem()
    {
        requireComponent<ClothComponent>();
    }

    
const int solverIters = 8;
    void Update(float dt)
    {
        float subDt = dt / float(numSubsteps);

        for(int i = 0; i < numSubsteps; i++)
        {
            for(auto& entity : getSystemEntities())
            {
                auto& cloth = entity.getComponent<ClothComponent>();
                cloth.preSolve(subDt, gravity);
                // cloth.solveGroundCollisions();
                for(int it = 0; it < solverIters; it++)
                    cloth.solve(subDt);
                cloth.postSolve(subDt);

                auto& mesh = entity.getComponent<MeshComponent>();
                auto vbo = mesh.model->getMeshes()[0].getVBO();
                 auto& meshComponent = entity.getComponent<MeshComponent>();

               
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                GLint bufferSize = 0;
                glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
                size_t numVertices = bufferSize / sizeof(Vertex);
                std::vector<Vertex> vertexData(numVertices);

                glGetBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, vertexData.data());
                
                // if(firstTime)
                // {
                //     std::cout << "oiii";
                //     std::cout << vertexData.size() << "\n";
                //     for(auto vertice : vertices)
                //     {
                //         std::cout << "vertice.position.x = " << vertice.x << ";vertice.position.y = " << vertice.y << "vertice.position.z = " << vertice.z << "\n"; 
                //     }
                //     firstTime = false;
                // }

                for(int i = 0, j = 0; i < cloth._positions.size(); i+=3, j++)
                {
                    vertexData[j].position.x = cloth._positions[i + 0];
                    vertexData[j].position.y = cloth._positions[i + 1];
                    vertexData[j].position.z = cloth._positions[i + 2];
                }
                // for (int i = 0; i < numVertices ; i++)
                // {
                //     vertexData[i].position.y -= 0.001f;
                // }
                glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, vertexData.data());
                glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        }
    }
    int start = 1;
    int step = 3;

    int numSubsteps = 16;
    glm::vec3 gravity{0.0f, -0.981f, 0.0f};
    bool firstTime = true;
};

#endif