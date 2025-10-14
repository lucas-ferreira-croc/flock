#include "game.hpp"

#include <thread>
#include <chrono>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ui/imgui_config.hpp"
#include "log/logger.hpp"

#include "render/light/directional_light.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"
#include "ecs/components/mesh_component.hpp"
#include "ecs/components/debug_mesh_component.hpp"
#include "ecs/components/shader_component.hpp"
#include "ecs/components/material_component.hpp"
#include "ecs/components/cubemap_component.hpp"

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
    std::string fsReflectionName = "C:\\dev\\shader\\flock\\assets\\shaders\\f_reflection.glsl";
    std::string fsRefractionName = "C:\\dev\\shader\\flock\\assets\\shaders\\f_refraction.glsl";
    DirectionalLight directionalLight(glm::vec3(1.0f), 0.7f, 0.7f);
    //directionalLight.direction = glm::normalize(glm::vec3(-2.0f, -4.0f, -2.0f)); 
    directionalLight.direction = glm::normalize(glm::vec3(-1.0f, -3.0f, -10.0f)); 

    std::vector<PointLight> pointLights;
	PointLight pointLight0;
	pointLight0.diffuseIntensity = 0.5f;
	pointLight0.ambientIntensity = 0.5f;
	pointLight0.color = glm::vec3(0.0f, 1.0f, 1.0f);
    pointLight0.position =glm::vec3(0.0f, 4.0f, 1.0f);
	pointLight0.attenuation.linear = 0.09f;
	pointLight0.attenuation.exp = 0.032f;
	pointLights.push_back(pointLight0);

    std::vector<SpotLight> spotLights;

    SpotLight spotLight0;
    spotLight0.diffuseIntensity = 0.3f;
    spotLight0.ambientIntensity = 0.5f;
    spotLight0.color = glm::vec3(1.0f, 1.0f, 1.0);
    spotLight0.attenuation.linear = 0.1f;
    spotLight0.cutoff = glm::cos(glm::radians(50.5f));
    spotLight0.outerCutofff = glm::cos(glm::radians(60.0f));
    spotLight0.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    spotLights.push_back(spotLight0);


    std::string cubemapVS = "C:\\dev\\shader\\flock\\assets\\shaders\\v_skybox.glsl";
    std::string cubemapFS = "C:\\dev\\shader\\flock\\assets\\shaders\\f_skybox.glsl";
    std::vector<std::string> cubemapFaces = {
        "C:\\dev\\shader\\flock\\assets\\skybox\\right.jpg",
        "C:\\dev\\shader\\flock\\assets\\skybox\\left.jpg",
        "C:\\dev\\shader\\flock\\assets\\skybox\\top.jpg",
        "C:\\dev\\shader\\flock\\assets\\skybox\\bottom.jpg",
        "C:\\dev\\shader\\flock\\assets\\skybox\\front.jpg",
        "C:\\dev\\shader\\flock\\assets\\skybox\\back.jpg"
    };

    Entity cubemap = _registry->createEntity();
    cubemap.addComponent<TransformComponent>(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
    cubemap.addComponent<ShaderComponent>(cubemapVS, cubemapFS);
    cubemap.addComponent<CubemapComponent>(cubemapFaces);
    cubemap.getComponent<ShaderComponent>().addUniformInt("skybox", 0);
    entities.push_back(cubemap);

    Entity backpack = _registry->createEntity();
    backpack.addComponent<TransformComponent>(glm::vec3(3.0f, -3.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    backpack.addComponent<MeshComponent>("C:\\dev\\shader\\flock\\assets\\models\\backpack\\backpack.obj");
    backpack.addComponent<MaterialComponent>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 8.0f);
    backpack.addComponent<ShaderComponent>(vsFilename, fsFilename);
    backpack.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    //backpack.getComponent<ShaderComponent>().setDirectionalLight(directionalLight);
    backpack.getComponent<ShaderComponent>().setPointLights(pointLights);
    backpack.getComponent<ShaderComponent>().setSpotLights(spotLights);
    backpack.addComponent<PhysicsShapeComponent>(PhysicsShapeType::BOX, 0.0f);
    
    _registry->getSystem<PhysicsSystem>().AddBody(backpack);
    entities.push_back(backpack);
    

    
    Entity teapot = _registry->createEntity();
    teapot.addComponent<TransformComponent>(glm::vec3(-3.0f, -4.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    teapot.addComponent<MeshComponent>("C:\\dev\\shader\\flock\\assets\\models\\smooth_teapot.dae");
    teapot.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    teapot.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 16.0f);
    //teapot.getComponent<ShaderComponent>().setDirectionalLight(directionalLight);
    teapot.getComponent<ShaderComponent>().setPointLights(pointLights);
    teapot.getComponent<ShaderComponent>().setSpotLights(spotLights);
    teapot.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    teapot.addComponent<PhysicsShapeComponent>(PhysicsShapeType::SPHERE, 0.0f);
    _registry->getSystem<PhysicsSystem>().AddBody(teapot);
    entities.push_back(teapot);
    

    
    Entity plane = _registry->createEntity();
    plane.addComponent<TransformComponent>(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(15.0f, 15.0f, 15.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
    plane.addComponent<MeshComponent>(MeshType::PLANE);
    plane.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    plane.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), 32.0f);
    //plane.getComponent<ShaderComponent>().setDirectionalLight(directionalLight);
    plane.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    plane.getComponent<ShaderComponent>().setPointLights(pointLights);
    plane.getComponent<ShaderComponent>().setSpotLights(spotLights);
    
    plane.addComponent<PhysicsShapeComponent>(PhysicsShapeType::BOX, 0.0f);
    entities.push_back(plane);

    _registry->getSystem<PhysicsSystem>().AddBody(plane);
    
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
        for(auto entity : entities)
        {
            auto shaderComponent = _registry->getComponent<ShaderComponent>(entity);
            shaderComponent.reset();
        }
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
    _registry->getSystem<RenderSystem>().Update(projection, _camera, *_display);

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