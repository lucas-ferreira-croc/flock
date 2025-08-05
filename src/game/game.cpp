#include "game.hpp"
#include "ecs/ecs.hpp"

#include <thread>
#include <chrono>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ui/imgui_config.hpp"
#include "log/logger.hpp"


Game::Game()
{

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

void Game::setup()
{
    //Entity square = registry.createEntity;
    // square.addComponent<TransformComponent>();
    // square.addComponent<BoxColliderComponent>();
    // square.addComponent<BoxGeometryComponent>();

    float vertices[] = {

        // x     y      z    r    g     b      u     v
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,


    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        1, 2, 4,
        2, 4, 5,
        4, 5, 6,
        5, 6, 7,
        3, 7, 0,
        0, 6, 7,
        2, 3, 7,
        2, 5, 7,
        0, 1, 6,
        1, 4, 6
    };

    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glm::vec3 modelPosition;

    const char* vs_filename = "C:\\dev\\shader\\flock\\assets\\shaders\\v.glsl";
    const char* fs_filename = "C:\\dev\\shader\\flock\\assets\\shaders\\f.glsl";

    _shader.createFromFile(vs_filename, fs_filename);
    previousSeconds = glfwGetTime();
    //_textures.push_back(std::make_shared<Texture>(GL_TEXTURE_2D, "C:\\dev\\shader\\flock\\assets\\images\\chariz.png"));
    //_textures.at(0)->loadTextureA();
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

}
void Game::render()
{
    _display->clearColor(0.5f, 0.5f, 0.5f, 1.0f); 

    _display->clear();

    if (angle <= 360)
        angle += 60.0f * (float)_deltaTime;
    else
        angle = 0.0f;

    glm::mat4 wvp(1.0f);
     glm::mat4 modelTransform(1.0f);
    modelTransform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    //movementSystem.update();
    //CollissionSystem.update();
    wvp = projection * _camera->getLookAt() * modelTransform;
    ///
    
    //_textures.at(0)->use(GL_TEXTURE0);
    _shader.bind();
    _shader.setMat4("wvp", wvp);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

  

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