#include "node_ui.hpp"
#include "log/logger.hpp"
#include "node_graph.hpp"
#include "output_node.hpp"
#include "node_factory.hpp"

#include <imgui_internal.h>

PendingConnection pendingConnection;
NodeGraph graph;
std::vector<std::shared_ptr<BaseNode>> nodes;

void NodeUIManager::renderNode(BaseNode& node)
{
    Logger::err("renderNode entrance");

    int id = node._id;
    std::string name = node._name;
    
    NodeUIState& state = nodeStates[id];
    Logger::err("renderNode entrance");
    ImVec2 scrollOffset(ImGui::GetScrollX(), ImGui::GetScrollY());
    
    if(state.position.x == 0 && state.position.y == 0)
        state.position = ImVec2(100 + id * 180, 250);
    
    ImVec2 nodeSize = ImVec2(200, 100);
    ImVec2 start = ImVec2(state.position.x - scrollOffset.x, state.position.y - scrollOffset.y);
    ImVec2 end = ImVec2(start.x - nodeSize.x, start.y - nodeSize.y);
    
    ImGui::SetCursorScreenPos(start);
    ImGui::PushID(id);
    ImGui::BeginGroup();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(start, end, IM_COL32(40, 40, 40, 255), 6.0f);
    drawList->AddRect(start, end, IM_COL32(255, 255, 255, 255), 6.0f);
    drawList->AddText(ImVec2(start.x + 10, start.y + 10), IM_COL32(255, 255, 255, 255), name.c_str());

    if(OutputNode* outputnode = dynamic_cast<OutputNode*>(&node))
    {
        Logger::warning("created output node in NodeUIManager");
    }


    ImGui::SetCursorScreenPos(start);
    ImGui::InvisibleButton("node_drag", nodeSize);


    if(ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) 
    {
        state.position.x += ImGui::GetIO().MouseDelta.x;
        state.position.y += ImGui::GetIO().MouseDelta.y;
    }

    if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) 
    {
        selectedNode = id;
    }

    if(id == selectedNode)
    {
        drawList->AddRect(start, end, IM_COL32(0, 255, 0, 255), 6.0f, 0, 3.0f);
    }

    ImGui::EndGroup();
    ImGui::PopID();
}

void NodeUIManager::renderConnections()
{
    ImVec2 scrollOffset(ImGui::GetScrollX(), ImGui::GetScrollY());
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    for(int i = 0; i < connections.size(); i++)
    {
        const auto& connection = connections[i];
        ImVec2 fromPos = nodeStates[connection.fromId].position;
        ImVec2 toPos = nodeStates[connection.toId].position;

        fromPos = ImVec2(fromPos.x - scrollOffset.x + 200, fromPos.y - scrollOffset.y + 70);
        toPos = ImVec2(toPos.x - scrollOffset.x, toPos.y - scrollOffset.y + 70);

        ImVec2 cp1 = ImVec2(fromPos.x + 50, fromPos.y);
        ImVec2 cp2 = ImVec2(toPos.x - 50, toPos.y);

        drawList->AddBezierCubic(fromPos, cp1, cp2, toPos, IM_COL32(200, 200, 100, 255), 3.0f);

        ImVec2 midPoint = ImVec2((fromPos.x + toPos.x) * 0.5f, (fromPos.y + toPos.y) * 0.5f);
        ImGui::SetCursorScreenPos(ImVec2(midPoint.x - 8, midPoint.y -9));
        ImGui::InvisibleButton(("connection_" + std::to_string(i)).c_str(), ImVec2(16, 16));
    
        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            connections.erase(connections.begin() + i);
            graph._connections.erase(
                std::remove_if(graph._connections.begin(), graph._connections.end(),
                [&](const auto& conn){
                    return conn.first == connection.fromId && conn.second == connection.toId;
                }),
                graph._connections.end()
            );
            graph.computeAll();
            break;
        }
    }

    for(const auto& [id, state] : nodeStates)
    {
        ImVec2 outputPin = ImVec2(state.position.x + 200 - scrollOffset.x, state.position.y + 70 - scrollOffset.y);
        ImGui::SetCursorScreenPos(ImVec2(outputPin.x - 5, outputPin.y - 5));
        ImGui::InvisibleButton(("ouput_" + std::to_string(id)).c_str(), ImVec2(10, 10));
        if(ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            pendingConnection.isDragging = true;
            pendingConnection.fromNodeId = id;
            pendingConnection.startPos = outputPin;
        }

        std::shared_ptr<BaseNode> toNode = graph._nodes[id];

        ImVec2 inputPin = ImVec2(state.position.x - scrollOffset.x, state.position.y + 70 - scrollOffset.y);
        ImGui::SetCursorScreenPos(ImVec2(inputPin.x- 5, inputPin.y - 5));
        ImGui::InvisibleButton(("input_" + std::to_string(id)).c_str(), ImVec2(10, 10));
        if(ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && pendingConnection.isDragging) 
        {
            int fromID = pendingConnection.fromNodeId;
            std::shared_ptr<BaseNode> from = graph._nodes[fromID];
            graph.connect(fromID, id);
            connections.push_back({fromID, id});
            graph.computeAll();
            pendingConnection = PendingConnection();
        }
        
        drawList->AddCircleFilled(inputPin, 5.0f, IM_COL32(0, 255, 255, 255));
        drawList->AddCircleFilled(outputPin, 5.0f, IM_COL32(255, 255, 0, 255));
    }    


    if(pendingConnection.isDragging)
    {
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        ImVec2 p1 = pendingConnection.startPos;
        ImVec2 cp1 = ImVec2(p1.x + 50, p1.y);
        ImVec2 cp2 = ImVec2(mousePos.x - 50, mousePos.y);
        drawList->AddBezierCubic(p1, cp1, cp2, mousePos, IM_COL32(255, 255, 0, 255), 2.0f);
    }

    if(pendingConnection.isDragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        pendingConnection = pendingConnection;
    }
}

void NodeUIManager::spawnNode(NodeType type)
{
    static int nextID = 1000;
    std::shared_ptr<BaseNode> newNode = createNodeByType(type, nextID++);
    if(!newNode.get()) 
        return;
    
    nodes.push_back(newNode);
    graph.addNode(newNode);
    graph._nodes[newNode->_id] = newNode;
    nodeStates[newNode->_id].position = ImVec2(300, 300);
}