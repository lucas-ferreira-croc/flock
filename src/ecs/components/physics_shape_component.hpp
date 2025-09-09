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
    PhysicsShapeComponent(PhysicsShapeType shapeType = PhysicsShapeType::BOX)
    {
        type = shapeType;
    }
    
    PhysicsShapeType type;
};

#endif