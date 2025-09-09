#include "world.hpp"

World::World(float gravity)
    : gravity(-gravity)
{
}

World::~World()
{

}

void World::AddBody(std::shared_ptr<Body> body)
{
    bodies.push_back(body);
}

std::vector<std::shared_ptr<Body>>& World::GetBodies()
{
    return bodies;
}

void World::AddForce(const glm::vec3& force)
{
    forces.push_back(force);
}

void World::AddTorque(float torque)
{
    torques.push_back(torque);
}

//void AddConstraint(std::shared_ptr<Constraint> constraint);
//std::vector<std::shared_ptr<Constriant>>& GetConstraints();

void World::Update(float deltaTime)
{
    
    for(auto body : bodies)
    {
        glm::vec3 weight = glm::vec3(0.0f, body->mass * gravity, 0.0f);
        body->AddForce(weight);
        
        for(auto force : forces)
        {
            body->AddForce(force);
        }
        
        for(auto torque : torques)
        {
            body->AddTorque(torque);
        }
    }
    
    
    for(auto body : bodies)
    {
        body->IntegrateForces(deltaTime);    
    }

    //std::vector<PenetrationConstraint> penetrations;
    for(int i = 0; i <= bodies.size(); i++)
    {
        for(int j = i + 1; j < bodies.size(); j++)
        {
            std::shared_ptr<Body> a = bodies[i];
            std::shared_ptr<Body> b = bodies[j];

            // find and solve collision
        }
    }

    // constraints


    for(auto body : bodies)
    {
        body->IntegrateVelocities(deltaTime);
    }
}

void World::CheckCollisions()
{
}