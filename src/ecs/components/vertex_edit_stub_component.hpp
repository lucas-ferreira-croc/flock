#ifndef VERTEX_EDIT_STUB_COMPONENT_HPP
#define VERTEX_EDIT_STUB_COMPONENT_HPP

#include "glm/glm.hpp"

struct VertexEditStubComponent
{
    VertexEditStubComponent(glm::vec3 lastPosition = glm::vec3(1.0f), int vertexId = -1, bool edit = false)
    {
        mLastPosition = lastPosition;
        mVertexID = vertexId;
        mEdit = false;
    }

    void setAssociatedVertices(std::vector<int> vertices)
    {
        mAssociatedVertices = vertices;
    }

    glm::vec3 mLastPosition;
    int mVertexID;
    std::vector<int> mAssociatedVertices;
    bool mEdit;
    std::string mParent;
};

#endif