#include "core/engine.hpp"
#include "core/assets.hpp"
#include "platform/input.hpp"

#include <SDL.h>
#include <iostream>

namespace wmw::core {

bool Engine::init(const std::string& assetsRoot) {
    assetsRoot_ = assetsRoot;

    std::cout << "[Engine] init (Windows x64 native core)\n";

    if (!physics_.init()) return false;
    if (!water_.init()) return false;
    if (!audio_.init()) return false;

    std::cout << "[Engine] status:\n";
    std::cout << "  Platform ............. REIMPLEMENTED (SDL2 / Win32)\n";
    std::cout << "  Level XML loader ...... REIMPLEMENTED\n";
    std::cout << "  PhysicsSystem ......... STUB\n";
    std::cout << "  WaterSystem ........... STUB\n";
    std::cout << "  Audio (FMOD) .......... STUB\n";
    std::cout << "  Debug renderer ........ REIMPLEMENTED (not original GLES)\n";

    initialized_ = true;
    return true;
}

bool Engine::loadLevel(const std::string& levelName) {
    if (!initialized_) return false;
    return levelLoader_.load(assetsRoot_, levelName, level_);
}

void Engine::update(float dt, const wmw::platform::Input& input) {
    if (!initialized_) return;

    // Debug camera zoom with keys (optional)
    (void)input;

    physics_.update(dt);
    water_.update(dt);
    audio_.update();
}

void Engine::render(SDL_Renderer* sdl) {
    if (!initialized_ || !sdl) return;

    int w = 0, h = 0;
    SDL_GetRendererOutputSize(sdl, &w, &h);
    screenW_ = w;
    screenH_ = h;

    renderer_.begin(sdl, screenW_, screenH_);
    renderer_.clear({20, 36, 56, 255});

    // Frame camera around room or object centroid
    float cx = level_.hasRoom ? level_.roomX : 0.f;
    float cy = level_.hasRoom ? level_.roomY : 0.f;
    if (!level_.hasRoom && !level_.objects.empty()) {
        cx = cy = 0.f;
        for (const auto& o : level_.objects) {
            cx += o.x;
            cy += o.y;
        }
        cx /= static_cast<float>(level_.objects.size());
        cy /= static_cast<float>(level_.objects.size());
    }
    renderer_.setCamera(cx, cy, 10.f);

    // Draw room marker
    if (level_.hasRoom) {
        renderer_.drawPoint(level_.roomX, level_.roomY, {255, 220, 80, 255}, 10);
    }

    // Draw every loaded object as a colored point by Type
    for (const auto& o : level_.objects) {
        const std::string type = o.get("Type", "");
        wmw::platform::Color c{180, 180, 180, 255};
        if (type == "spout") c = {80, 180, 255, 255};
        else if (type == "star") c = {255, 210, 60, 255};
        else if (type == "switch" || type == "yswitch") c = {255, 100, 100, 255};
        else if (o.get("Filename").find("pipe") != std::string::npos) c = {140, 140, 160, 255};

        renderer_.drawPoint(o.x, o.y, c, 6);
    }

    renderer_.end();
}

void Engine::shutdown() {
    if (!initialized_) return;
    audio_.shutdown();
    water_.shutdown();
    physics_.shutdown();
    std::cout << "[Engine] shutdown\n";
    initialized_ = false;
}

} // namespace wmw::core
