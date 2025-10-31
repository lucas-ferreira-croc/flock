#include "output_node.hpp"

#include "log/logger.hpp"

void OutputNode::setInput(glm::vec3 albedo)
{
    _albedo = albedo;
    compute();
    Logger::warning("received an albedo via connection");
}

void OutputNode::compute()
{
    Logger::warning("computing albedo");
}

