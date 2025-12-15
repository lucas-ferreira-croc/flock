#include "game.hpp"

#include <thread>
#include <chrono>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "log/logger.hpp"

#include "render/light/directional_light.hpp"
#include "ecs/components/transform_component.hpp"
#include "ecs/components/rigid_body_component.hpp"
#include "ecs/components/mesh_component.hpp"
#include "ecs/components/debug_mesh_component.hpp"
#include "ecs/components/shader_component.hpp"
#include "ecs/components/material_component.hpp"
#include "ecs/components/cubemap_component.hpp"
#include "ecs/components/cloth_component.hpp"
#include "ecs/components/FABRIK_component.hpp"

#include "ecs/systems/render_system.hpp"
#include "ecs/systems/movement_system.hpp"
#include "ecs/systems/physics_system.hpp"
#include "ecs/systems/gui_system.hpp"
#include "ecs/systems/picking_system.hpp"
#include "ecs/systems/xpbd_system.hpp"
#include "ecs/systems/ik_system.hpp"

// #include "node_graph/node_graph.hpp"
// #include "node_graph/output_node.hpp"
// #include "node_graph/node_ui.hpp"

double Game::mouse_x = 0.0f, Game::mouse_y = 0.0f;
bool Game::mouseClick = false;
bool simulating = false;
//NodeUIManager* uiManager;
//NodeGraph* graph;
//std::shared_ptr<OutputNode> outputNode;
//std::vector<std::shared_ptr<BaseNode>> nodes_;


void Game::mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(!(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS))
        return;
    
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    mouseClick = true;
}

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
    glfwSetMouseButtonCallback(_display->getWindow(), &Game::mouse_click_callback);
    ///

    /// Imgui Session
    _display->initializeUI();
}

