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
    std::cout << "  HsLoader / Spout ...... IMPLEMENTED (from .hs props)\n";
    std::cout << "  ParticleDescription ... MINIMAL (pos/vel/fluid) [INFERRED fields]\n";
    std::cout << "  Fluids solver ......... STUB\n";
    std::cout << "  PhysicsSystem ......... STUB\n";
    std::cout << "  Audio (FMOD) .......... STUB\n";

    initialized_ = true;
    return true;
}

bool Engine::loadLevel(const std::string& levelName) {
    if (!initialized_) return false;
    if (!levelLoader_.load(assetsRoot_, levelName, level_)) {
        return false;
    }
    return water_.bindLevel(assetsRoot_, level_);
}

void Engine::update(float dt, const wmw::platform::Input& input) {
    if (!initialized_) return;
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

    if (level_.hasRoom) {
        renderer_.drawPoint(level_.roomX, level_.roomY, {255, 220, 80, 255}, 10);
    }

    // Objects (legacy dots)
    for (const auto& o : level_.objects) {
        const std::string type = o.get("Type", "");
        wmw::platform::Color c{180, 180, 180, 255};
        if (type == "spout") c = {80, 180, 255, 255};
        else if (type == "star") c = {255, 210, 60, 255};
        else if (type == "switch" || type == "yswitch") c = {255, 100, 100, 255};
        else if (o.get("Filename").find("pipe") != std::string::npos) c = {140, 140, 160, 255};
        renderer_.drawPoint(o.x, o.y, c, 6);
    }

    // Spout mouths
    for (const auto& io : water_.objects()) {
        if (!io.spout.has_value()) continue;
        Vec2 m = io.spout->mouthWorld();
        renderer_.drawPoint(m.x, m.y, {0, 255, 255, 255}, 8);
    }

    // [DEBUG PARTICLE VISUALIZATION] — not original fluid render
    for (const auto& p : water_.particles()) {
        wmw::platform::Color c{100, 180, 255, 220};
        if (p.fluid == FluidType::Poison) c = {180, 80, 255, 220};
        else if (p.fluid == FluidType::Mud) c = {140, 100, 60, 220};
        else if (p.fluid == FluidType::Steam) c = {220, 220, 220, 180};
        renderer_.drawPoint(p.position.x, p.position.y, c, 3);
    }

    renderer_.end();
}

void Engine::shutdown() {
    if (!initialized_) return;
    std::cout << "[WaterDebug] Particles emitted (session): " << water_.totalEmitted() << "\n";
    audio_.shutdown();
    water_.shutdown();
    physics_.shutdown();
    std::cout << "[Engine] shutdown\n";
    initialized_ = false;
}

} // namespace wmw::core
