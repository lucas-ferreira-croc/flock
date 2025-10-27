#ifndef GUI_SYSTEM_HPP
#define GUI_SYSTEM_HPP

#include "ecs/components/id_component.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/ecs.hpp"
#include "ui/imgui_config.hpp"
#include "log/logger.hpp"

#include "glm/gtc/type_ptr.hpp"
class GUISystem : public System
{
public:
    GUISystem()
    {
        Logger::log("requererra");
        requireComponent<IDComponent>();
        Logger::log("requiriu");
    }

    void Update()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Entities");
        for(auto& entity : getSystemEntities())
        {
            auto& idComponent = entity.getComponent<IDComponent>();
            ImGui::PushID(entity.getId());
            ImGui::Text("Entity: %s", idComponent._name.c_str());
            if(entity.hasComponent<TransformComponent>())
            {
                auto& transformComponent = entity.getComponent<TransformComponent>();
                ImGui::SliderFloat3("Position", glm::value_ptr(transformComponent.position), -100.0f, 100.0f, "%.3f",  0.1f); ImGui::NewLine();
                ImGui::SliderFloat3("Rotation", glm::value_ptr(transformComponent.rotation), -360.0f, 360.0f, "%.3f", 0.1f); ImGui::NewLine();
                ImGui::SliderFloat3("Scale", glm::value_ptr(transformComponent.scale), 1.0f, 100.0f, "%.3f", 0.1f); ImGui::NewLine();
            }

            ImGui::Separator();
            ImGui::PopID();
        }
        
        ImGui::End();
        ImGui::render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

#endif