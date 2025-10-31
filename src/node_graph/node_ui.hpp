#ifndef NODE_UI_HPP
#define NODE_UI_HPP

#include <unordered_map>
#include <string>
#include<imgui.h>
#include <vector>
#include <memory>

#include "base_node.hpp"

extern std::vector<std::shared_ptr<BaseNode>> nodes;

struct NodeUIState 
{
    ImVec2 position;
    bool beingDragged = false;
};

struct ConnectionVisual
{
    int fromId;
    int toId;
};

struct PendingConnection 
{
    bool isDragging = false;
    int fromNodeId = -1;
    ImVec2 startPos;
};

extern PendingConnection pendingConnection;

class NodeUIManager
{
public:
    std::unordered_map<int, NodeUIState> nodeStates;
    std::vector<ConnectionVisual> connections;
    PendingConnection pendingConnection;
    int selectedNode = -1;

    void spawnNode(NodeType type);
    void renderNode(BaseNode& node);
    void renderConnections();

};

#endif