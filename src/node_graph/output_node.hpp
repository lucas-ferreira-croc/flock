#ifndef OUTPUT_NODE_HPP
#define OUTPUT_NODE_HPP

#include <glm/glm.hpp>

#include "base_node.hpp"

class OutputNode : public BaseNode
{
public:
    glm::vec3 _albedo;

    OutputNode(int id) : BaseNode(id, "OutputNode", NodeType::NODE_OUTPUT) {}; 
    void setInput(glm::vec3 albedo);
    void compute() override;
};

#endif