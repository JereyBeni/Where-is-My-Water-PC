#include "core/engine.hpp"
#include "core/assets.hpp"

#include <iostream>

namespace wmw::core {

bool Engine::init(const std::string& assetsRoot) {
    assetsRoot_ = assetsRoot;

    std::cout << "[Engine] init\n";
    std::cout << "[Engine] assets root confirmed: " << assetsRoot_ << "\n";

    // ------------------------------------------------------------------
    // STUB ZONE — do not invent real implementations yet
    // ------------------------------------------------------------------
    // TODO: study libwmw.so + Level XML format before implementing:
    //   - LevelLoader
    //   - Water / fluid simulation
    //   - Terrain / dig / sand / mud
    //   - Object system (pipes, ducks, switches, etc.)
    //   - Animation / skeleton system
    //   - Audio (original uses FMOD)
    //   - Actual game renderer (not this SDL clear-color)
    // ------------------------------------------------------------------

    std::cout << "[Engine] systems status:\n";
    std::cout << "  Platform/Window ........ OK (SDL2)\n";
    std::cout << "  Assets path check ...... OK\n";
    std::cout << "  Physics ................ STUB (not implemented)\n";
    std::cout << "  Water simulation ....... STUB (not implemented)\n";
    std::cout << "  Level loader ........... STUB (not implemented)\n";
    std::cout << "  Gameplay ............... STUB (not implemented)\n";
    std::cout << "  Audio (FMOD) ........... STUB (not implemented)\n";
    std::cout << "  Game renderer .......... STUB (SDL clear only)\n";

    initialized_ = true;
    return true;
}

void Engine::update(float /*dt*/) {
    if (!initialized_) return;
    // Future: fixed timestep, input, physics step, water step, gameplay
}

void Engine::render() {
    if (!initialized_) return;
    // Future: real game rendering pipeline
    // Currently the window layer just clears to a solid color.
}

void Engine::shutdown() {
    if (!initialized_) return;
    std::cout << "[Engine] shutdown\n";
    initialized_ = false;
}

} // namespace wmw::core
