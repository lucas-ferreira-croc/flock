#ifndef NODE_FACTORY_HPP
#define NODE_FACTORY_HPP

#include "base_node.hpp"
#include "output_node.hpp"

std::shared_ptr<BaseNode> createNodeByType(NodeType type, int id)
{
    switch(type)
    {
        case NodeType::NODE_COLOR:
        {
            //return new NodeColor()
        }
        
        case NodeType::NODE_OUTPUT:
        {
            return std::make_shared<OutputNode>(id);
        }
    }
}

#endif