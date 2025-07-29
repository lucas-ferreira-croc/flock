#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "ui/imgui_config.hpp"
#include "render/display.hpp"
#include "render/shader.hpp"
#include "render/camera.hpp"
#include "texture/texture.hpp"
#include "game/game.hpp"

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


int main(int argc, char* argv[])
{
    Game game;
    game.initialize();
    game.run();
    game.Destroy();
    
    return 0;    
}