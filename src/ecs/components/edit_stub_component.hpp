#ifndef EDIT_STUB_COMPONENT_HPP
#define EDIT_STUB_COMPONENT_HPP

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
        associatedVertices = vertices;
    }

    glm::vec3 mLastPosition;
    int mVertexID;
    std::vector<int> associatedVertices;
    bool mEdit;
};

#endif