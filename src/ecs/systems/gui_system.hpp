#ifndef GUI_SYSTEM_HPP
#define GUI_SYSTEM_HPP

#include <filesystem>

#include "ecs/components/id_component.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/ecs.hpp"
#include "ui/imgui_config.hpp"
#include "ImGuizmo/ImGuizmo.h"
#include "log/logger.hpp"
#include "render/display.hpp"
#include "integration/SO/file_dialog.hpp"
#include "glm/gtc/type_ptr.hpp"

static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
static bool useSnap(false);
static float snap[3] = {1.0f, 1.0f, 1.0f};


class GUISystem : public System
{
public:
    GUISystem()
    {
        requireComponent<IDComponent>();
    }

    void transformStart(glm::mat4& view, glm::mat4& projection, glm::mat4& objectMatrix)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_T))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(objectMatrix), matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(objectMatrix));

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_S))
        useSnap = !useSnap;
        ImGui::Checkbox(" ", &useSnap);
        ImGui::SameLine();
        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }

        ImGuiIO& io = ImGui::GetIO();
        float viewManipulateRight = io.DisplaySize.x;
        float viewManipulateTop = 0;

        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        viewManipulateRight = ImGui::GetWindowPos().x + 1600;
        viewManipulateTop = ImGui::GetWindowPos().y;
        //ImGuiWindow* window = ImGui::GetCurrentWindow();
        //gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
        ImGuizmo::ViewManipulate(glm::value_ptr(view), 100.0f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);


    }

    void editTransform(glm::mat4& view, glm::mat4& projection, glm::mat4& objectMatrix)
    {
         ImGuiIO& io = ImGui::GetIO();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(objectMatrix), NULL, useSnap ? &snap[0] : NULL);
    }

    void drawHierarchy()
    {
        ImGui::Begin("Hierarchy");
        for (auto& e : getSystemEntities())
        {
            auto& id = e.getComponent<IDComponent>();
            if(ImGui::Selectable(id._name.c_str(), id.isPicked))
            {
                for (auto& o : getSystemEntities())
                    o.getComponent<IDComponent>().isPicked = false;

                id.isPicked = true;
            }
        }
        ImGui::End();
    }

    void drawGizmo(glm::mat4& view, glm::mat4& projection)
    {
        for(auto& entity : getSystemEntities())
        {
            if(entity.hasComponent<IDComponent>())
            {
                auto idComponent = entity.getComponent<IDComponent>();
                if(idComponent.isPicked)
                {
                    glm::mat4 modelTransform(1.0f);
                    auto& transform = entity.getComponent<TransformComponent>();
                    modelTransform = glm::translate(modelTransform, transform.position);
                    modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                    modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                    modelTransform = glm::rotate(modelTransform, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
                    modelTransform = glm::scale(modelTransform, transform.scale);
                    
                    transformStart(view, projection, modelTransform);
                    ImGuizmo::PushID(1);
                    editTransform(view, projection, modelTransform);
                    if(ImGuizmo::IsUsing)
                    {
                        float T[3], R[3], S[3];
                        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelTransform), T, R, S);
                        transform.position = glm::vec3(T[0], T[1], T[2]);
                        transform.rotation = glm::vec3(R[0], R[1], R[2]);
                        transform.scale    = glm::vec3(S[0], S[1], S[2]);
                    }
                    ImGuizmo::PopID();
                }
            }
        }
    }

    void drawInspector(glm::mat4& view, glm::mat4& projection)
    {
        ImGui::Begin("Inspector");

        for (auto& e : getSystemEntities())
        {
            auto& id = e.getComponent<IDComponent>();
            if(!id.isPicked) continue;
            
            ImGui::Text("Entity: %s", id._name.c_str());
            if(e.hasComponent<TransformComponent>())
            {
                ImGui::Text("Transform Component:");
                drawGizmo(view, projection);
            }

            if(e.hasComponent<MaterialComponent>())
            {
                
                if(ImGui::CollapsingHeader("Material Component"))
                {
                    auto& m = e.getComponent<MaterialComponent>();
                    ImGui::DragFloat3("Ambient", glm::value_ptr(m.ambient), 0.1f);
                    ImGui::DragFloat3("Diffuse", glm::value_ptr(m.ambient), 0.1f);
                    ImGui::DragFloat3("Specular", glm::value_ptr(m.ambient), 0.1f);
                    ImGui::DragFloat("Shininess", &m.shininess);
                }
            }

            if(e.hasComponent<MeshComponent>())
            {
                if(ImGui::CollapsingHeader("Mesh Component"))
                {
                    auto& m = e.getComponent<MeshComponent>();
                    ImGui::Text("Loaded Mesh: %s",
                        std::filesystem::path(m.model->getDirectory()).filename().string().c_str());
                    if(ImGui::Button("Load Mesh"))
                    {
                        std::string path = openFileDialog("*.fbx;*.obj;*.gltf");
                        if(!path.empty())
                        {
                            m.model = std::make_shared<Model>(path.c_str());
                        }
                    }
                }
            }

            break;
        }

        

        ImGui::End();
    }

    void drawConsole()
    {
        ImGui::Begin("Console");
        for(const LogEntry& logEntry : Logger::messages)
        {
            auto message = logEntry.message.c_str();
            ImVec4 color;
            switch(logEntry.type)
            {
                case LogType::LOG_INFO:
                    color = ImVec4(0.0f, 1.0f, 0.0f, 1.0);
                    break;
                case LogType::LOG_WARNING:
                    color = ImVec4(1.0f, 1.0f, 0.0f, 1.0);
                    break;
                case LogType::LOG_ERROR:
                    color = ImVec4(1.0f, 0.0f, 0.0f, 1.0);
                    break;
                default:
                    color = ImVec4(0.0f, 1.0f, 0.0f, 1.0);
            }
            ImGui::TextColored(color, message);
        }
        ImGui::End();
    }

    void Update(glm::mat4& view, glm::mat4& projection, std::shared_ptr<Display> display)
    {
        display->renderUI();
        
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::BeginFrame();
        drawHierarchy();
        drawInspector(view, projection);
        drawConsole();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_context);
        }
        display->swapBuffers();             
    }
};

#endif