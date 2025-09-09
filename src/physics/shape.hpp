#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <memory>
#include <vector>
#include "glm/glm.hpp"


enum class ShapeType{ CIRCLE, POLYGON, BOX};

struct Shape : public std::enable_shared_from_this<Shape>
{
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual float GetMomentOfInertia() const = 0;
    virtual std::shared_ptr<Shape> Clone() = 0;
    virtual void UpdateVertices(float angle, const glm::vec3 position) = 0;
};


struct CircleShape : public Shape 
{
    float radius;
    CircleShape(const float radius);
    virtual ~CircleShape();

    ShapeType GetType() const override;
    float GetMomentOfInertia() const override;
    std::shared_ptr<Shape> Clone() override;
    void UpdateVertices(float angle, const glm::vec3 position) override;
};

struct PolygonShape : public Shape 
{
    std::vector<glm::vec3> localVertices;
    std::vector<glm::vec3> worldVertices;

    PolygonShape() = default;
    PolygonShape(const std::vector<glm::vec3>& vertices);
    virtual ~PolygonShape();

    void UpdateVertices(float angle, const glm::vec3 position) override;
    glm::vec3 EdgeAt(int index) const;

    // Collision section;
    float FindMinSeparation(const PolygonShape& other, int& indexReferenceEdege, glm::vec3 supportPoint) const;
    int FindIncidentEdge(const glm::vec3& normal) const;
    int ClipSegmentToLine(const std::vector<glm::vec3>& contactsIn, const std::vector<glm::vec3>& contactsOut, const glm::vec3& c0, const glm::vec3& c1) const;

    ShapeType GetType() const override;
    float GetMomentOfInertia() const override;
    std::shared_ptr<Shape> Clone() override;
};


struct BoxShape : public PolygonShape 
{
    float width;
    float height;
    float depth;
    
    BoxShape(float width, float height, float depth);
    ~BoxShape();
    ShapeType GetType() const override;
    float GetMomentOfInertia() const override;
    std::shared_ptr<Shape> Clone() override;
};

#endif