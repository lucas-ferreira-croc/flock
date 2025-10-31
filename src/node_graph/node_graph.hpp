#ifndef NODE_GRAPH_HPP
#define NODE_GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <memory>

#include "base_node.hpp"

class NodeGraph
{
public:
    std::unordered_map<int, std::shared_ptr<BaseNode>> _nodes;
    std::vector<std::pair<int, int>> _connections;

    void addNode(std::shared_ptr<BaseNode> node);
    void connect(int fromId, int toId);
    void computeAll();
};

#endif