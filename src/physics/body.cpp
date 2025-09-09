#include "body.hpp"

Body::Body(std::shared_ptr<Shape> shape, float x, float y, float z, float mass)
    : shape(shape->Clone()), 
      position(x, y, z), velocity(0.0f, 0.0f, 0.0f), acceleration(0.0f, 0.0f, 0.0f),
      rotation(0.0f), angularVelocity(0.0f), angularAcceleration(0.0f),
      sumForces(0.0f), sumTorque(0.0f),
      mass(mass), invMass(mass != 0.0f ? 1.0f / mass : 0.0f),
      I(shape->GetMomentOfInertia() * mass),
      restituition(1.0f),
      friction(0.7f)
{
    this->shape->UpdateVertices(rotation, position);
}

Body::~Body()
{

}

bool Body::IsIstatic() const 
{
    const float epislon = 0.005f;
    return fabs(invMass  - 0.0f) <= epislon;
}

void Body::AddForce(glm::vec3 force)
{
    sumForces += force;
}

void Body::AddTorque(float torque)
{
    sumTorque += torque;
}

void Body::ClearForces()
{
    sumForces.x = 0.0f;
    sumForces.y = 0.0f;
    sumForces.z = 0.0f;
}

void Body::ClearTorque()
{
    sumTorque = 0.0f;
}

void Body::IntegrateForces(const float deltaTime)
{
    if(IsIstatic())
        return;

    acceleration = sumForces * invMass;
    velocity += acceleration * deltaTime;

    angularAcceleration = sumTorque * invI;
    angularVelocity += angularAcceleration * deltaTime;

    ClearForces();
    ClearTorque();
}


void Body::IntegrateVelocities(const float deltaTime)
{
    if (IsIstatic())
        return;
    
    position += velocity * deltaTime;
    rotation += angularVelocity * deltaTime;

    shape->UpdateVertices(rotation, position);
}

void Body::IntegrateLinear(float deltaTime) 
{
    if(IsIstatic())
        return;

    // F = ma -> second law of motion
    acceleration = sumForces * invMass;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    ClearForces();
}

void Body::IntegrateAngular(float deltaTime)
{
    if(IsIstatic())
        return;

    // F = ma -> second law of motion
    angularAcceleration = sumTorque * invI;
    angularVelocity += angularAcceleration * deltaTime;
    rotation += angularVelocity * deltaTime;

    ClearTorque();
}

void Body::ApplyImpulseLinear(glm::vec3& j)
{
    if(IsIstatic())
        return;

    velocity += j * invMass;
}
void Body::ApplyImpulseAngular(float j)
{
    if(IsIstatic())
        return;
    
    angularVelocity *=  j * invI;
}

void Body::ApplyImpulseAtPoint(glm::vec3& j, glm::vec3& r)
{
    if(IsIstatic())
        return;

    // TO DO:
	velocity += j * invMass;
	//angularVelocity += r.Cross(j) * invI;
}