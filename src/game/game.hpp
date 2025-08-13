#ifndef GAME_HPP
#define GAME_HPP

#include <memory>
#include <vector>

#include "ecs/ecs.hpp"
#include "ui/imgui_config.hpp"
#include "render/display.hpp"
#include "render/camera.hpp"
#include "texture/texture.hpp"
#include "render/shader.hpp"
#include "render/model.hpp"

class Game 
{
public:
    Game();
    ~Game();

    void initialize();
    void setup();
    void loadLevel(int level);
    
    void run();
    void processInput();
    void update();
    void render();
    void destroy();

    
private:
    std::unique_ptr<Registry> _registry;

    const int WINDOW_WIDTH = 1600;
    const int WINDOW_HEIGHT = 1200;
    const int FPS = 60;
    const double FIXED_TIMESTEP = 1.0 / FPS;

    std::shared_ptr<Camera> _camera;
    std::shared_ptr<Display> _display;

    double previousSeconds;
    glm::mat4 projection;

    std::vector<std::shared_ptr<Texture>> _textures;
    Shader _shader;

    //legacy code, to be removed
    float angle = 0.0f;
    double _deltaTime;

    unsigned int VAO, VBO, IBO;

    std::unique_ptr<Model> model;
};

#endif