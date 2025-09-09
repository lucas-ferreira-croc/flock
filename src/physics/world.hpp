#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <memory>

#include "body.hpp"
#include <glm/glm.hpp>

class World
{
public:
    World(float gravity);
    ~World();

    void AddBody(std::shared_ptr<Body> body);
    std::vector<std::shared_ptr<Body>>& GetBodies();

    void AddForce(const glm::vec3& force);
    void AddTorque(float torque);
    
    //void AddConstraint(std::shared_ptr<Constraint> constraint);
    //std::vector<std::shared_ptr<Constriant>>& GetConstraints();

    void Update(float deltaTime);
    private:
    float gravity;
    
    std::vector<std::shared_ptr<Body>> bodies;
    //std::vector<std::shared_ptr<Constraint>> constraints;
    
    std::vector<glm::vec3> forces;
    std::vector<float> torques;
    void CheckCollisions();
};

#endif