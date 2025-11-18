#ifndef FABRIK_COMPONENT_HPP
#define FABRIK_COMPONENT_HPP

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "log/logger.hpp"
#include "render/shader.hpp"

struct FABRIKComponent
{
    std::vector<glm::vec3> joints;
    std::vector<float> lengths;
    glm::vec3 target;
    glm::vec3 origin;

    float tolerance = 0.1f;
    float totalLength;

    unsigned int vao;
    unsigned int vbo;
    std::shared_ptr<Shader> shader;

    FABRIKComponent(std::vector<glm::vec3> joints = std::vector<glm::vec3>(), const glm::vec3& target = glm::vec3(0.0f))
    {
        this->target = target;
        this->joints = joints;
        totalLength = 0.0f;
        if(joints.size() != 0)
        {
            for(int i = 0; i < joints.size() - 1; i++)
            {   
                lengths.push_back(glm::length(joints[i] - joints[i + 1]));
            }
            
            for(auto length : lengths)
            {
                totalLength += length;
            }

            glGenVertexArrays(1, & vao);
            glGenBuffers(1, &vbo);
            
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, joints.size() * sizeof(glm::vec3), joints.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

            glBindVertexArray(0);

            shader = std::make_shared<Shader>();
            shader->createFromFile("C:\\dev\\shader\\flock\\assets\\shaders\\v_simple.glsl", "C:\\dev\\shader\\flock\\assets\\shaders\\f_simple.glsl");
        }
    }

    void solve(glm::vec3& position)
    {
        float distance = glm::length(joints[0] - target);
        if(distance > totalLength) 
        {
            for(int i = 0; i < joints.size() -1; i++)
            {
                float r = glm::length(target - joints[i]);
                float lambda = lengths[i] / r;

                joints[i + 1] = (1 - lambda) * joints[i] + lambda * target;
                Logger::log("solving IK when distance > totalLength");
            }
        }
        Logger::err("x_target = " + std::to_string(target.x) + "; y = " + std::to_string(target.y) + "; z = " + std::to_string(target.z));
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, joints.size() * sizeof(glm::vec3), joints.data());
    }
};

#endif