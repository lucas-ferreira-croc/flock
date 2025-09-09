#include "shape.hpp"

CircleShape::CircleShape(const float radius)
    : radius(radius)
{
   
}

CircleShape::~CircleShape()
{
}

ShapeType CircleShape::GetType() const
{
    return ShapeType::CIRCLE;
}

float CircleShape::GetMomentOfInertia() const 
{
    return (radius * radius) * 0.5f;
}

std::shared_ptr<Shape> CircleShape::Clone()
{
    return shared_from_this();
}

void CircleShape::UpdateVertices(float angle, const glm::vec3 position) 
{
    return;
}

///

PolygonShape::PolygonShape(const std::vector<glm::vec3>& vertices)
{
    for(auto vertex : vertices)
    {
        localVertices.push_back(vertex);
        worldVertices.push_back(vertex);
    }
}

PolygonShape::~PolygonShape()
{

}

void PolygonShape::UpdateVertices(float angle, const glm::vec3 position)
{
    for(int i = 0; i < localVertices.size(); i++)
    {
        // TO DO: figure out rotation
        worldVertices[i] += position;
    }
}

glm::vec3 PolygonShape::EdgeAt(int index) const
{
    return glm::vec3(0.0f);
}

ShapeType PolygonShape::GetType() const 
{
    return ShapeType::POLYGON;
}

float PolygonShape::GetMomentOfInertia() const 
{
    return 5000.0f;
}

std::shared_ptr<Shape> PolygonShape::Clone() 
{
    return shared_from_this();
}


BoxShape::BoxShape(float width, float height, float depth)
	: width(width), height(height), depth(depth)
{
    localVertices.push_back(glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f));
	localVertices.push_back(glm::vec3(width  / 2.0f, -height / 2.0f,  depth / 2.0f));
	localVertices.push_back(glm::vec3(width  / 2.0f,  height / 2.0f, -depth / 2.0f));
	localVertices.push_back(glm::vec3(-width / 2.0f,  height / 2.0f,  depth / 2.0f));

	worldVertices.push_back(glm::vec3(-width / 2.0f, -height / 2.0f, -depth / 2.0f));
	worldVertices.push_back(glm::vec3(width  / 2.0f, -height / 2.0f,  depth / 2.0f));
	worldVertices.push_back(glm::vec3(width  / 2.0f,  height / 2.0f, -depth / 2.0f));
	worldVertices.push_back(glm::vec3(-width / 2.0f,  height / 2.0f,  depth / 2.0f));
}

BoxShape::~BoxShape()
{
}

ShapeType BoxShape::GetType() const
{
	return ShapeType::BOX;
}

float BoxShape::GetMomentOfInertia() const
{
	return (0.083333f) * (width * width + height * height + depth * depth);
}

std::shared_ptr<Shape> BoxShape::Clone()
{
	return shared_from_this();
}
