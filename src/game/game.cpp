#include "game.hpp"

#include <thread>
#include <chrono>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ui/imgui_config.hpp"
#include "log/logger.hpp"


#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"
#include "ecs/components/mesh_component.hpp"
#include "ecs/components/shader_component.hpp"
#include "ecs/components/material_component.hpp"

#include "ecs/systems/render_system.hpp"
#include "ecs/systems/movement_system.hpp"
#include "ecs/systems/physics_system.hpp"

Game::Game()
{
    _registry = std::make_unique<Registry>();
}

Game::~Game()
{
}

void Game::initialize()
{
    _display = std::make_shared<Display>(WINDOW_WIDTH, WINDOW_HEIGHT);
    _display->initializeWindow();

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
    _camera = std::make_shared<Camera>(cameraPos, target);
    projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, .1f, 100.0f);
    
    /// callback session
    glfwSetWindowUserPointer(_display->getWindow(), _camera.get());
    glfwSetKeyCallback(_display->getWindow(), &Camera::keyCallback);
    glfwSetCursorPosCallback(_display->getWindow(), &Camera::cursorPositionCallback);
    ///

    /// Imgui Session
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplGlfw_InitForOpenGL(_display->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    /// end of Imgui Session


    // opengl code
   
}

void Game::loadLevel(int level)
{
    _registry->addSystem<MovementSystem>();
    _registry->addSystem<RenderSystem>();
    _registry->addSystem<PhysicsSystem>(9.8f);

    std::string vsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\v.glsl";
    std::string fsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\f.glsl";
    std::string fsColorfilename = "C:\\dev\\shader\\flock\\assets\\shaders\\f_color.glsl";
    glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    Entity light = _registry->createEntity();
    light.addComponent<TransformComponent>(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    //light.addComponent<MeshComponent>(MeshType::BOX);
    light.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    //light.getComponent<ShaderComponent>().addUniformVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    //light.getComponent<ShaderComponent>().addUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    //light.getComponent<ShaderComponent>().addUniformVec3("lightPos", light.getComponent<TransformComponent>().position);


    // Entity sphere = _registry->createEntity();
    // sphere.addComponent<TransformComponent>(glm::vec3(-2.0f, 0.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    // //sphere.addComponent<RigidBodyComponent>(glm::vec3(1.0f, 0.0f, -5.0f));
    // sphere.addComponent<MeshComponent>(MeshType::SPHERE);
    // sphere.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    // sphere.getComponent<ShaderComponent>().addUniformVec4("color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    // sphere.getComponent<ShaderComponent>().addUniformVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    // sphere.getComponent<ShaderComponent>().addUniformVec3("lightPos", light.getComponent<TransformComponent>().position);

    Entity backpack = _registry->createEntity();
    backpack.addComponent<TransformComponent>(glm::vec3(3.0f, 0.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    //backpack.addComponent<RigidBodyComponent>(glm::vec3(0.0f, 0.0f, -10.0f));
    backpack.addComponent<MeshComponent>("C:\\dev\\shader\\flock\\assets\\models\\backpack\\backpack.obj");
    backpack.addComponent<MaterialComponent>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f);
    backpack.addComponent<ShaderComponent>(vsFilename, fsFilename);
    backpack.getComponent<ShaderComponent>().addUniformVec3("light.position", light.getComponent<TransformComponent>().position);
    backpack.getComponent<ShaderComponent>().addUniformVec3("light.ambient", lightAmbient);
    backpack.getComponent<ShaderComponent>().addUniformVec3("light.diffuse", lightDiffuse);
    backpack.getComponent<ShaderComponent>().addUniformVec3("light.specular", lightSpecular);

    Entity teapot = _registry->createEntity();
    teapot.addComponent<TransformComponent>(glm::vec3(-3.0f, 0.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    teapot.addComponent<MeshComponent>("C:\\dev\\shader\\flock\\assets\\models\\teapot.dae");
    teapot.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    teapot.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 256.0f);
    teapot.getComponent<ShaderComponent>().addUniformVec3("light.position", light.getComponent<TransformComponent>().position);
    teapot.getComponent<ShaderComponent>().addUniformVec3("light.ambient", lightAmbient);
    teapot.getComponent<ShaderComponent>().addUniformVec3("light.diffuse", lightDiffuse);
    teapot.getComponent<ShaderComponent>().addUniformVec3("light.specular", lightSpecular);
    teapot.addComponent<PhysicsShapeComponent>(PhysicsShapeType::SPHERE);
    _registry->getSystem<PhysicsSystem>().AddBody(teapot);

    Entity plane = _registry->createEntity();
    plane.addComponent<TransformComponent>(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    plane.addComponent<MeshComponent>(MeshType::PLANE);
    plane.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    plane.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), 32.0f);
    plane.getComponent<ShaderComponent>().addUniformVec3("light.position", light.getComponent<TransformComponent>().position);
    plane.getComponent<ShaderComponent>().addUniformVec3("light.ambient", lightAmbient);
    plane.getComponent<ShaderComponent>().addUniformVec3("light.diffuse", lightDiffuse);
    plane.getComponent<ShaderComponent>().addUniformVec3("light.specular", lightSpecular);

     // square.addComponent<TransformComponent>();
    // square.addComponent<BoxColliderComponent>();
    // square.addComponent<BoxGeometryComponent>()
    _shader.createFromFile(vsFilename, fsFilename);
}

void Game::setup()
{
    loadLevel(1);
    //model = std::make_unique<Model>("C:\\dev\\shader\\flock\\assets\\models\\backpack\\backpack.obj");
    previousSeconds = glfwGetTime();
}

void Game::run()
{
    setup();
    while (!glfwWindowShouldClose(_display->getWindow())) {
        processInput();
        update();
        render();
    }
}

void Game::processInput()
{
    if (glfwGetKey(_display->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(_display->getWindow(), true);
    }

    if (glfwGetKey(_display->getWindow(), GLFW_KEY_M) == GLFW_PRESS)
    {
        _camera->flipDislodgeMouse();
    }

    if (glfwGetKey(_display->getWindow(), GLFW_KEY_R) == GLFW_PRESS)
    {
    }
}
void Game::update()
{

    double currentSeconds = glfwGetTime();
    _deltaTime = currentSeconds - previousSeconds;
    previousSeconds = currentSeconds;

    if (_deltaTime < FIXED_TIMESTEP) {
        double timeToSleep = FIXED_TIMESTEP - _deltaTime;

        std::this_thread::sleep_for(std::chrono::duration<double>(timeToSleep));

        currentSeconds = glfwGetTime();
    }
    _camera->update(_deltaTime);
    
    _registry->getSystem<MovementSystem>().Update(_deltaTime);
    _registry->getSystem<PhysicsSystem>().Update(_deltaTime);
    _registry->update();
}
void Game::render()
{
    _display->clearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    _display->clear();

    if (angle <= 360)
        angle += 60.0f * (float)_deltaTime;
    else
        angle = 0.0f;

    glm::mat4 wvp(1.0f);
    glm::mat4 modelTransform(1.0f);

    modelTransform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    _registry->getSystem<RenderSystem>().Update(projection, _camera);

  //// ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");
    ImGui::End();

    ImGui::render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    _display->swapBuffers();

    ///
}

void Game::destroy(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}