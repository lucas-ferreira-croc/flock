#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "ui/imgui_config.hpp"
#include "render/shader.hpp"
#include "render/camera.hpp"
#include "texture/texture.hpp"


const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1200;

const int BUFFER_SIZE = 4096;
static char fragmentShaderBuffer[BUFFER_SIZE] = "";
static char vertexShaderBuffer[BUFFER_SIZE] = "";


void loadFileIntoBuffer(const std::string& filename, static char* buffer)
{
    std::ifstream file(filename);
    if(!file)
    {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    std::stringstream ss;
    ss << file.rdbuf();

    std::string content = ss.str();

    if(content.size() >=  BUFFER_SIZE)
    {
        std::cerr << "File too long.";
        content = content.substr(0, BUFFER_SIZE - 1);
    }

    std::copy(content.begin(), content.end(), buffer);
    buffer[content.size()] = '\0';
}

void loadBufferIntoFile(const std::string& filename, static char* buffer)
{
    std::fstream file(filename, std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    file << buffer;
    file.close();
}

void reloadShaderFromUI(Shader& shader)
{
    shader.reload();
}


int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW not initialized\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "flock", nullptr, nullptr);
    if(!window)
    {
        std::cerr << "failed creating GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "failed initializing glad\n";
        glfwDestroyWindow(window);
	    glfwTerminate();
        return -1;
    }

    
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);


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

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    /// end of Imgui Session

    float vertices[] = {    
        
        // x     y      z    r    g     b      u     v
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int VAO, VBO, IBO;
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
	
    Shader shader;
    shader.createFromFile(vs_filename, fs_filename);

    loadFileIntoBuffer(vs_filename, vertexShaderBuffer);
    loadFileIntoBuffer(fs_filename, fragmentShaderBuffer);

    Texture texture(GL_TEXTURE_2D, "C:\\dev\\shader\\flock\\assets\\images\\mew.jpg");
    texture.loadTextureA();

    ///
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
    Camera camera(cameraPos, target);
    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, .1f, 100.0f);
    ///


    ///
   /* glfwSetKeyCallback(window, &Camera::keyCallback);
    glfwSetCursorPosCallback(window, &Camera::cursorPositionCallback);*/
    ///

    ///
    double previousSeconds = glfwGetTime();

    ///
    float angle = 0.0f;

    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /// Input
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);        
        }

        if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            shader.reload();
            loadFileIntoBuffer(vs_filename, vertexShaderBuffer);
            loadFileIntoBuffer(fs_filename, fragmentShaderBuffer);
        }
        
        ///

        double currentSeconds = glfwGetTime();
        double deltaTime = currentSeconds - previousSeconds;
        previousSeconds = currentSeconds;
        camera.update(deltaTime);
        ///
        
        glm::mat4 wvp(1.0f);
        glm::mat4 modelTransform(1.0f);

        //angle += 50.5f * (float)deltaTime;

        modelTransform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        wvp = projection * camera.getLookAt() * modelTransform;
        /// opengl
        
        texture.use(GL_TEXTURE0);
        shader.bind();
        shader.setInt("textureSampler", 0);
        shader.setMat4("wvp", wvp);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        ///
        
        /// ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("OpenGL Shaders");
        ImGui::InputTextMultiline("Vertex Shader", vertexShaderBuffer, BUFFER_SIZE, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 20));
        ImGui::InputTextMultiline("Fragment Shader", fragmentShaderBuffer, BUFFER_SIZE, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 20));
        if (ImGui::Button("save")) 
        {
            loadBufferIntoFile(vs_filename, vertexShaderBuffer);
            loadBufferIntoFile(fs_filename, fragmentShaderBuffer);
            reloadShaderFromUI(shader);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ///

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;    
}