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
    int maxIterations = 10;

    unsigned int vao;
    unsigned int vbo;
    std::shared_ptr<Shader> shader;

    FABRIKComponent(std::vector<glm::vec3> joints = std::vector<glm::vec3>(), const glm::vec3& target = glm::vec3(0.0f))
    {
        if(joints.size() != 0)
        {
            this->target = target;
            this->joints = joints;
            this->origin = joints[0];
            totalLength = 0.0f;
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

    void backward() 
    {
        joints.back() = target;

        // end effector as target;
        for(int i = (int)joints.size() - 2; i >= 0; i--) 
        {
            glm::vec3 jointNextJoint = joints[i + 1] - joints[i]; 
            float dist = glm::length(jointNextJoint);
            if(dist <= 1e-6f)
            {
                continue;
            }
            
            float lambda = lengths[i] / dist;
            // find new position
            glm::vec3 position = (1.0f - lambda) * joints[i + 1] + lambda * joints[i];
            joints[i] = position;
        }
    }


    void forward() 
    {
        // forward reaching, root at initial position
        joints[0] = origin;

        for(int i = 0; i < joints.size() - 1; i++)
        {
            glm::vec3 jointNextJoint = (joints[i + 1] - joints[i]);
            float dist = glm::length(jointNextJoint);
            if(dist <= 1e-6f)
            {
                continue;
            }

            float lambda = lengths[i] / dist;

            // find new position
            glm::vec3 position = (1.0f - lambda) * joints[i] + lambda * joints[i + 1];
            joints[i + 1] = position;
        }
    }

    void solve(glm::vec3& position)
    {
        float distance = glm::length(joints[0] - target);
        if(distance > totalLength) 
        {
            for(int i = 0; i < joints.size() -1; i++)
            {
                float jointTarget = glm::length(target - joints[i]);
                float lambda = lengths[i] / jointTarget;

                joints[i + 1] = (1.0f - lambda) * joints[i] + lambda * target;
            }
            Logger::warning("computing ik for out of reach target");
        }
        else
        {
            int bCount = 0;
            float dif = glm::length(joints.back() - target);
            while (dif > tolerance && bCount < maxIterations)
            {
                backward();
                forward();
                dif = glm::length(joints.back() - target);
                bCount++;
            }
            Logger::err("computing ik for inbouds target");
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, joints.size() * sizeof(glm::vec3), joints.data());
    }
};

#endif