void Game::loadLevel(int level)
{
    _registry->addSystem<MovementSystem>();
    _registry->addSystem<RenderSystem>();
    _registry->addSystem<PhysicsSystem>(9.8f);
    _registry->addSystem<GUISystem>();
    _registry->addSystem<PickingSystem>();
    _registry->addSystem<XPBDSystem>();
    _registry->addSystem<IKSystem>();
    _registry->addSystem<MultiEndedIKSystem>();

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
    cubemap.addComponent<CubemapComponent>(cubemapFaces);
    cubemap.addComponent<IDComponent>("cubemap");
    cubemap.addComponent<ShaderComponent>(cubemapVS, cubemapFS);
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
    backpack.addComponent<IDComponent>("backpack");
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
    teapot.addComponent<IDComponent>("teapot");
    _registry->getSystem<PhysicsSystem>().AddBody(teapot);
    entities.push_back(teapot);
    

    Entity plane = _registry->createEntity();
    //plane.addComponent<TransformComponent>(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(15.0f, 15.0f, 15.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
    plane.addComponent<TransformComponent>(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(15.0f, 15.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    plane.addComponent<MeshComponent>("C:\\dev\\shader\\flock\\assets\\models\\plane.obj");
    plane.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    plane.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), 32.0f);
    //plane.getComponent<ShaderComponent>().setDirectionalLight(directionalLight);
    plane.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    plane.getComponent<ShaderComponent>().setPointLights(pointLights);
    plane.getComponent<ShaderComponent>().setSpotLights(spotLights);    
    plane.addComponent<PhysicsShapeComponent>(PhysicsShapeType::BOX, 0.0f);
    entities.push_back(plane);
    plane.addComponent<IDComponent>("plane");
    plane.addComponent<ClothComponent>(plane.getComponent<MeshComponent>().model);

    _registry->getSystem<PhysicsSystem>().AddBody(plane);
    


    Entity IKtarget = _registry->createEntity();
    IKtarget.addComponent<TransformComponent>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(.3f), glm::vec3(0.0f));
    IKtarget.addComponent<MeshComponent>(MeshType::SPHERE);
    IKtarget.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    IKtarget.addComponent<MaterialComponent>(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 16.0f);
    IKtarget.getComponent<ShaderComponent>().setPointLights(pointLights);
    IKtarget.getComponent<ShaderComponent>().setSpotLights(spotLights);
    IKtarget.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    IKtarget.addComponent<IDComponent>("IKtarget");
    entities.push_back(IKtarget);


   
    
    Entity spheres0 = _registry->createEntity();
    spheres0.addComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(.3f), glm::vec3(0.0f));
    spheres0.addComponent<MeshComponent>(MeshType::SPHERE);
    spheres0.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    spheres0.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 16.0f);
    spheres0.getComponent<ShaderComponent>().setPointLights(pointLights);
    spheres0.getComponent<ShaderComponent>().setSpotLights(spotLights);
    spheres0.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    spheres0.addComponent<IDComponent>("sphere0");

    Entity spheres1 = _registry->createEntity();
    spheres1.addComponent<TransformComponent>(glm::vec3(7.0f, 0.0f, 0.0f), glm::vec3(.3f), glm::vec3(0.0f));
    spheres1.addComponent<MeshComponent>(MeshType::SPHERE);
    spheres1.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    spheres1.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 16.0f);
    spheres1.getComponent<ShaderComponent>().setPointLights(pointLights);
    spheres1.getComponent<ShaderComponent>().setSpotLights(spotLights);
    spheres1.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    spheres1.addComponent<IDComponent>("sphere1");


    Entity spheres2 = _registry->createEntity();
    spheres2.addComponent<TransformComponent>(glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(.3f), glm::vec3(0.0f));
    spheres2.addComponent<MeshComponent>(MeshType::SPHERE);
    spheres2.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    spheres2.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 16.0f);
    spheres2.getComponent<ShaderComponent>().setPointLights(pointLights);
    spheres2.getComponent<ShaderComponent>().setSpotLights(spotLights);
    spheres2.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    spheres2.addComponent<IDComponent>("sphere2");

    Entity spheres3 = _registry->createEntity();
    spheres3.addComponent<TransformComponent>(glm::vec3(4.0f, 7.0f, 0.0f), glm::vec3(.3f), glm::vec3(0.0f));
    spheres3.addComponent<MeshComponent>(MeshType::SPHERE);
    spheres3.addComponent<ShaderComponent>(vsFilename, fsColorfilename);
    spheres3.addComponent<MaterialComponent>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 16.0f);
    spheres3.getComponent<ShaderComponent>().setPointLights(pointLights);
    spheres3.getComponent<ShaderComponent>().setSpotLights(spotLights);
    spheres3.getComponent<ShaderComponent>().addUniformVec3("cameraPos", _camera->getPosition());
    spheres3.addComponent<IDComponent>("sphere3");

    spheresVectors.push_back(spheres0.getComponent<TransformComponent>().position);
    spheresVectors.push_back(spheres1.getComponent<TransformComponent>().position);
    spheresVectors.push_back(spheres2.getComponent<TransformComponent>().position);
    spheresVectors.push_back(spheres3.getComponent<TransformComponent>().position);
    spheresVectors.push_back(IKtarget.getComponent<TransformComponent>().position);

    spheres0.addComponent<MultiEndedFABRIKComponent>();


    auto a = spheres0.getComponent<MultiEndedFABRIKComponent>().buildChain(spheres0.getComponent<TransformComponent>().position, spheres1.getComponent<TransformComponent>().position, 10);
    auto a1 = spheres0.getComponent<MultiEndedFABRIKComponent>().buildChain(spheres1.getComponent<TransformComponent>().position, spheres2.getComponent<TransformComponent>().position, 10);
    auto a2 = spheres0.getComponent<MultiEndedFABRIKComponent>().buildChain(spheres1.getComponent<TransformComponent>().position, spheres3.getComponent<TransformComponent>().position, 10);
    auto a3 = spheres0.getComponent<MultiEndedFABRIKComponent>().buildChain(spheres1.getComponent<TransformComponent>().position, IKtarget.getComponent<TransformComponent>().position, 10);
    spheres0.getComponent<MultiEndedFABRIKComponent>().addChain(a, spheres1.getComponent<TransformComponent>().position);
    spheres0.getComponent<MultiEndedFABRIKComponent>().addChain(a1, spheres2.getComponent<TransformComponent>().position);
    spheres0.getComponent<MultiEndedFABRIKComponent>().addChain(a2, spheres3.getComponent<TransformComponent>().position);
    spheres0.getComponent<MultiEndedFABRIKComponent>().addChain(a3, IKtarget.getComponent<TransformComponent>().position);
    for(auto& chain : spheres0.getComponent<MultiEndedFABRIKComponent>().chains)
    {
        chain.shader->bind();
        chain.shader->setMat4("view", _camera->getLookAt());
        chain.shader->setMat4("proj", projection);
    }


    entities.push_back(spheres0);
    entities.push_back(spheres1);
    entities.push_back(spheres2);
    entities.push_back(spheres3);
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
    if (glfwGetKey(_display->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        simulating = !simulating;
    }

    if (glfwGetKey(_display->getWindow(), GLFW_KEY_R) == GLFW_PRESS)
    {
        for(auto entity : entities)
        {
            auto shaderComponent = _registry->getComponent<ShaderComponent>(entity);
            shaderComponent.reset();
        }
    }

    if(mouseClick)
    {
        float x = (2.0f * mouse_x) / WINDOW_WIDTH - 1.0f;
        float y = 1.0f - (2.0f * mouse_y) / WINDOW_HEIGHT;
        glm::vec4 ndc4d = glm::vec4(x, y, -1.0f, 1.0f);


        glm::vec4 rayView4d = glm::inverse(projection) * ndc4d;
        rayView4d.z = -1.0f; // direção para frente no space view
        rayView4d.w = 0.0f;

        glm::mat4 invView = glm::inverse(_camera->getLookAt()); // assumindo getLookAt() retorna view matrix
        glm::vec3 rayDirectionWorld = glm::normalize(glm::vec3(invView * rayView4d));
        glm::vec3 rayOrigin = _camera->getPosition();

        _registry->getSystem<PickingSystem>().Update(rayOrigin, rayDirectionWorld);
        mouseClick = false;
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

    if(simulating)
    {
        glm::vec3 target;
        for(auto& entity : _registry->getSystem<GUISystem>().getSystemEntities())
        {
            if(entity.getComponent<IDComponent>()._name == "IKtarget")
                target = entity.getComponent<TransformComponent>().position;
        }
        _registry->getSystem<IKSystem>().Update(target);

            std::vector<glm::vec3> entityPositions(5, glm::vec3(0.0f));
            for (auto& e : _registry->getSystem<GUISystem>().getSystemEntities())
            {
                const auto& id = e.getComponent<IDComponent>()._name;
                if (id == "sphere0") entityPositions[0] = e.getComponent<TransformComponent>().position;
                else if (id == "sphere1") entityPositions[1] = e.getComponent<TransformComponent>().position;
                else if (id == "sphere2") entityPositions[2] = e.getComponent<TransformComponent>().position;
                else if (id == "sphere3") entityPositions[3] = e.getComponent<TransformComponent>().position;
                else if (id == "IKtarget") entityPositions[4] = e.getComponent<TransformComponent>().position;
            }
            
            _registry->getSystem<MultiEndedIKSystem>().Update(entityPositions);
    }

    _registry->update();
}
void Game::render()
{
    _display->clear();
    _registry->getSystem<RenderSystem>().Update(projection, _camera, *_display);
    _registry->getSystem<GUISystem>().Update(_camera->getLookAt(), projection, _display);
}

void Game::destroy(){
    _display->destroyUI();
}