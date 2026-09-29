#pragma once

#include "core/level_loader.hpp"
#include "core/physics_system.hpp"
#include "core/water_system.hpp"
#include "platform/audio.hpp"
#include "platform/renderer.hpp"

#include <string>

struct SDL_Renderer;

namespace wmw::platform {
class Input;
}

namespace wmw::core {

class Engine {
public:
    bool init(const std::string& assetsRoot);
    bool loadLevel(const std::string& levelName);
    void update(float dt, const wmw::platform::Input& input);
    void render(SDL_Renderer* sdl);
    void shutdown();

private:
    std::string assetsRoot_;
    bool initialized_ = false;

    LevelData level_;
    LevelLoader levelLoader_;
    PhysicsSystem physics_;
    WaterSystem water_;
    wmw::platform::Audio audio_;
    wmw::platform::Renderer renderer_;

    int screenW_ = 1280;
    int screenH_ = 720;
};

} // namespace wmw::core
