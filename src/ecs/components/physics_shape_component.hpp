#ifndef PHYSICS_SHAPE_COMPONENT_HPP
#define PHYSICS_SHAPE_COMPONENT_HPP

enum class PhysicsShapeType
{
    BOX = 0,
    PLANE,
    SPHERE,
    CONE,
    CYLINDER
};

struct PhysicsShapeComponent
{
    PhysicsShapeComponent(PhysicsShapeType shapeType = PhysicsShapeType::BOX, float mass = 1.0f, float size = 1.0f)
    {
        type = shapeType;
        this->mass = mass;
        this->size = size;
    }
    
    PhysicsShapeType type;
    float mass;
    float size;
};

#endif