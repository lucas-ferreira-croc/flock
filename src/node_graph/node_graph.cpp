#include "node_graph.hpp"
#include "output_node.hpp"

void NodeGraph::addNode(std::shared_ptr<BaseNode> node)
{
    _nodes[node->_id] = node;
}

void NodeGraph::connect(int fromId, int toId)
{
    _connections.push_back({fromId, toId});

    std::shared_ptr<BaseNode> from = _nodes[fromId];
    std::shared_ptr<BaseNode> to = _nodes[toId];

    if(!from.get() || !to.get()) 
        return;


    switch(from->_nodeType)
    {
        case NodeType::NODE_OUTPUT: 
        {
            auto output = dynamic_cast<OutputNode*>(to.get());
            output->setInput(glm::vec3(from->getOuput(), from->getOuput(), from->getOuput()));
        }
    }
}

void NodeGraph::computeAll()
{
    for(auto& [fromId, toId] : _connections)
    {
        std::shared_ptr<BaseNode> fromNode = _nodes[fromId];
        std::shared_ptr<BaseNode> toNode = _nodes[toId];

        if(!fromNode.get() || !toNode.get()) continue;

        glm::vec3 color;

        
    }
}