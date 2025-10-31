#ifndef BASE_NODE_HPP
#define BASE_NODE_HPP

#include <string>
#include <map>
#include <vector>

#include "node_type.hpp"

#include "log/logger.hpp"

class BaseNode
{
public:
    int _id;
    std::string _name;
    std::map<std::string, float> _floatParams;
    std::vector<int> _inputIds;
    std::vector<int> _outputIds;
    NodeType _nodeType;

    BaseNode(int id, const std::string& name, NodeType nodeType) : _id(id), _name(name), _nodeType(nodeType)
    {
        Logger::warning("Base node created with id " + std::to_string(id) + " and name " + name);
    }
    virtual ~BaseNode() = default;
    virtual float getOuput() const { return 1.0f; };
    virtual void setInput(){}

    virtual void compute() = 0;
};

#endif