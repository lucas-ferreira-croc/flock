#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <vector>

#include "ui/imgui_config.hpp"
#include "render/display.hpp"
#include "render/camera.hpp"
#include "texture/texture.hpp"
#include "render/shader.hpp"

class Game 
{
public:
    Game();
    ~Game();

    void initialize();
    void run();
    void ProcessInput();
    void update();
    void Render();
    void Destroy();



private:
    const int WINDOW_WIDTH = 1600;
    const int WINDOW_HEIGHT = 1200;
    const double FIXED_TIMESTEP = 1.0 / 60.0;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Display> _display;

    double previousSeconds;
    glm::mat4 projection;

    std::vector<std::shared_ptr<Texture>> _textures;
    Shader _shader;

    //legacy code, to be removed
    float angle = 0.0f;
    double deltaTime;

    unsigned int VAO, VBO, IBO;
};

#endif