#ifndef BODY_HPP
#define BODY_HPP

#include <memory>
#include "glm/glm.hpp"

#include "shape.hpp"
struct Body 
{
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    std::shared_ptr<Shape> shape;

    glm::vec3 sumForces;
    float sumTorque;

    //
    float rotation;
    float angularVelocity;
    float angularAcceleration;
    
    float mass;
    float invMass;
    float I;
    float invI;

    float restituition;
    float friction;

    Body(std::shared_ptr<Shape> shape, float x, float y, float z, float mass);
    ~Body();

    bool IsIstatic() const;

    void AddForce(glm::vec3 force);
    void AddTorque(float torque);

    void ClearForces();
    void ClearTorque();

    void IntegrateForces(const float deltaTime);
    void IntegrateVelocities(const float deltaTime);

    void IntegrateLinear(float deltaTime);
    void IntegrateAngular(float deltaTime);
    
    void ApplyImpulseLinear(glm::vec3& j);
    void ApplyImpulseAngular(float j);
    void ApplyImpulseAtPoint(glm::vec3& j, glm::vec3& r);
};

#endif